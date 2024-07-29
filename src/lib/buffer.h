//
// Created by pavlo on 13-Jun-24.
//

#ifndef MARS_BUFFER_H
#define MARS_BUFFER_H

#include "slice.h"

#include <stdint.h>

typedef struct {
    byte_t* mem;
    size_t offset, len, cap, max_cap;
} buffer_t;

buffer_t buffer_new(size_t prealloc, size_t max_cap);
void     buffer_free(buffer_t* buff);
bool     buffer_append(buffer_t* buff, const byte_t* data, size_t size);
bool     buffer_append_char(buffer_t* buff, const char* data, size_t size);
slice_t  buffer_segment(buffer_t* buff);
size_t   buffer_segment_len(buffer_t* buff);
void     buffer_clear(buffer_t* buff);

#endif //MARS_BUFFER_H
