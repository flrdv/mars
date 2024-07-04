//
// Created by floordiv on 22-Jun-24.
//

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "keyval.h"
#include "memcmpfold.h"

keyval_t new_keyval(const size_t prealloc, const size_t step) {
    return (keyval_t) {
        .pairs = malloc(prealloc * sizeof(keyval_pair_t)),
        .len = 0,
        .cap = prealloc,
        .step = step,
    };
}

static void keyval_grow_(keyval_t* self) {
    const size_t new_cap = self->cap + self->step;
    keyval_pair_t* new_mem = malloc(new_cap * sizeof(keyval_pair_t));
    memcpy(new_mem, self->pairs, self->len*sizeof(keyval_pair_t));
    free(self->pairs);
    self->pairs = new_mem;
    self->cap = new_cap;
}

void keyval_append(keyval_t* self, const slice_t key, const slice_t value) {
    if (self->len >= self->cap) keyval_grow_(self);

    self->pairs[self->len++] = (keyval_pair_t) { key, value };
}

static bool slice_cmp_fold(const slice_t s1, const slice_t s2) {
    if (s1.len != s2.len)
        return false;

    return memcmpfold((char*)s1.elems, (char*)s2.elems, s1.len);
}

/*
 * Returns a pointer to the found key-value pair, or NULL if not found
 */
keyval_pair_t* keyval_get(const keyval_t* self, const slice_t key) {
    for (size_t i = 0; i < self->len; i++) {
        if (slice_cmp_fold(key, self->pairs[i].key)) return &self->pairs[i];
    }

    return NULL;
}

keyval_iterator_t keyval_values(keyval_t* self, const slice_t key) {
    return (keyval_iterator_t) {
        .testkey = key,
        .storage = self,
        .index = 0
    };
}

keyval_pair_t* keyval_values_next(keyval_iterator_t* self) {
    for (; self->index < self->storage->len; self->index++) {
        if (slice_cmp_fold(self->storage->pairs[self->index].key, self->testkey)) {
            return &self->storage->pairs[self->index++];
        }
    }
    
    return NULL;
}

size_t keyval_len(const keyval_t* self) {
    return self->len;
}

void keyval_clear(keyval_t* self) {
    self->len = 0;
}

void keyval_free(const keyval_t* self) {
    free(self->pairs);
}
