//
// Created by pavlo on 28.06.24.
//

#ifndef MARS_NET_CLIENT_H
#define MARS_NET_CLIENT_H

#include "lib/slice.h"

#include <netdb.h>
#include <sys/types.h>

// EOF occurs when an operation is performed on a passively closed socket.
#define NET_EOF 0
#define NET_EAGAIN -1
#define NET_EPIPE -2

typedef struct net_client_t {
    void* self;
    int                (*read)(void*, slice_t);
    int                (*write)(void*, slice_t);
    void               (*preserve)(void*, slice_t);
    struct sockaddr_in (*remote)(void*);
    int                (*close)(void*);
    void               (*free)(void*);
} net_client_t;

/// Shortcut for `client->read(client->self)`.
static int net_read(net_client_t* client, slice_t buff) {
    return client->read(client->self, buff);
}

/// Shortcut for `client->write(client->self, data)`.
static ssize_t net_write(net_client_t* client, slice_t data) {
    return client->write(client->self, data);
}

/// Shortcut for `client->preserve(client->self, data)`.
static void net_preserve(net_client_t* client, slice_t data) {
    client->preserve(client->self, data);
}

/// Shortcut for `client->remote(client->self)`.
static struct sockaddr_in net_remote(net_client_t* client) {
    return client->remote(client->self);
}

/// Shortcut for `client->close(client->self)`.
static ssize_t net_close(net_client_t* client) {
    return client->close(client->self);
}

/// Shortcut for `client->free(client->self)`.
static void net_free(net_client_t* client) {
    return client->free(client->self);
}

#endif //MARS_NET_CLIENT_H
