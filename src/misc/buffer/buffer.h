//
// Created by floordiv on 13-Jun-24.
//

#ifndef MARS_BUFFER_H
#define MARS_BUFFER_H

#include <stdbool.h>
#include "misc/types.h"

typedef struct {
    byte_t* mem;
    size_t offset, len, cap, max_cap;
} buffer_buffer_t;

typedef struct {
    byte_t* data;
    size_t len;
} buffer_segment_t;

buffer_buffer_t buffer_new(uint32_t prealloc, uint32_t max_cap);

void buffer_free(buffer_buffer_t* buff);

bool buffer_append(buffer_buffer_t* buff, byte_t* data, size_t size);

buffer_segment_t buffer_segment(buffer_buffer_t* buff);

size_t buffer_segment_len(buffer_buffer_t* buff);

#endif //MARS_BUFFER_H