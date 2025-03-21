//
// Created by pavlo on 02.08.24.
//


#ifdef __linux__

#include "ev.h"
#include "net/tcp/client.h"

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>

typedef struct {
    int sock;
    int epfd;
    reporter_t* reporter;
    size_t rbufflen, wbufflen;
    arena_t* tasks;
    arena_t* reads;
    arena_t* writes;
} ev_t;

static int ev_set_nb(int conn) {
    return fcntl(conn, F_SETFL, fcntl(conn, F_GETFL, 0) | O_NONBLOCK) == 0;
}

static int ev_accept(ev_t* self, int epfd, arena_t* tasks, ev_task_spawner_t spawn) {
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int conn = accept(self->sock, (struct sockaddr*)&addr, &addr_len);
    if (conn == -1) {
        // TODO: check whether the error depicts temporal or permanent state.
        return NET_EAGAIN;
    }

    // TODO: check for error in future
    ev_set_nb(conn);

    ev_task_t* task = arena_acquire(tasks);
    *task = (ev_task_t) {
        .state = EV_READ,
        .conn = tcp_client_new(conn),
        .coro = spawn()
    };

    struct epoll_event event = {
        .events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET,
        .data = { task },
    };

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, conn, &event) == -1) {
        // TODO: close connection and report an error.
    }
    return 0;
}

static int ev_accept_all(ev_t* self, int epfd, arena_t* tasks, ev_task_spawner_t spawn) {
    for (;;) {
        int n = ev_accept(self, epfd, tasks, spawn);
        if (n == 0)
            continue;

        // TODO: check for error different from NET_EAGAIN and return it

        return 0;
    }
}

static bool ev_has_preserved(slice_t preserved) {
    return preserved.len > 0;
}

/// Calls read closure and preserves unread data.
/// Returns bool, which depends on whether buffer was preserved.
static bool ev_coro_read(ev_task_t* task, slice_t buff) {
    ev_result_t status = task->coro.read(task->coro.env, buff);
    task->state = status.next;

    if (status.next != EV_ERROR && status.processed < buff.len) {
        task->preserved_read = slice_offset(buff, status.processed);
        return true;
    }

    return false;
}

static void ev_task_close(ev_t* self, ev_task_t* task) {
    if (ev_has_preserved(task->preserved_read))
        arena_release(self->reads, task->preserved_read.ptr);
    if (ev_has_preserved(task->preserved_write))
        arena_release(self->writes, task->preserved_write.ptr);

    // detaching connection from epoll is unfortunately necessary in order to function
    // properly:(
    epoll_ctl(self->epfd, EPOLL_CTL_DEL, task->conn.env.fd, NULL);
    task->coro.free(task->coro.env);
    task->conn.close(task->conn.env);
    arena_release(self->tasks, task);
}

typedef enum {
    EV_TASK_CONTINUE,
    EV_TASK_DISABLE,
    EV_TASK_CLOSE
} ev_task_status_t;

static ev_task_status_t ev_process(ev_t* self, ev_task_t* task) {
    if (ev_has_preserved(task->preserved_write)) {
        slice_t data = task->preserved_write;
        int n = task->conn.write(task->conn.env, data.ptr, data.len);
        // printf("%p: preserved write: %d; ", task, n);
        if (n == NET_EAGAIN)
            return EV_TASK_DISABLE;
        if (n <= 0)
            return EV_TASK_CLOSE;

        task->preserved_write = slice_offset(data, n);
        if (n >= data.len)
            arena_release(self->writes, data.ptr);

        return EV_TASK_CONTINUE;
    }

    switch (task->state) {
    case EV_READ: {
        if (ev_has_preserved(task->preserved_read)) {
            bool preserved = ev_coro_read(task, task->preserved_read);
            if (!preserved)
                arena_release(self->reads, task->preserved_read.ptr);

            return EV_TASK_CONTINUE;
        }

        byte_t* buff = arena_acquire(self->reads);
        int n = task->conn.read(task->conn.env, buff, self->rbufflen);
        if (n == NET_EAGAIN) {
            printf("%p EAGAIN\n", task);
            arena_release(self->reads, buff);
            return EV_TASK_DISABLE;
        }

        if (n <= 0) {
            arena_release(self->reads, buff);
            return EV_TASK_CLOSE;
        }

        bool preserved = ev_coro_read(task, slice_new(buff, n));
        if (!preserved)
            arena_release(self->reads, buff);

        return EV_TASK_CONTINUE;
    }
    case EV_WRITE: {
        byte_t* buff = arena_acquire(self->writes);
        ev_result_t status = task->coro.write(task->coro.env, slice_new(buff, self->wbufflen));
        task->state = status.next;
        int n = task->conn.write(task->conn.env, buff, status.processed);
        if (n == NET_EAGAIN) {
            printf("write EAGAIN\n");
            task->preserved_write = slice_new(buff, status.processed);
            return EV_TASK_DISABLE;
        }

        if (n <= 0) {
            arena_release(self->writes, buff);
            return EV_TASK_CLOSE;
        }

        if (n < status.processed)
            task->preserved_write = slice_new(buff+n, status.processed-n);
        else
            arena_release(self->writes, buff);

        return EV_TASK_CONTINUE;
    }
    default:
        report_bug(self->reporter, "{str}: unexpected task state: {int}", __FILE__, task->state);
        return EV_TASK_CLOSE;
    }
}

static size_t ev_process_cached(ev_t* self, list_t* evcache, ev_task_t** replacements, size_t rcap) {
    ev_task_t** events = evcache->ptr;
    size_t rhead = 0;
    ssize_t last_disabled_task = -1;

    for (ssize_t i = 0; i < evcache->len; i++) {
        ev_task_t* task = events[i];
        if (task == NULL) {
            if (rhead >= rcap) {
                last_disabled_task = i;
                continue;
            }

            task = replacements[rhead++];
            events[i] = task;
        }

        last_disabled_task = -1;
        ev_task_status_t status = ev_process(self, task);
        switch (status) {
        case EV_TASK_CONTINUE:
            // printf("intact\n");
            break;
        case EV_TASK_DISABLE:
            if (rhead < rcap) {
                events[i--] = replacements[rhead++];
                break;
            }

            events[i] = NULL;
            last_disabled_task = i;
            break;
        case EV_TASK_CLOSE:
            ev_task_close(self, task);

            if (rhead < rcap) {
                events[i--] = replacements[rhead++];
                break;
            }

            events[i] = NULL;
            last_disabled_task = i;
            break;
        default:
            report_bug(self->reporter, "{str}: got unexpected cached task processing status: {int}.", __FILE__, status);
            ev_task_close(self, task);
        }
    }

    if (last_disabled_task != -1) {
        list_rstrip(evcache, evcache->len-last_disabled_task);
    }

    return rhead;
}

int ev_run(
    int sock, reporter_t* reporter, size_t rbufflen, size_t wbufflen,
    size_t rprealloc, size_t wprealloc, ev_task_spawner_t spawn
) {
    if (!ev_set_nb(sock))
        return EV_ENBIO;

    int epfd = epoll_create1(0);
    if (epfd == -1)
        return EV_ESPAWN;

    arena_t tasks = arena_new(sizeof(ev_task_t), 1024);
    list_t evcache = list_alloc(sizeof(ev_task_t*), 1024);
    arena_t reads = arena_new(rbufflen, rprealloc);
    arena_t writes = arena_new(wbufflen, wprealloc);

    ev_t self = (ev_t) {
        .sock = sock,
        .epfd = epfd,
        .reporter = reporter,
        .rbufflen = rbufflen,
        .wbufflen = wbufflen,
        .tasks = &tasks,
        .reads = &reads,
        .writes = &writes
    };

    ev_task_t* server_task = arena_acquire(&tasks);
    *server_task = (ev_task_t) {
        // TODO: add in future some way to differentiate different server sockets
        .state = EV_ACCEPT
    };

    struct epoll_event ev = { 0 };
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = server_task;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);
    int timeout = -1;

    for (;;) {
        const size_t EPOLL_EVENTS = 1024;
        size_t cached_tasks_len = 0;
        ev_task_t* cached_tasks[EPOLL_EVENTS];
        struct epoll_event evs[EPOLL_EVENTS];

        int n = epoll_wait(epfd, evs, EPOLL_EVENTS, timeout);
        if (n == -1) {
            report_bug(reporter, "{str}: epoll returned error (errno: {int}).", __FILE__, errno);
            continue;
        }

        printf("epoll: %d new events, %lu cached events\n", n, evcache.len);

        // printf("fresh loop:\n");

        for (size_t i = 0; i < n; i++) {
            struct epoll_event event = evs[i];
            ev_task_t* task = event.data.ptr;
            uint32_t flags = event.events;
            printf("%p: %d (in=%d out=%d rdhup=%d)\n", task, flags, (flags&EPOLLIN)!=0, (flags&EPOLLOUT)!=0, (flags&EPOLLRDHUP)!=0);
            if (task->state == EV_ACCEPT) {
                // printf("accept\n");
                if (ev_accept_all(&self, epfd, &tasks, spawn) != 0) { //NOLINT: one day it may become required
                    // TODO: report error
                }

                continue;
            }

            if (event.events&EPOLLRDHUP)
                continue;

            for (size_t j = 0; j < evcache.len; j++) {
                if (task == *(ev_task_t**)list_access(&evcache, j))
                    printf("%p: duplicate event\n", task);
            }

            ev_task_status_t status = ev_process(&self, task);
            switch (status) {
            case EV_TASK_CONTINUE:
                // printf("caching\n");
                cached_tasks[cached_tasks_len++] = task;
                break;
            case EV_TASK_DISABLE:
                // printf("omitting\n");
                // problem has disappeared by itself. Everything is just fine
                break;
            case EV_TASK_CLOSE:
                // printf("closing in-place\n");
                ev_task_close(&self, task);
                break;
            default:
                report_bug(reporter, "{str}: got unexpected task processing status: {int}.", __FILE__, status);
                ev_task_close(&self, task);
            }
        }

        // printf("cached loop:\n");

        size_t processed = ev_process_cached(&self, &evcache, cached_tasks, cached_tasks_len);
        // append the rest of the unprocessed tasks back. Unfortunately, they must have until the next
        // round to be processed
        list_append(&evcache, &cached_tasks[processed], cached_tasks_len-processed);
        // wait until new events if there are no cached ones
        timeout = evcache.len ? 0 : -1;
    }
}

#endif //__linux__
