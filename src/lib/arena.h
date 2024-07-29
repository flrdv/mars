//
// Created by pavlo on 27.07.24.
//

#ifndef MARS_SRC_LIB_ALLOC_H
#define MARS_SRC_LIB_ALLOC_H

#include "list.h"
#include "byte.h"

#include <stddef.h>

typedef struct {
    size_t arenasize;
    size_t elemsize;
    list_t arenas;
    list_t ptrs;
} arena_t;

arena_t arena_new(size_t elemsize, size_t prealloc);
void*   arena_acquire(arena_t* self);
bool    arena_release(arena_t* self, void* ptr);

#endif //MARS_SRC_LIB_ALLOC_H
