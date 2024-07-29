//
// Created by pavlo on 13-Jun-24.
//

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "buffer.h"

#define TRY_GROW(buff, size) if (!buffer_grow(buff, size)) { return false; }

buffer_t buffer_new(size_t prealloc, size_t max_cap) {
    return (buffer_t) {
        .mem = malloc(prealloc),
        .offset = 0,
        .len = 0,
        .cap = prealloc,
        .max_cap = max_cap,
    };
}

void buffer_free(buffer_t* buff) {
    free(buff->mem);
}

static bool buffer_grow(buffer_t* buff, size_t increment) {
    // TODO: make some smarter algorithm. Or just use the one from list.h
    size_t new_cap = buff->cap + increment;
    if (new_cap > buff->max_cap) return false;

    byte_t* new_mem = malloc(new_cap);
    memcpy(new_mem, buff->mem, buff->len);
    free(buff->mem);
    buff->mem = new_mem;
    buff->cap = new_cap;
    return true;
}

bool buffer_append(buffer_t* buff, const byte_t* const data, size_t size) {
    if (buff->len + size > buff->cap) {
        TRY_GROW(buff, size - (buff->cap - buff->len));
    }

    memcpy(&buff->mem[buff->len], data, size);
    buff->len += size;
    return true;
}

bool buffer_append_char(buffer_t* buff, const char* data, size_t size) {
    return buffer_append(buff, (byte_t*)data, size);
}

slice_t buffer_segment(buffer_t* buff) {
    byte_t *segment = &buff->mem[buff->offset];
    size_t len = buff->len - buff->offset;
    buff->offset = buff->len;
    return slice_new(segment, len);
}

size_t buffer_segment_len(buffer_t* buff) {
    return buff->len - buff->offset;
}

void buffer_clear(buffer_t* buff) {
    buff->len = 0;
    buff->offset = 0;
}
