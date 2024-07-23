//
// Created by pavlo on 16.07.24.
//

#ifndef MARS_SRC_NET_EV_EVLOOP_H
#define MARS_SRC_NET_EV_EVLOOP_H

#include "lib/list.h"
#include "lib/slice.h"
#include "net/client.h"

typedef enum {
    CORO_CONTINUE,
    CORO_DONE,
    CORO_ERROR
} ev_flow_t;

#define EV_READ_RESULT(s, r)  (ev_read_result_t)  { .status = s, .read = r    }
#define EV_WRITE_RESULT(s, w) (ev_write_result_t) { .status = s, .written = w }

typedef struct {
    ev_flow_t status;
    size_t read;
} ev_read_result_t;

typedef ev_read_result_t (*ev_read_closure_t)(void* env, net_client_t* client, slice_t data);

typedef struct {
    ev_flow_t status;
    size_t written;
} ev_write_result_t;

typedef ev_write_result_t (*ev_write_closure_t)(void* env, net_client_t* client, slice_t buff);

typedef enum {
    READ,
    WRITE,
    INACTIVE
} ev_state_t;

typedef struct {
    ev_state_t state;
    net_client_t client;
    void* env;
    ev_read_closure_t read;
    ev_write_closure_t write;
} ev_task_t;

/// event is simply an index of a task. Only positive values are used, negative
/// are reserved for events sources
typedef int64_t ev_event_t;

typedef struct {
    list_t tasks, events;
    size_t read_buff, write_buff;
} evloop_t;

evloop_t evloop_new(size_t read_buff_size, size_t write_buff_size);
uint64_t evloop_assign(evloop_t* self, ev_task_t task);
bool     evloop_invoke(evloop_t* self, ev_event_t* evs, int evc);

#endif //MARS_SRC_NET_EV_EVLOOP_H
