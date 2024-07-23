//
// Created by pavlo on 17.07.24.
//

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "list.h"

#include <stdio.h>

list_t list_new(uint16_t elemsize) {
    return list_alloc(elemsize, 0);
}

list_t list_alloc(uint16_t elemsize, int n) {
    return (list_t) {
        .elemsize = elemsize,
        .ptr = malloc(n * elemsize),
        .len = 0,
        .cap = n * elemsize
    };
}

static int list_nextcap(int cap) {
    const int threshold = 512;
    if (cap < threshold)
        return cap + cap + 1; // +1 solves the edge case of cap=0

    // Transition from growing 2x for small slices
    // to growing 1.25x for large slices. This formula
    // gives a smooth-ish transition between the two.
    // Proudly stolen from Go's runtime.
    return cap + ((cap + 3*threshold) >> 2);
}

static bool list_grow(list_t* self, int newlen) {
    int newcap = list_nextcap(self->cap);
    while (newcap < newlen) {
        // increase cap as long as it doesn't exceed newlen
        int nextcap = list_nextcap(newcap);
        if (nextcap < newcap)
            return false;

        newcap = nextcap;
    }

    if (newcap <= self->cap)
        return false;

    void* newmem = malloc(newcap * self->elemsize);
    memcpy(newmem, self->ptr, self->len * self->elemsize);
    free(self->ptr);
    self->ptr = newmem;
    self->cap = newcap;
    return true;
}

/// list_append copies _data behind the pointer_, so it's safe to pass pointers
/// pointing at temporal data on stack.
/// Returns bool, based on whether the operation succeeded.
bool list_append(list_t* self, void* elem) {
    return list_append_many(self, elem, 1);
}

/// Does the same thing as list_append, but with many elements per once.
/// Returns bool, based on whether the operation succeeded.
bool list_append_many(list_t* self, void* elems, int elemc) {
    if (!elemc) return true;

    if (self->len+elemc > self->cap)
        if (!list_grow(self, self->len+elemc))
            return false;

    memcpy(self->ptr + self->len * self->elemsize, elems, elemc*self->elemsize);
    self->len += elemc;
    return true;
}

void* list_access(list_t* self, uint index) {
    if (index > self->len)
        return NULL;

    return self->ptr + index * self->elemsize;
}

/// Free the list's allocated memory. However, length, cap, and other fields
/// are staying intact. No element is destructed either.
void list_free(list_t* self) {
    free(self->ptr);
}
