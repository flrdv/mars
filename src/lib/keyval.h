//
// Created by floordiv on 22-Jun-24.
//

#ifndef MARS_SRC_LIB_KEYVAL_H
#define MARS_SRC_LIB_KEYVAL_H

#include <sys/types.h>
#include "lib/slice.h"

typedef struct {
    slice_t key;
    slice_t value;
} keyval_pair_t;

/*
 * Stores key-value pairs internally and provides an interface for accessing them.
 * Works basically as a dynamic array, but according to the fact that only 1 element
 * can be appended per once, growing step is constant, i.e. growth graphic is linear
 */
typedef struct keyval {
    keyval_pair_t* pairs;
    size_t len;
    size_t cap;
    size_t step;
} keyval_t;

typedef struct {
    slice_t testkey;
    keyval_t* storage;
    size_t index;
} keyval_iterator_t;

keyval_t          keyval_new(size_t prealloc, size_t step);
void              keyval_append(keyval_t* self, slice_t key, slice_t value);
keyval_pair_t*    keyval_get(const keyval_t* self, slice_t key);
keyval_iterator_t keyval_values(keyval_t* self, slice_t key);
keyval_pair_t*    keyval_values_next(keyval_iterator_t* self);
size_t            keyval_len(const keyval_t* self);
void              keyval_clear(keyval_t* self);
void              keyval_free(const keyval_t* self);

#endif //MARS_SRC_LIB_KEYVAL_H
