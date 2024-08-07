//
// Created by pavlo on 19.06.24.
//

#include "request.h"

#include <stdint.h>

#define STR3_CMP(m, c0, c1, c2) \
    ((*(uint32_t*)m) & 0x00ffffff) == ((c2 << 16) | (c1 << 8) | (c0))
#define STR4_CMP(m, c0, c1, c2, c3) \
    (*(uint32_t*)m) == ((c3 << 24) | (c2 << 16) | (c1 << 8) | (c0))
#define STR5_CMP(m, c0, c1, c2, c3, c4) \
    ((*(uint64_t*)m) & 0x000000ffffffffff) == \
    ((uint64_t)c4 << 32 | (c3 << 24) | (c2 << 16) | (c1 << 8) | (c0))
#define STR6_CMP(m, c0, c1, c2, c3, c4, c5) \
    ((*(uint64_t*)m) & 0x0000ffffffffffff) == \
    (((uint64_t)c5 << 40) | ((uint64_t)c4 << 32) | (c3 << 24) | (c2 << 16) | (c1 << 8) | (c0))
#define STR7_CMP(m, c0, c1, c2, c3, c4, c5, c6) \
    ((*(uint64_t*)m) & 0x00ffffffffffffff) == \
    (((uint64_t)c6 << 48) | ((uint64_t)c5 << 40) | ((uint64_t)c4 << 32) | (c3 << 24) | (c2 << 16) | (c1 << 8) | (c0))

known_http_methods_t http_parse_method(const slice_t str) {
    switch (str.len) {
    case 3:
        if (STR3_CMP(str.ptr, 'G', 'E', 'T')) {
            return GET;
        }
        if (STR3_CMP(str.ptr, 'P', 'U', 'T')) {
            return PUT;
        }

        return UNKNOWN;
    case 4:
        if (STR4_CMP(str.ptr, 'H', 'E', 'A', 'D')) {
            return HEAD;
        }
        if (STR4_CMP(str.ptr, 'P', 'O', 'S', 'T')) {
            return POST;
        }

        return UNKNOWN;
    case 5:
        if (STR5_CMP(str.ptr, 'T', 'R', 'A', 'C', 'E')) {
            return TRACE;
        }
        if (STR5_CMP(str.ptr, 'P', 'A', 'T', 'C', 'H')) {
            return PATCH;
        }

        return UNKNOWN;
    case 6:
        if (STR6_CMP(str.ptr, 'D', 'E', 'L', 'E', 'T', 'E')) {
            return DELETE;
        }

        return UNKNOWN;
    case 7:
        if (STR7_CMP(str.ptr, 'O', 'P', 'T', 'I', 'O', 'N', 'S')) {
            return OPTIONS;
        }
        if (STR7_CMP(str.ptr, 'C', 'O', 'N', 'N', 'E', 'C', 'T')) {
            return CONNECT;
        }

        return UNKNOWN;
    default:
        return UNKNOWN;
    }
}

http_request_t http_request_new(keyval_t storage) {
    return (http_request_t) {
        .headers = storage,
    };
}

void http_request_free(http_request_t* request) {
    keyval_free(&request->headers);
}
