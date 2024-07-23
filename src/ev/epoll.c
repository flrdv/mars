//
// Created by pavlo on 17.07.24.
//

#include "epoll.h"
#include "net/client.h"
#include "net/tcp/client.h"

#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#define EV_EPOLL_OK 0
#define EV_EPOLL_COULD_NOT_SPAWN 1
#define EV_EPOLL_CANNOT_LISTEN 2

static bool ev_epoll_set_nonblocking(int sock) {
    return fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK) == 0;
}

int ev_epoll_run(int sock, size_t readbuff, size_t writebuff, const int max_events, task_spawner_t spawn) {
    evloop_t evloop = evloop_new(readbuff, writebuff);
    int epfd = epoll_create1(0);
    if (epfd == -1)
        return EV_EPOLL_COULD_NOT_SPAWN;

    // first set sock non-blocking. As far as API doesn't care about socket modes,
    // we must ensure it is set.
    assert(ev_epoll_set_nonblocking(sock) /* failed to set the socket to non-blocking mode. */);
    struct epoll_event ev = { 0 };
    ev.events = EPOLLIN | EPOLLET;
    ev.data.u64 = (uint64_t)-1;
    epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &ev);

    int timeout = 0;

    for (;;) {
        ev_event_t events[max_events];
        size_t ev_event_len = 0;
        struct epoll_event epevents[max_events];
        printf("epoll: waiting, timeout=%d\n", timeout);
        int n = epoll_wait(epfd, epevents, max_events, timeout);

        for (int i = 0; i < n; i++) {
            ev_event_t task_index = (int64_t)epevents[i].data.u64;

            if (task_index < 0) {
                struct sockaddr_in addr;
                socklen_t addrlen = sizeof(addr);
                printf("epoll: accepting new connection...");
                int conn = accept(sock, (struct sockaddr*)&addr, &addrlen);
                if (conn == -1) {
                    // TODO: check whether the occurred error is temporal. If it's
                    // TODO: permanent, we must shut down the eventloop and return an error
                    assert(false /* failed to accept a connection */);
                }
                printf("done!\n");

                if (!ev_epoll_set_nonblocking(conn)) {
                    // TODO: this must be a bug. Anyway, the error isn't related to server socket,
                    // TODO: so just log it and forget about it.
                    assert(false /* failed to set the connection to non-blocking mode */);
                }

                tcp_client_t* tcp_client = tcp_client_new(conn, addr);
                net_client_t client = tcp_client_as_net(tcp_client);
                uint64_t index = evloop_assign(&evloop, spawn(client));
                printf("epoll: assigned a new task id: %lu\n", index);

                struct epoll_event conn_event = { 0 };
                conn_event.events = EPOLLIN | EPOLLOUT | EPOLLET;
                conn_event.data.u64 = index;
                epoll_ctl(epfd, EPOLL_CTL_ADD, conn, &conn_event);
                continue;
            }

            events[ev_event_len++] = task_index;
        }

        bool wait_longer = evloop_invoke(&evloop, events, ev_event_len);
        timeout = wait_longer? -1 : 0;
    }
}
