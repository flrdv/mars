//
// Created by floordiv on 12-Jun-24.
//

#ifndef MARS_MARS_HTTP_H
#define MARS_MARS_HTTP_H

#include <stdint.h>

#include "mars_http_statuses.c"

typedef struct {

} http_request_t;

typedef struct {
    http_status_t status;
} http_response_t;

#endif //MARS_MARS_HTTP_H
