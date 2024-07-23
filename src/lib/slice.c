//
// Created by pavlo on 19.06.24.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slice.h"

#include "itoa.h"
#include "memcmpfold.h"

slice_t slice_new(byte_t* ptr, size_t len) {
    return (slice_t) {
        .ptr = ptr,
        .len = len
    };
}

slice_t slice_str(const char* str) {
    return slice_new((byte_t*)str, strlen(str));
}

bool slice_cmp(slice_t a, slice_t b) {
    if (a.len != b.len) return false;

    return memcmp(a.ptr, b.ptr, a.len) == 0;
}

bool slice_cmpfold(slice_t a, slice_t b) {
    if (a.len != b.len) return false;

    return memcmpfold((char*)a.ptr, (char*)b.ptr, a.len);
}

char* slice_ascstr(const slice_t self) {
    char* str = malloc(self.len + 1);
    memcpy(str, self.ptr, self.len);
    str[self.len] = 0;
    return str;
}

slice_t slice_clone(slice_t self) {
    byte_t* clone = malloc(self.len);
    memcpy(clone, self.ptr, self.len);

    return slice_new(clone, self.len);
}

size_t slice_copyto(slice_t from, slice_t to) {
    size_t minlen = from.len > to.len ? to.len : from.len;
    memcpy(to.ptr, from.ptr, minlen);

    return minlen;
}

slice_t slice_offset(slice_t self, ssize_t offset) {
    self.ptr = &self.ptr[offset];
    self.len -= offset;
    return self;
}

void slice_print_escaped(slice_t self) {
    char* escapes[256] = {
        ['\r'] = "\\r",
        ['\n'] = "\\n"
    };

    putchar('"');

    for (size_t i = 0; i < self.len; i++) {
        char ch = self.ptr[i];
        if (ch >= 32 && ch < 127) {
            putchar(ch);
            continue;
        }

        char* escaped = escapes[ch];
        if (escaped) {
            printf("%s", escaped);
            continue;
        }

        char buff[3] = { 0 };
        itoa_hex((byte_t*)buff, (uint64_t)ch);
        printf("\\%s", buff);
    }

    puts("\"\n");
}

void slice_free(slice_t self) {
    free(self.ptr);
}
