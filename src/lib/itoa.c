//
// Created by pavlo on 07.07.24.
//

#include "itoa.h"

static void reverse(byte_t* src, size_t len) {
    // don't care about bswap, as usually strings are short enough
    for (size_t i = 0; i < len/2; i++) {
        char tmp = src[i];
        src[i] = src[len-i-1];
        src[len-i-1] = tmp;
    }
}

int itoa_u64(byte_t* into, uint64_t decimal) {
    size_t offset = 0;

    do {
        into[offset++] = decimal % 10 + '0';
        decimal /= 10;
    } while (decimal > 0);

    reverse(into, offset);
    return offset;
}

static char HEX_ABC[] = "0123456789abcdef";

int itoa_hex(byte_t* into, uint64_t hex) {
    size_t offset = 0;

    do {
        into[offset++] = HEX_ABC[hex & 0xf];
        hex >>= 4;
    } while (hex > 0);

    reverse(into, offset);
    return offset;
}
