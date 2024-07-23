//
// Created by pavlo on 17.07.24.
//

#ifndef MARS_SRC_NET_EV_EPOLL_H
#define MARS_SRC_NET_EV_EPOLL_H

#include "evloop.h"

typedef ev_task_t (*task_spawner_t)(net_client_t client);

int ev_epoll_run(int sock, size_t readbuff, size_t writebuff, int max_events, task_spawner_t spawn);

#endif //MARS_SRC_NET_EV_EPOLL_H
