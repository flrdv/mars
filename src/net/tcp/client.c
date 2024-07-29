//
// Created by pavlo on 20.07.24.
//

#include "client.h"
#include "lib/arena.h"

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

tcp_client_t tcp_client_new(int fd, struct sockaddr_in remote) {
    return (tcp_client_t) {
        .fd = fd,
        .remote = remote
    };
}

static int tcp_read(void* s, slice_t buff) {
    tcp_client_t* self = s;
    if (self->preserved.len) {
        ssize_t copied = slice_copyto(self->preserved, buff);
        self->preserved = slice_offset(self->preserved, copied);

        return copied;
    }

    int n = read(self->fd, buff.ptr, buff.len);
    if (n == 0)
        return NET_EOF;
    if (n == -1)
        switch (errno) {
        case EAGAIN: return NET_EAGAIN;
        // TODO: handle other interesting errors, too. Therefore, some
        // TODO: interesting and rare errors can be logged, which will be a good
        // TODO: starting point in troubleshooting
        default: return NET_EPIPE;
        }

    return n;
}

static int tcp_write(void* s, slice_t data) {
    tcp_client_t* self = s;
    int written = write(self->fd, data.ptr, data.len);
    if (written == -1)
        switch (errno) {
        case EAGAIN: return NET_EAGAIN;
        default:     return NET_EPIPE;
        }

    // man says, that zero of written bytes on non-regular files is not specified.
    // So simply ignoring a zero-value it must be just fine
    return written;
}

static void tcp_preserve(void* s, slice_t data) {
    tcp_client_t* self = s;
    self->preserved = data;
}

static int tcp_set_nonblocking(void* s) {
    tcp_client_t* self = s;
    return fcntl(self->fd, F_SETFL, fcntl(self->fd, F_GETFL, 0) | O_NONBLOCK) == 0;
}

static sockaddr_t tcp_remote(void* s) {
    tcp_client_t* self = s;
    return self->remote;
}

static int tcp_close(void* s) {
    tcp_client_t* self = s;

    // TODO: returned value isn't platform-uniform. Introduce all the same generic
    // TODO: error codes here, too
    return close(self->fd);
}

net_client_t tcp_client_as_net(tcp_client_t* self) {
    return (net_client_t) {
        .env = self,
        .read = tcp_read,
        .write = tcp_write,
        .preserve = tcp_preserve,
        .set_nonblocking = tcp_set_nonblocking,
        .remote = tcp_remote,
        .close = tcp_close,
    };
}
