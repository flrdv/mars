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

#define EV_RESULT(s, p) (ev_result_t) { .next = s, .processed = p }

typedef enum {
    EV_ERROR    = 0b0000,
    EV_READ     = 0b0001,
    EV_WRITE    = 0b0010,
    EV_SERVER   = 0b0100,
    EV_INACTIVE = 0b1000
} ev_state_t;

typedef struct {
    ev_state_t next;
    size_t processed;
} ev_result_t;

typedef ev_result_t (*ev_closure_t)(void* env, net_client_t* client, slice_t data);

typedef struct {
    ev_closure_t read;
    ev_closure_t write;
    void (*free)(void*);
    void* env;
} ev_coroutine_t;

typedef struct {
    ev_state_t state;
    net_client_t client;
    ev_coroutine_t coro;
    slice_t preserved_read;
    slice_t preserved_write;
} ev_task_t;

typedef ev_coroutine_t (*ev_task_spawner_t)(net_client_t client);

typedef ARRAY(ev_task_t*) ev_disconnects_t;

typedef struct {
    size_t resume_at;
    size_t rbufflen;
    size_t wbufflen;
    reporter_t reporter;
    ev_disconnects_t disconnects;
    list_t events;
    arena_t readarena;
    arena_t writearena;
} ev_t;

typedef struct {
    bool queue_empty;
    ev_disconnects_t disconnects;
} ev_updates_t;

ev_t         ev_new(size_t max_disconnects, size_t rbufflen, size_t wbufflen, arena_t reads, arena_t writes);
ev_task_t    ev_task_new(net_client_t client, ev_coroutine_t coro);
ev_task_t    ev_server_task_new(void);
ev_updates_t ev_invoke(ev_t* self, size_t evc, ev_task_t* evs[]);

#endif //MARS_SRC_NET_EV_EVLOOP_H
