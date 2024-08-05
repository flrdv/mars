//
// Created by pavlo on 20.07.24.
//

#include "client.h"

#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

static ssize_t tcp_read(net_env_t env, void* buff, size_t len) {
    ssize_t n = read(env.fd, buff, len);
    if (n < 0)
        switch (errno) {
        case EAGAIN: return NET_EAGAIN;
        default:     return NET_EPIPE;
        }

    return n;
}

static ssize_t tcp_write(net_env_t env, void* buff, size_t len) {
    ssize_t n = write(env.fd, buff, len);
    if (n < 0)
        switch (errno) {
        case EAGAIN: return NET_EAGAIN;
        default: return NET_EPIPE;
        }

    return n;
}

static void tcp_close(net_env_t env) {
    close(env.fd);
}

net_client_t tcp_client_new(int fd) {
    return (net_client_t) {
        .env = fd,
        .read = tcp_read,
        .write = tcp_write,
        .close = tcp_close
    };
}
