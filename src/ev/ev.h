//
// Created by pavlo on 16.07.24.
//

#ifndef MARS_SRC_NET_EV_EVLOOP_H
#define MARS_SRC_NET_EV_EVLOOP_H

#include "lib/list.h"
#include "lib/slice.h"
#include "lib/arena.h"
#include "net/client.h"
#include "report/report.h"

#define EV_OK       0
#define EV_ESPAWN  -1 // could not spawn. For example, epoll instantiation failed
#define EV_ELISTEN -2 // could not listen. Can occur for various reasons
#define EV_ENBIO   -3 // non-blocking IO error. Must never happen

#define EV_RESULT(s, p) (ev_result_t) { .next = s, .processed = p }

typedef enum {
    EV_ERROR,
    EV_READ,
    EV_WRITE,
    EV_ACCEPT,
} ev_state_t;

typedef struct {
    ev_state_t next;
    size_t processed;
} ev_result_t;

typedef ev_result_t (*ev_closure_t)(void* env, slice_t buff);

typedef struct {
    ev_closure_t read;
    ev_closure_t write;
    void (*free)(void* env);
    void* env;
} ev_coroutine_t;

typedef struct {
    ev_state_t state;
    net_client_t conn;
    ev_coroutine_t coro;
    slice_t preserved_read;
    slice_t preserved_write;
} ev_task_t;

typedef ev_coroutine_t (*ev_task_spawner_t)(void);

int ev_run(
    int sock, reporter_t* reporter, size_t rbufflen, size_t wbufflen,
    size_t rprealloc, size_t wprealloc, ev_task_spawner_t spawn
);

#endif //MARS_SRC_NET_EV_EVLOOP_H
