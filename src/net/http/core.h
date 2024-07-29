//
// Created by pavlo on 28.06.24.
//

#ifndef MARS_SRC_NET_HTTP_CORE_H
#define MARS_SRC_NET_HTTP_CORE_H

#include "parser.h"
#include "net/client.h"

typedef struct {
    http_parser_t parser;
} http_core_t;

http_core_t http_new(void);

#endif //MARS_SRC_NET_HTTP_CORE_H
