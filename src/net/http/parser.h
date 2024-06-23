//
// Created by pavlo on 12-Jun-24.
//

#ifndef MARS_HTTP_PARSER_H
#define MARS_HTTP_PARSER_H

#include <stdbool.h>
#include "types.h"
#include "status.h"
#include "request.h"
#include "misc/buffer.h"

typedef struct {
    bool completed;
    byte_t* extra;
    int extra_size;
    http_status_t error;
} http_parser_status_t;

typedef enum {
    ST_METHOD,
    ST_URI,
    ST_PROTO,
    ST_HEADER_KEY,
    ST_HEADER_COLONSP,
    ST_HEADER_VALUE,
} state_t;

typedef struct {
    http_request_t* request;
    buffer_buffer_t* req_line_buff;
    buffer_buffer_t* header_buff;
    char* header_key;
    uint16_t headers_count;
    state_t state;
} http_parser_t;

http_parser_t http_parser_new(
        http_request_t* req, buffer_buffer_t* req_line_buff, buffer_buffer_t* header_buff
);
http_parser_status_t http_parse(http_parser_t* parser, const byte_t* data, ssize_t len);

#endif //MARS_HTTP_PARSER_H


