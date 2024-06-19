//
// Created by pavlo on 19.06.24.
//

#ifndef MARS_REQUEST_H
#define MARS_REQUEST_H

#include "types.h"

typedef enum {
    UNKNOWN,
    GET,
    HEAD,
    POST,
    PUT,
    DELETE,
    CONNECT,
    OPTIONS,
    TRACE,
    PATCH,
} known_http_methods_t;

typedef struct {
    known_http_methods_t method;
    slice_t repr;
} http_method_t;

typedef struct {
    http_method_t method;
    slice_t path;
} http_request_t;

known_http_methods_t http_parse_method(slice_t str);

#endif //MARS_REQUEST_H
