#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/list.h"
#include "lib/slice.h"
#include "ev/epoll.h"
#include "ev/evloop.h"
#include "net/client.h"

typedef struct {
    list_t read;
} async_env_t;

ev_read_result_t async_read(void* env, net_client_t* client, slice_t data) {
    printf("ASYNC_READ\n");
    async_env_t* self = env;
    if (!list_append_many(&self->read, data.ptr, data.len)) {
        printf("async_read: error: cannot append more data!");
        return EV_READ_RESULT(CORO_ERROR, 0);
    }

    for (size_t i = 0; i < data.len; i++) {
        if (data.ptr[i] == 0) {
            printf("async_read: message: %s\n", (char*)self->read.ptr);

            return EV_READ_RESULT(CORO_DONE, i);
        }
    }

    return EV_READ_RESULT(CORO_CONTINUE, 0);
}

ev_write_result_t async_write(void* env, net_client_t* client, slice_t buff) {
    printf("ASYNC_WRITE\n");
    async_env_t* self = env;
    size_t n = slice_copyto(slice_new(self->read.ptr, self->read.len), buff);
    if (n == self->read.len) {
        return EV_WRITE_RESULT(CORO_DONE, n);
    }

    return EV_WRITE_RESULT(CORO_CONTINUE, n);
}

ev_task_t async_task_spawner(net_client_t client) {
    async_env_t* env = malloc(sizeof(async_env_t));
    *env = (async_env_t) {
        .read = list_new(sizeof(byte_t))
    };

    return (ev_task_t) {
        .state = READ,
        .client = client,
        .env = env,
        .read = async_read,
        .write = async_write
    };
}

int main(void) {
    const int read_buff_size = 1024;
    const int write_buff_size = 1024;

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("main: create socket: failed somehow\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in addr = { 0 };
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9090);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        printf("main: bind socket: failed idk how\n");
        return EXIT_FAILURE;
    }

    if (listen(sock, 5) != 0) {
        printf("main: listen socket: failed in the most magical way\n");
        return EXIT_FAILURE;
    }

    printf("running\n");
    ev_epoll_run(sock, read_buff_size, write_buff_size, 1024, async_task_spawner);
    return 0;
}
