//
// Created by pavlo on 28.06.24.
//

#ifndef RESPONSE_H
#define RESPONSE_H

#include "status.h"
#include "lib/slice.h"
#include "lib/keyval.h"

typedef struct {
    http_status_t status;
    // TODO: add response status overriding?
    keyval_t headers;
    slice_t body;
} http_response_t;

http_response_t http_response_new(keyval_t headers);

#endif //RESPONSE_H
