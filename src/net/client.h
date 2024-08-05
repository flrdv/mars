//
// Created by pavlo on 28.06.24.
//

#ifndef MARS_NET_CLIENT_H
#define MARS_NET_CLIENT_H

#include <netdb.h>
#include <sys/types.h>

// EOF occurs when an operation is performed on a passively closed socket.
#define NET_EOF 0
#define NET_EAGAIN -1
#define NET_EPIPE -2

typedef struct sockaddr_in sockaddr_t;

typedef union {
    int fd;
    void* ptr;
} net_env_t;

typedef struct {
    net_env_t env;
    ssize_t (*read)(net_env_t env, void* buff, size_t len);
    ssize_t (*write)(net_env_t env, void* buff, size_t len);
    void    (*close)(net_env_t env);
} net_client_t;

#endif //MARS_NET_CLIENT_H
