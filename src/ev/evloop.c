//
// Created by pavlo on 16.07.24.
//

#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>

#include "evloop.h"

#include <stdio.h>

evloop_t evloop_new(size_t read_buff_size, size_t write_buff_size) {
    return (evloop_t) {
        .tasks = list_new(sizeof(ev_task_t)),
        .events = list_new(sizeof(ev_event_t)),
        .read_buff = read_buff_size,
        .write_buff = write_buff_size
    };
}

uint64_t evloop_assign(evloop_t* self, ev_task_t task) {
    list_append(&self->tasks, &task);
    return self->tasks.len-1;
}

typedef struct {
    int inactives;
    int new_events_procesesed;
} dispatched_events_t;

/// processes all the pooled events and populates (replaces inactive) with new ones.
/// Returns a number of populated (and processed) new events. All the new events,
/// whose indexes are more or equal to returned number, are guaranteed to be left
/// intact.
static dispatched_events_t evloop_dispatch_events(evloop_t* self, ev_event_t* evs, int evc) {
#define DEACTIVE_EVENT                         \
    do {                                       \
        if (evhead < evc) {                    \
            events[i] = evs[evhead++];         \
            continue;                          \
        }                                      \
                                               \
        ++inactives  ;                         \
        task->state = INACTIVE;                \
    } while (0)

    int evhead = 0, inactives = 0;
    ev_event_t* events = self->events.ptr;

    for (size_t i = 0; i < self->events.len; ) {
        ev_event_t event = events[i];
        // we can remove this assertation and catch such bugs in the if below.
        assert(event >= 0 /* BUG: event is a task index, which therefore can't be negative */);
        ev_task_t* task = list_access(&self->tasks, event);
        if (task == NULL)
            assert(false /* BUG: we received an invalid event. Report as a bug. */);
        
        switch (task->state) {
        case READ: {
            byte_t buff[self->read_buff];
            int n = net_read(&task->client, slice_new(buff, self->read_buff));
            if (n == NET_EAGAIN) {
                DEACTIVE_EVENT;
                break;
            }

            if (n <= 0) {
                // all other errors, including NET_EOF, should be treated in this manner
                net_close(&task->client);
                net_free(&task->client);
                DEACTIVE_EVENT;
                break;
            }

            ev_read_result_t status = task->read(task->env, &task->client, slice_new(buff, n));
            switch (status.status) {
            case CORO_CONTINUE:
                // TODO: process here (and in CORO_DONE) saving returned extra data
                break;
            case CORO_DONE:
                task->state = WRITE;
                break;
            case CORO_ERROR:
                // when coroutine returns an error, it handles it by itself at first.
                // Therefore, the only thing is left to do is to close the connection
                net_close(&task->client);
                net_free(&task->client);
                DEACTIVE_EVENT;
                break;
            }

            break;
        }
        case WRITE: {
            byte_t buff[self->write_buff];
            ev_write_result_t status = task->write(task->env, &task->client, slice_new(buff, self->write_buff));
            switch (status.status) {
            case CORO_CONTINUE: {
                // TODO: save somewhere data, that could not be written (written bytes amount
                // TODO: may be smaller than was initially given).
                int written = net_write(&task->client, slice_new(buff, status.written));
                if (written == NET_EAGAIN) {
                    DEACTIVE_EVENT;
                } else if (written < 0) {
                    net_close(&task->client);
                    net_free(&task->client);
                    DEACTIVE_EVENT;
                }

                break;
            }
            case CORO_DONE:
                int written = net_write(&task->client, slice_new(buff, status.written));
                if (written == NET_EAGAIN) {
                    DEACTIVE_EVENT;
                    break;
                }
                if (written < 0) {
                    net_close(&task->client);
                    net_free(&task->client);
                    DEACTIVE_EVENT;
                    break;
                }

                task->state = READ;
                break;
            case CORO_ERROR:
                net_close(&task->client);
                net_free(&task->client);
                DEACTIVE_EVENT;
                break;
            }

            break;
        }
        case INACTIVE:
            if (evhead < evc) {
                events[i] = evs[evhead++];
                continue;
            }

            ++inactives;
            // if there are no new events to populate, just skip this
            break;
        }

        ++i;
    }

    return (dispatched_events_t) {
        .inactives = inactives,
        .new_events_procesesed = evhead
    };
}

/// dispatches all the pooled and newly added events.
/// Returned value tells the caller, whether active events pool is empty, therefore
/// in case it's empty a wrapper can wait _a bit longer_, knowing this won't disrupt
/// processing.
bool evloop_invoke(evloop_t* self, ev_event_t* evs, int evc) {
    // TODO: instead of explicity detaching connections, we just close them in-place
    // TODO: without any further notifications for caller. This works fine with epoll,
    // TODO: however it's a mechanism-dependent behavior, which may not work in general case
    int evhead = 0, inactives;

    for (;;) {
        // if list fails to append more elements, loop until some events are freed
        dispatched_events_t status = evloop_dispatch_events(self, evs, evc);
        evhead += status.new_events_procesesed;
        inactives = status.inactives;

        if (list_append_many(&self->events, &evs[evhead], evc-evhead))
            break;
    }

    return self->events.len <= inactives;
}
