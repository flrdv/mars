//
// Created by pavlo on 19.06.24.
//

#include "request.h"

#define STR3_CMP(m, c0, c1, c2) \
    *(uint32_t*)m == ((c2 << 24) | (c1 << 16) | (c0 << 8))
#define STR4_CMP(m, c0, c1, c2, c3) \
    *(uint32_t*)m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | (c0))
#define STR5_CMP(m, c0, c1, c2, c3, c4) \
    (*(uint32_t*)m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | (c0))) && m[4] == c4
#define STR6_CMP(m, c0, c1, c2, c3, c4, c5) \
    *(uint32_t*)m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | (c0)) && \
    *(uint16_t*)m[4] == ((c5 << 8) | c4)
#define STR7_CMP(m, c0, c1, c2, c3, c4, c5, c6) \
    *(uint32_t*)m == ((c3 << 24) | (c2 << 16) | (c1 << 8) | (c0)) && \
    *(uint32_t*)m[4] == ((c6 << 24) | (c5 << 16) | (c4 << 8) | m[7])

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
            return UNKNOWN;
        case 6:
            return UNKNOWN;
        case 7:
            return UNKNOWN;
        default:
            return UNKNOWN;
    }
}
