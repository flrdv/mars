//
// Created by pavlo on 28.06.24.
//

#ifndef MARS_NET_CLIENT_H
#define MARS_NET_CLIENT_H

/*
* Client is a dynamically dispatched interface for various underlying protocols. It may
* a TCP socket, TLS, a mock, QUIC implementation, etc.
*/

#include <sys/types.h>
#include "types.h"

#define NET_OK 0
#define NET_EOF 1

#define NET_STATUS(s, d) (net_status) { .errno = s, .data = d }

typedef struct net_status {
    ssize_t errno;
    slice_t data;
} net_status;

typedef struct net_client {
    void* self;
    net_status (*read)(void*);
    int (*write)(void*, slice_t);
    void (*preserve)(void*, slice_t);
    ssize_t (*close)(void*);
} net_client;

#endif //MARS_NET_CLIENT_H
