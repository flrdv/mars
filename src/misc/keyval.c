//
// Created by floordiv on 22-Jun-24.
//

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "keyval.h"

keyval_storage_t new_keyval(size_t prealloc, size_t step) {
    return (keyval_storage_t) {
        .pairs = malloc(prealloc),
        .len = 0,
        .cap = prealloc,
        .step = step,
    };
}

static void keyval_grow_(keyval_storage_t* storage) {
    size_t new_cap = storage->cap + storage->step;
    keyval_pair_t* new_mem = malloc(new_cap);
    memcpy(new_mem, storage->pairs, storage->len);
    free(storage->pairs);
    storage->pairs = new_mem;
    storage->cap = new_cap;
}

void keyval_append(keyval_storage_t* storage, slice_t key, slice_t value) {
    if (storage->len >= storage->cap)
        keyval_grow_(storage);

    storage->pairs[storage->len] = (keyval_pair_t) { key, value };
    storage->len++;
}

static bool slice_cmp_fold(slice_t s1, slice_t s2) {
    if (s1.len != s2.len)
        return false;

    // FIXME: the trick with a|0x20 doesn't work good with non-letters; replace with LUT
    for (size_t i = 0; i < s1.len; i++) {
        if ((s1.data[i]|0x20) != (s2.data[i]|0x20)) return false;
    }

    return true;
}

/*
 * Returns a pointer to the found key-value pair, or NULL if not found
 */
keyval_pair_t* keyval_get(keyval_storage_t* storage, slice_t key) {
    for (size_t i = 0; i < storage->len; i++) {
        if (slice_cmp_fold(key, storage->pairs[i].key)) return &storage->pairs[i];
    }

    return NULL;
}
