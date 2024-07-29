//
// Created by pavlo on 05.07.24.
//

#ifndef MARS_SRC_NET_HTTP_PROTOCOL_H
#define MARS_SRC_NET_HTTP_PROTOCOL_H

#include <stdint.h>
#include "lib/slice.h"

typedef uint8_t http_version_t;

#define HTTP_VER_UNSUPPORTED 0
#define HTTP_VER_10          1
#define HTTP_VER_11          2

static const slice_t HTTP_VER_TOSLICE[256] = {
    [HTTP_VER_10] = (slice_t) { .ptr = (byte_t*)"HTTP/1.0", .len = 8},
    [HTTP_VER_11] = (slice_t) { .ptr = (byte_t*)"HTTP/1.1", .len = 8},
};

#endif //MARS_SRC_NET_HTTP_PROTOCOL_H
