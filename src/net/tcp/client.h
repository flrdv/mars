//
// Created by pavlo on 20.07.24.
//

#ifndef MARS_SRC_NET_TCP_CLIENT_H
#define MARS_SRC_NET_TCP_CLIENT_H

#include <netdb.h>

#include "net/client.h"

typedef struct {
    int fd;
    sockaddr_t remote;
    slice_t preserved;
} tcp_client_t;

tcp_client_t tcp_client_new(int fd, struct sockaddr_in remote);
net_client_t tcp_client_as_net(tcp_client_t* self);

#endif //MARS_SRC_NET_TCP_CLIENT_H
