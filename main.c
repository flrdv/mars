#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ev/ev.h"
#include "lib/slice.h"
#include "net/client.h"

typedef struct {
    size_t len;
    byte_t* ptr;
} async_env_t;

ev_result_t async_read(void* env, slice_t data) {
    async_env_t* self = env;

    for (size_t i = 0; i < data.len; i++) {
        if (data.ptr[i] == 0) {
            memcpy(self->ptr+self->len, data.ptr, i);
            self->len += i;

            return EV_RESULT(EV_WRITE, i+1);
        }
    }

    memcpy(self->ptr+self->len, data.ptr, data.len);
    self->len += data.len;

    return EV_RESULT(EV_READ, data.len);
}

ev_result_t async_write(void* env, slice_t buff) {
    async_env_t* self = env;
    size_t len = self->len;
    self->len = 0;
    memcpy(buff.ptr, self->ptr, len);

    return EV_RESULT(EV_READ, len);
}

void async_free(void* env) {
    // buffer is anyway allocated in the same memory block
    free(env);
}

ev_coroutine_t async_task_spawner(void) {
    void* env = malloc(sizeof(async_env_t)+1024); //NOLINT
    *(async_env_t*)env = (async_env_t) {
        .len = 0,
        .ptr = env+sizeof(async_env_t)
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
    ev_run(sock, NULL, read_buff_size, write_buff_size, 8, 8, async_task_spawner);
    return 0;
}
