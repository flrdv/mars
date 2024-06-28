//
// Created by pavlo on 28.06.24.
//

#ifndef MARS_DUMMY_CLIENT_H
#define MARS_DUMMY_CLIENT_H

#include "types.h"
#include "net/client.h"

typedef struct dummy_client {
    slice_t* reads;
    size_t reads_count;
    slice_t* writes;
    size_t writes_count;
} dummy_client;



#endif //MARS_DUMMY_CLIENT_H
