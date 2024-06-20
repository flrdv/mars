//
// Created by pavlo on 19.06.24.
//

#include "request.h"

#define STR3_CMP(m, c0, c1, c2) \
    *(uint32_t*)m == (((uint32_t)c2 << 16) | (c1 << 9) | (c0))
#define STR4_CMP(m, c0, c1, c2, c3) \
    *(uint32_t*)m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | (c0))
#define STR5_CMP(m, c0, c1, c2, c3, c4) \
    (*(uint32_t*)m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | (c0))) && m[4] == c4
#define STR6_CMP(m, c0, c1, c2, c3, c4, c5) \
    *(uint32_t*)m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | c0) && \
    (((uint32_t*) m)[1] & 0xffff) == ((c5 << 8) | c4)
#define STR7_CMP(m, c0, c1, c2, c3, c4, c5, c6) \
    ((*(uint64_t*)m) & 0xffffffffffffff00) == \
    (((uint64_t)c6 << 56) | (c5 << 48) | (c4 << 40) | (c3 << 32) | (c2 << 24) | (c1 << 16) | (c0 << 8))

known_http_methods_t http_parse_method(const slice_t str) {
    switch (str.len) {
        case 3:
            if (STR3_CMP(str.data, 'G', 'E', 'T')) {
                return GET;
            } else if (STR3_CMP(str.data, 'P', 'U', 'T')) {
                return PUT;
            }

            return UNKNOWN;
        case 4:
            if (STR4_CMP(str.data, 'H', 'E', 'A', 'D')) {
                return HEAD;
            } else if (STR4_CMP(str.data, 'P', 'O', 'S', 'T')) {
                return POST;
            }

            return UNKNOWN;
        case 5:
            if (STR5_CMP(str.data, 'T', 'R', 'A', 'C', 'E')) {
                return TRACE;
            } else if (STR5_CMP(str.data, 'P', 'A', 'T', 'C', 'H')) {
                return PATCH;
            }

            return UNKNOWN;
        case 6:
            if (STR6_CMP(str.data, 'D', 'E', 'L', 'E', 'T', 'E')) {
                return DELETE;
            }

            return UNKNOWN;
        case 7:
            if (STR7_CMP(str.data, 'O', 'P', 'T', 'I', 'O', 'N', 'S')) {
                return OPTIONS;
            } else if (STR7_CMP(str.data, 'C', 'O', 'N', 'N', 'E', 'C', 'T')) {
                return CONNECT;
            }

            return UNKNOWN;
        default:
            return UNKNOWN;
    }
}
