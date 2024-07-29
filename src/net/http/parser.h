//
// Created by pavlo on 12-Jun-24.
//

#ifndef MARS_SRC_NET_HTTP_PARSER_H
#define MARS_SRC_NET_HTTP_PARSER_H

#include "status.h"
#include "request.h"
#include "lib/slice.h"
#include "lib/buffer.h"

#include <stdbool.h>

typedef struct {
    bool completed;
    size_t extra;
    http_status_t error;
} http_parser_status_t;

typedef enum {
    ST_METHOD,
    ST_URI,
    ST_PROTO,
    ST_HEADER_KEY,
    ST_HEADER_SPACE,
    ST_HEADER_VALUE,
    ST_LAST_LF
} http_parser_state_t;

typedef struct {
    uint16_t headers_count;
    http_request_t* request;
    buffer_t* req_line_buff;
    buffer_t* header_buff;
    http_parser_state_t state;
    slice_t current_header_key;
} http_parser_t;

http_parser_t        http_parser_new(http_request_t* req, buffer_t* req_line_buff, buffer_t* header_buff);
http_parser_status_t http_parse(http_parser_t* self, const byte_t* data, ssize_t len);
void                 http_parser_free(http_parser_t* self);

#endif //MARS_SRC_NET_HTTP_PARSER_H


