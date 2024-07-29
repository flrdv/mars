//
// Created by pavlo on 16.07.24.
//


#include "ev.h"

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

ev_t ev_new(
    size_t max_disconnects, size_t rbufflen, size_t wbufflen, arena_t reads, arena_t writes
) {
    return (ev_t) {
        .resume_at = 0,
        .rbufflen = rbufflen,
        .wbufflen = wbufflen,
        .disconnected = (ev_disconnected_t) {
            .cap = max_disconnects,
            .ptr = malloc(max_disconnects * sizeof(ev_task_t*))
        },
        // TODO: pass preallocated events buffer?
        .events = list_new(sizeof(ev_task_t*)),
        // TODO: if read and write buffers share the same size, we can combine them
        .readarena = reads,
        .writearena = writes
    };
}

ev_task_t ev_task_new(net_client_t client, ev_coroutine_t coro) {
    return (ev_task_t) {
        .state = EV_READ,
        .client = client,
        .coro = coro
    };
}

ev_task_t ev_server_task_new(void) {
    return (ev_task_t) {
        .state = EV_SERVER
    };
}

#define EV_NOMEM /**/ (ev_dispatched_events_t) { .nomem = true }

typedef struct {
    bool nomem;
    size_t inactives;
    size_t new_events_processed;
    size_t disconnected;
} ev_dispatched_events_t;

/// processes all the pooled events and populates (replaces inactive) with new ones.
/// Returns a number of populated (and processed) new events. All the new events,
/// whose indexes are more or equal to returned number, are guaranteed to be left
/// intact.
static ev_dispatched_events_t ev_dispatch_events(ev_t* self, size_t evc, ev_task_t* evs[]) {
#define DEACTIVATE_TASK                                                  \
    do {                                                                 \
        if (evhead < evc) {                                              \
            task->state &= ~EV_INACTIVE;                                 \
            events[i] = evs[evhead++];                                   \
            break;                                                       \
        }                                                                \
                                                                         \
        ++inactives;                                                     \
        task->state |= EV_INACTIVE;                                      \
    } while (0)

#define CLOSE_TASK                                                       \
    do {                                                                 \
        DEACTIVATE_TASK;                                                 \
        if (task->preserved_read.len)                                    \
            arena_release(&self->readarena, task->preserved_read.ptr);   \
        if (task->preserved_write.len)                                   \
            arena_release(&self->writearena, task->preserved_write.ptr); \
        self->disconnected.ptr[disconnects++] = task;                    \
        if (disconnects >= self->disconnected.cap) {                     \
            self->resume_at = i;                                         \
            return (ev_dispatched_events_t) {                            \
                .nomem = false,                                          \
                .inactives = inactives,                                  \
                .new_events_processed = evhead,                          \
                .disconnected = disconnects                              \
            };                                                           \
        }                                                                \
    } while (0)

    int evhead = 0, inactives = 0, disconnects = 0;
    ev_task_t** events = self->events.ptr;

    byte_t* readbuff = arena_acquire(&self->readarena);
    if (readbuff == NULL)
        return EV_NOMEM;

    byte_t* writebuff = arena_acquire(&self->writearena);
    if (writebuff == NULL)
        return EV_NOMEM;

    size_t resume_at = self->resume_at;
    self->resume_at = 0;

    for (size_t i = resume_at; i < self->events.len; ) {
        ev_task_t* task = events[i];
        if (task == NULL) {
            report_bug(&self->reporter, "ev: encountered an invalid event. Skipping");
            continue;
        }

        if (task->state & EV_INACTIVE) {
            if (evhead < evc) {
                task->state &= ~EV_INACTIVE;
                events[i] = evs[evhead++];
            } else {
                ++inactives;
                ++i;
            }

            continue;
        }

        switch (task->state) {
        case EV_READ: {
            if (task->preserved_read.len) {
                ev_result_t status = task->coro.read(task->coro.env, task->preserved_read);
                if (status.next == EV_ERROR) {
                    // TODO: maybe, notify?
                    CLOSE_TASK;
                    break;
                }

                task->state = status.next;

                if (status.processed < task->preserved_read.len) {
                    task->preserved_read = slice_offset(task->preserved_read, status.processed);
                } else {
                    arena_release(&self->readarena, task->preserved_read.ptr);
                    task->preserved_read.len = 0;
                }
            } else {
                int n = net_read(&task->client, slice_new(readbuff, self->rbufflen));
                if (n == NET_EAGAIN) {
                    DEACTIVATE_TASK;
                    break;
                }

                if (n <= 0) {
                    // all other errors, including NET_EOF, should be treated in this manner
                    CLOSE_TASK;
                    break;
                }

                ev_result_t status = task->coro.read(task->coro.env, slice_new(readbuff, n));
                if (status.next == EV_ERROR) {
                    CLOSE_TASK;
                    break;
                }

                task->state = status.next;

                if (status.processed < n) {
                    task->preserved_read = slice_new(readbuff+status.processed, n-status.processed);
                    readbuff = arena_acquire(&self->readarena);
                    if (readbuff == NULL)
                        return EV_NOMEM;
                }
            }

            break;
        }
        case EV_WRITE: {
            // TODO: if buffer can hold a lot of data, pass it to the write coroutine to fit
            // TODO: more data in it, therefore reducing total write calls
            if (task->preserved_write.len) {
                int written = net_write(&task->client, task->preserved_write);
                if (written == NET_EAGAIN)
                    DEACTIVATE_TASK;
                else if (written < 0)
                    CLOSE_TASK;
                else if (written == task->preserved_write.len) {
                    arena_release(&self->writearena, task->preserved_write.ptr);
                    task->preserved_write.len = 0;
                }

                break;
            }

            ev_result_t status = task->coro.write(task->coro.env, slice_new(writebuff, self->wbufflen));
            if (status.next == EV_ERROR) {
                CLOSE_TASK;
                break;
            }

            task->state = status.next;

            int written = net_write(&task->client, slice_new(writebuff, status.processed));
            if (written == NET_EAGAIN)
                DEACTIVATE_TASK;
            else if (written < 0)
                CLOSE_TASK;
            else if (written < status.processed) {
                task->preserved_write = slice_new(writebuff+written, status.processed-written);
                writebuff = arena_acquire(&self->writearena);
                if (writebuff == NULL)
                    return EV_NOMEM;
            }

            break;
        }
        default:
            report_bug(&self->reporter, "ev: encountered an unexpected task state ({int}). Skipping", task->state);
            break;
        }

        ++i;
    }

    bool ok = arena_release(&self->readarena, readbuff) || arena_release(&self->writearena, writebuff);

    return (ev_dispatched_events_t) {
        .nomem = !ok,
        .inactives = inactives,
        .new_events_processed = evhead,
        .disconnected = disconnects
    };
}

/// dispatches all the pooled and newly added events.
/// Returned value tells the caller, whether active events pool is empty, therefore
/// in case it's empty a wrapper can wait _a bit longer_, knowing this won't disrupt
/// processing.
ev_updates_t ev_invoke(ev_t* self, size_t evc, ev_task_t* evs[]) {
    // TODO: we close all the connections in-place instead of explicitly detaching them,
    // TODO: as in case of epoll such behaviour is just perfectly fine. However, I strongly
    // TODO: doubt it's going to be fine for other mechanisms. Looks like we should be able
    // TODO: to tweak such behaviour somehow.
    ev_dispatched_events_t status = ev_dispatch_events(self, evc, evs);

    if (status.nomem) {
        // I assume this may happen so much rarely and under so much exotic conditions
        // (except virtual machines) that we are officially allowed to leave some kind
        // of easter egg here.
        report_emergency(&self->reporter, "ev: allocating memory failed. Halt and catch fire");
        // TODO: not the best way to quit. There is still so much work to be done, e.g.
        // TODO: closing actively all the connections, including server socket, etc.
        exit(1);
    }

    if (!list_append(&self->events, &evs[status.new_events_processed], evc-status.new_events_processed)) {
        report_emergency(&self->reporter, "ev: allocating memory failed. Halt and catch fire");
        // TODO: same goes here
        exit(1);
    }

    return (ev_updates_t) {
        .queue_empty = self->events.len <= status.inactives,
        .disconnects = (ev_disconnected_t) {
            .cap = status.disconnected,
            .ptr = self->disconnected.ptr
        }
    };
}
