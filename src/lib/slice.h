//
// Created by pavlo on 13-Jun-24.
//

#ifndef MARS_TYPES_H
#define MARS_TYPES_H

#include <inttypes.h>
#include <stdbool.h>
#include <sys/types.h>

#include "lib/array.h"

typedef uint8_t byte_t;

typedef ARRAY(byte_t) slice_t;

#define SLICE_NULL slice_new(NULL, 0)
slice_t slice_new(byte_t* ptr, size_t len);
slice_t slice_str(const char* str);
bool    slice_cmp(slice_t a, slice_t b);
bool    slice_cmpfold(slice_t a, slice_t b);
char*   slice_ascstr(slice_t self);
slice_t slice_clone(slice_t self);
size_t  slice_copyto(slice_t from, slice_t to);
slice_t slice_offset(slice_t self, ssize_t offset);
void    slice_print_escaped(slice_t self);
void    slice_free(slice_t self);

#endif //MARS_TYPES_H
