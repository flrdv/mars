//
// Created by pavlo on 13-Jun-24.
//

#include <stdlib.h>
#include <string.h>
#include "buffer.h"

#define TRY_GROW(buff, size) if (!buffer_grow_(buff, size)) { return false; }

buffer_buffer_t buffer_new(uint32_t prealloc, uint32_t max_cap) {
    return (buffer_buffer_t) {
        .mem = malloc(prealloc),
        .offset = 0,
        .len = 0,
        .cap = prealloc,
        .max_cap = max_cap,
    };
}

void buffer_free(buffer_buffer_t* buff) {
    free(buff->mem);
}

static bool buffer_grow_(buffer_buffer_t* buff, size_t increment) {
    size_t new_cap = buff->cap + increment; // TODO: make some smarter algorithm
    if (new_cap > buff->max_cap) return false;

    byte_t* new_mem = malloc(new_cap);
    // copy up to len instead of cap because space after the len
    // must anyway be empty
    memcpy(new_mem, buff->mem, buff->len);
    free(buff->mem);
    buff->mem = new_mem;
    buff->cap = new_cap;
    return true;
}

bool buffer_append(buffer_buffer_t* buff, const byte_t* const data, size_t size) {
    if (buff->len + size > buff->cap) {
        TRY_GROW(buff, size - (buff->cap - buff->len));
    }

    memcpy(buff->mem + buff->len, data, size);
    buff->len += size;
    return true;
}

slice_t buffer_segment(buffer_buffer_t* buff) {
    byte_t *segment = &buff->mem[buff->offset];
    size_t len = buff->len - buff->offset;
    buff->offset = buff->len;
    return new_slice(segment, len);
}

size_t buffer_segment_len(buffer_buffer_t* buff) {
    return buff->len - buff->offset;
}
