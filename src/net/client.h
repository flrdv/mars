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

typedef struct sockaddr_in sockaddr_t;

typedef struct net_client_t {
    int        (*read)(void*, slice_t);
    int        (*write)(void*, slice_t);
    void       (*preserve)(void*, slice_t);
    int        (*set_nonblocking)(void*);
    sockaddr_t (*remote)(void*);
    int        (*close)(void*);
    void*      env;
} net_client_t;

/// Shortcut for `self->read(self->env)`.
int net_read(const net_client_t* self, slice_t buff);

/// Shortcut for `self->write(self->env, data)`.
int net_write(const net_client_t* self, slice_t data);

/// Shortcut for `self->preserve(self->env, data)`.
void net_preserve(const net_client_t* self, slice_t data);

/// Shortcut for `self->remote(self->env)`.
sockaddr_t net_remote(const net_client_t* self);

/// Shortcut for `self->close(self->env)`.
int net_close(const net_client_t* self);

#endif //MARS_NET_CLIENT_H
