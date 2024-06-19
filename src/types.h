//
// Created by pavlo on 13-Jun-24.
//

#ifndef MARS_TYPES_H
#define MARS_TYPES_H

#include <inttypes.h>
#include <stddef.h>

typedef uint8_t byte_t;

typedef struct {
    byte_t* data;
    size_t len;
} slice_t;

slice_t new_slice(byte_t* data, size_t len);

#endif //MARS_TYPES_H


