//
// Created by pavlo on 28.06.24.
//

#ifndef CLIENT_H
#define CLIENT_H

/*
* Client is a dynamically dispatched interface for various underlying protocols. It may
* a TCP socket, TLS, a mock, QUIC implementation, etc.
*/

#include <sys/types.h>
#include "types.h"

typedef struct net_status {
    bool ok;
    slice_t data;
} net_status;

typedef struct net_client {
    void* self;
    net_status (*read)(void*);
    int (*write)(void*, slice_t);
    void (*preserve)(void*, slice_t);
} net_client;

#endif //CLIENT_H
