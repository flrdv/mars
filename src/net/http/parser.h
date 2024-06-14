//
// Created by floordiv on 12-Jun-24.
//

#ifndef MARS_HTTP_PARSER_H
#define MARS_HTTP_PARSER_H

#include "misc/types.h"
#include <stdbool.h>

typedef struct {
    bool completed;
    byte_t* extra;
    int extra_size;
} http_parser_status_t;

typedef struct {

} http_parser_t;

#endif //MARS_HTTP_PARSER_H


