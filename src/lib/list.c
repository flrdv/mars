//
// Created by pavlo on 17.07.24.
//

#include "list.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

list_t list_new(uint16_t elemsize) {
    return list_alloc(elemsize, 0);
}

/// Pre-allocate a list with a capacity capable of holding n elements.
/// If n is zero, ptr is NULL.
list_t list_alloc(uint16_t elemsize, size_t n) {
    void* ptr = NULL;
    if (n) ptr = malloc(n*elemsize); //NOLINT

    return (list_t) {
        .elemsize = elemsize,
        .ptr = ptr,
        .len = 0,
        .cap = n
    };
}

static size_t list_nextcap(size_t cap) {
    const size_t threshold = 512;
    if (cap < threshold)
        return cap + cap + 1; // +1 solves the edge case of cap=0

    // Transition from growing 2x for small slices
    // to growing 1.25x for large slices. This formula
    // gives a smooth-ish transition between the two.
    // Proudly stolen from Go's runtime.
    return cap + ((cap + 3*threshold) >> 2);
}

static bool list_grow(list_t* self, size_t newlen) {
    size_t newcap = list_nextcap(self->cap);
    while (newcap < newlen) {
        // increase cap as long as it doesn't exceed newlen
        size_t nextcap = list_nextcap(newcap);
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

/// list_push copies _a single object behind the pointer_, so it's safe to pass pointers
/// pointing at temporal data on stack.
/// Returns bool, based on whether the operation succeeded.
bool list_push(list_t* self, void* elem) {
    return list_append(self, elem, 1);
}

/// Does the same thing as list_append, but with many elements per once.
/// Returns bool, based on whether the operation succeeded.
bool list_append(list_t* self, void* elems, size_t elemc) {
    if (!elemc) return true;

    if (self->len+elemc > self->cap)
        if (!list_grow(self, self->len+elemc))
            return false;

    // printf("list.c (append, %p): appending %lu elems (%lu bytes) to array of %lu bytes capacity (%lu bytes occupied, %d elemsize)\n",
        // self, elemc, elemc*self->elemsize, self->cap * self->elemsize, self->len*self->elemsize, self->elemsize);

    memcpy(self->ptr + self->len*self->elemsize, elems, elemc*self->elemsize);
    self->len += elemc;
    return true;
}

/// Removes the last element from the list and returns pointer pointing at it.
void* list_pop(list_t* self) {
    if (self->len == 0)
        return NULL;

    return self->ptr + self->elemsize * --self->len;
}

/// Removes last n elements from the list. If n is greater or equal to the length
/// of the list, list becomes empty.
void list_rstrip(list_t* self, size_t n) {
    self->len -= self->len > n ? n : self->len;
}

void* list_access(list_t* self, size_t index) {
    if (index >= self->len)
        return NULL;

    return self->ptr + index * self->elemsize;
}

void list_clear(list_t* self) {
    self->len = 0;
}

/// Free the list's allocated memory. However, length, cap, and other fields
/// are staying intact. No element is destructed either.
void list_free(list_t* self) {
    if (self->ptr != NULL)
        free(self->ptr);
}
