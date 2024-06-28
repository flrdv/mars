//
// Created by pavlo on 19.06.24.
//

#include <string.h>
#include "types.h"

slice_t slice_new(byte_t* data, size_t len) {
    return (slice_t) {
        .data = data,
        .len = len
    };
}

bool slice_cmp(slice_t a, slice_t b) {
    if (a.len != b.len) return false;

    return memcmp(a.data, b.data, a.len) == 0;
}

char* slice_ascstr(const slice_t self) {
    char* str = malloc(self.len + 1);
    memcpy(str, self.data, self.len);
    str[self.len] = 0;
    return str;
}
