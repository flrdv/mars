//
// Created by floordiv on 12-Jun-24.
//

#include <stdbool.h>
#include "mars_http_parser.h"

typedef struct {
    bool completed;
    byte_t* extra;
    int extra_size;
} http_parser_status_t;

typedef struct {

} http_parser_t;
