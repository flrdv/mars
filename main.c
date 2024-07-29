#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib/list.h"
#include "lib/slice.h"
#include "ev/epoll.h"
#include "ev/ev.h"
#include "net/client.h"

typedef struct {
    list_t read;
} async_env_t;

ev_result_t async_read(void* env, net_client_t* client, slice_t data) {
    async_env_t* self = env;

    for (size_t i = 0; i < data.len; i++) {
        if (data.ptr[i] == 0) {
            list_append(&self->read, data.ptr, i+1);
            printf("async_read: message: %s\n", (char*)self->read.ptr);
            list_pop(&self->read);

            return EV_RESULT(EV_WRITE, i+1);
        }
    }

    list_append(&self->read, data.ptr, data.len);

    return EV_RESULT(EV_READ, data.len);
}

ev_result_t async_write(void* env, net_client_t* client, slice_t buff) {
    printf("async_write\n");
    async_env_t* self = env;
    size_t datalen = self->read.len;
    memcpy(buff.ptr, self->read.ptr, datalen);
    list_clear(&self->read);

    return EV_RESULT(EV_READ, datalen);
}

void async_free(void* env) {
    async_env_t* self = env;
    list_free(&self->read);
    free(env);
}

ev_coroutine_t async_task_spawner(net_client_t client) {
    async_env_t* env = malloc(sizeof(async_env_t)); //NOLINT
    *env = (async_env_t) {
        .read = list_new(sizeof(byte_t))
    };

    return (ev_coroutine_t) {
        .read = async_read,
        .write = async_write,
        .free = async_free,
        .env = env
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
        printf("main: bind socket: TIME_WAIT\n");
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
