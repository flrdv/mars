//
// Created by pavlo on 27.07.24.
//

#include "arena.h"

#include <stdio.h>
#include <stdlib.h>

arena_t arena_new(size_t elemsize, size_t prealloc) {
    const size_t MINIMAL_SIZE = (1 << 16) - 1;
    const size_t MINIMUM_ENTRIES = 64;
    const size_t LIST_PREALLOC = 10;

    // all preallocated elements are stored on a single arena, no matter how big it is.
    // Using ARENAS_LIST_PREALLOC in order to reduce a number of further reallocations,
    // as they may become quite frequent when starting with low capacity.
    list_t arenas = list_alloc(sizeof(void*), prealloc ? LIST_PREALLOC : 0); //NOLINT
    list_t ptrs = list_alloc(sizeof(void*), prealloc);                       //NOLINT

    size_t arena_size = elemsize * MINIMUM_ENTRIES;
    if (arena_size < MINIMAL_SIZE)
        arena_size = MINIMAL_SIZE;

    if (prealloc) {
        size_t base_arena_size = arena_size;
        if (base_arena_size < elemsize*prealloc)
            base_arena_size = elemsize*prealloc;

        // in case any of operations fail, just return as-is. If nothing was allocated
        // or could only partially be appended (e.g. ptrs to base), we may have luck
        // during following acquirations. Or simply NULL pointer will be returned there,
        // which is also a good thing.
        void* base = malloc(base_arena_size);
        if (base == NULL)
            goto exit;

        if (!list_push(&arenas, &base))
            goto exit;

        for (size_t i = 0; i <= base_arena_size-elemsize; i += elemsize) {
            void* ptr = base + i;
            if (!list_push(&ptrs, &ptr))
                goto exit;
        }
    }

exit:
    return (arena_t) {
        .arenasize = arena_size,
        .elemsize = elemsize,
        .arenas = arenas,
        .ptrs = ptrs
    };
}

static bool arena_grow(arena_t* self) {
    void* base = malloc(self->arenasize);
    if (base == NULL) return false;

    if (!list_push(&self->arenas, &base)) {
        free(base);
        return false;
    }

    for (size_t i = 0; i <= self->arenasize-self->elemsize; i += self->elemsize) {
        void* ptr = base + i;
        if (!list_push(&self->ptrs, &ptr)) {
            list_rstrip(&self->ptrs, i);
            free(base);

            return false;
        }
    }

    return true;
}

void* arena_acquire(arena_t* self) {
    void* ptr = list_pop(&self->ptrs);
    if (ptr == NULL) {
        if (!arena_grow(self))
            return NULL;

        // at this point, ptr is guaranteed to be valid. Otherwise is considered
        // to be a bug. This may actually be quite harmful, however.
        ptr = list_pop(&self->ptrs);
    }

    return *(void**)ptr;
}

bool arena_release(arena_t* self, void* ptr) {
    return list_push(&self->ptrs, &ptr);
}

void arena_free(arena_t* self) {
    byte_t** ptrs = self->arenas.ptr;

    for (size_t i = 0; i < self->arenas.len; i++)
        free(ptrs[i]);

    list_free(&self->ptrs);
    list_free(&self->arenas);
}
