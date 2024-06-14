//
// Created by floordiv on 13-Jun-24.
//

#ifndef MARS_BUFFER_H
#define MARS_BUFFER_H

#include "misc/types.h"

typedef struct {
    byte_t* mem;
    size_t offset, len, cap, max_cap;
} buffer_buffer_t;

typedef struct {
    byte_t* data;
    size_t len;
} buffer_segment_t;

#endif //MARS_BUFFER_H
