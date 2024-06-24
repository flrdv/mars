//
// Created by floordiv on 22-Jun-24.
//

#ifndef MARS_KEYVAL_H
#define MARS_KEYVAL_H

#include "types.h"

typedef struct {
    slice_t key;
    slice_t value;
} keyval_pair_t;

/*
 * Stores key-value pairs internally and provides an interface for accessing them.
 * Works basically as a dynamic array, but according to the fact that only 1 element
 * can be appended per once, growing step is constant, i.e. growth graphic is linear
 */
typedef struct {
    keyval_pair_t* pairs;
    size_t len;
    size_t cap;
    size_t step;
} keyval_storage_t;

keyval_storage_t new_keyval(size_t prealloc, size_t step);
void keyval_append(keyval_storage_t* storage, slice_t key, slice_t value);
keyval_pair_t* keyval_get(keyval_storage_t* storage, slice_t key);
size_t keyval_len(keyval_storage_t* storage);
void keyval_clear(keyval_storage_t* storage);
void keyval_free(keyval_storage_t* storage);

#endif //MARS_KEYVAL_H
