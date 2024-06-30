//
// Created by pavlo on 13-Jun-24.
//

#ifndef MARS_TYPES_H
#define MARS_TYPES_H

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

typedef uint8_t byte_t;

#define SLICE_NULL slice_new(NULL, 0)

typedef struct {
    size_t len;
    byte_t* data;
} slice_t;

slice_t slice_new(byte_t* data, size_t len);
bool slice_cmp(slice_t a, slice_t b);
char* slice_ascstr(slice_t self);
slice_t slice_clone(slice_t self);

#endif //MARS_TYPES_H


