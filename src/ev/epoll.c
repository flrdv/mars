//
// Created by pavlo on 17.07.24.
//

#include "epoll.h"
#include "net/client.h"
#include "net/tcp/client.h"

#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>

static bool ev_epoll_set_nonblocking(int sock) {
    return fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK) == 0;
}

int ev_epoll_run(int sock, size_t readbuff, size_t writebuff, int max_events, task_spawner_t spawn) {
    arena_t conns = arena_new(sizeof(ev_task_t)+sizeof(tcp_client_t), 1);

    // TODO: move this initializations somewhere out of here, into the toplevel
    const size_t max_disconnects = 1024;
    arena_t reads = arena_new(readbuff, 0);
    arena_t writes = arena_new(writebuff, 0);
    ev_t evloop = ev_new(max_disconnects, readbuff, writebuff, reads, writes);

    ev_task_t* server_task = arena_acquire(&conns);
    *server_task = ev_server_task_new();

    int epfd = epoll_create1(0);
    if (epfd == -1)
        return EV_EPOLL_COULD_NOT_SPAWN;

    if (!ev_epoll_set_nonblocking(sock))
        return EV_EPOLL_NBIO_NOT_AVAILABLE;

    struct epoll_event ev = { 0 };
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = server_task;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);

    int timeout = 0;

    for (;;) {
        ev_task_t* ev_events[max_events];
        size_t ev_events_len = 0;
        struct epoll_event events[max_events];
        int n = epoll_wait(epfd, events, max_events, timeout);

        for (int i = 0; i < n; i++) {
            struct epoll_event event = events[i];
            ev_task_t* task = event.data.ptr;

            if (task->state != EV_SERVER) {
                ev_events[ev_events_len++] = task;
                continue;
            }

            struct sockaddr_in addr;
            socklen_t addrlen = sizeof(addr);
            int conn = accept(sock, (struct sockaddr*)&addr, &addrlen);
            if (conn == -1) {
                // TODO: report EBADF, EINVAL, ENOTSOCK, EFAULT as bug. Otherwise, the state is
                // TODO: temporal, so simply skip the connection.
                close(conn);
                continue;
            }

            if (!ev_epoll_set_nonblocking(conn)) {
                // TODO: this must be a bug. Anyway, the error isn't related to server socket,
                // TODO: so just log it and forget about it.
                close(conn);
                continue;
            }

            void* conn_context = arena_acquire(&conns);
            ev_task_t* new_task = conn_context;
            tcp_client_t* client = conn_context + sizeof(ev_task_t);
            *client = tcp_client_new(conn, addr);
            net_client_t net_client = tcp_client_as_net(client);
            *new_task = ev_task_new(net_client, spawn(net_client));

            struct epoll_event conn_event = { 0 };
            conn_event.events = EPOLLIN | EPOLLOUT | EPOLLET;
            conn_event.data.ptr = new_task;
            epoll_ctl(epfd, EPOLL_CTL_ADD, conn, &conn_event);
        }

        ev_updates_t status = ev_invoke(&evloop, ev_events_len, ev_events);
        timeout = status.queue_empty ? -1 : 0;

        for (size_t i = 0; i < status.disconnects.cap; i++) {
            ev_task_t* task = status.disconnects.ptr[i];
            task->coro.free(task->coro.env);
            net_close(&task->client);
            // client's memory is automatically freed, too, as it's glued
            // with task altogether
            arena_release(&conns, task);
        }
    }
}
