//
// Created by pavlo on 28.06.24.
//

#ifndef RESPONSE_H
#define RESPONSE_H

#include "status.h"
#include "misc/keyval.h"

typedef struct {
    http_status_t status;
    // TODO: add response status overriding?
    keyval_t headers;
} http_response_t;

#endif //RESPONSE_H
