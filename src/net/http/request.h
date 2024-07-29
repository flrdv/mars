//
// Created by pavlo on 19.06.24.
//

#ifndef MARS_HTTP_REQUEST_H
#define MARS_HTTP_REQUEST_H

#include "lib/slice.h"
#include "lib/keyval.h"

#include <stdint.h>

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
    slice_t protocol;
    keyval_t headers;
    uint32_t content_length;
} http_request_t;

http_request_t http_request_new(keyval_t storage);
void http_request_free(http_request_t* request);
known_http_methods_t http_parse_method(slice_t str);

#endif //MARS_HTTP_REQUEST_H
