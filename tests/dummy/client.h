//
// Created by pavlo on 28.06.24.
//

#ifndef MARS_DUMMY_CLIENT_H
#define MARS_DUMMY_CLIENT_H

#include "types.h"
#include "net/client.h"

typedef struct dummy_client_t {
    slice_t* reads;
    size_t reads_cap;
    size_t reads_head;
    slice_t* writes;
    size_t writes_cap;
    size_t writes_len;
    slice_t preserved;
} dummy_client_t;

dummy_client_t* dummy_client_new(slice_t* reads, size_t len);
net_client_t dummy_client_as_net(dummy_client_t*);
void dummy_client_free(dummy_client_t*);

#endif //MARS_DUMMY_CLIENT_H
