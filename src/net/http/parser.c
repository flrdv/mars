//
// Created by pavlo on 12-Jun-24.
//

#include <stdio.h>
#include "parser.h"

#define HTTP_ERROR(err) (http_parser_status_t) { \
    .completed = false,                          \
    .extra_size = 0,                             \
    .error = err                                 \
}
#define HTTP_BUFFER_APPEND(buff, data, len, err) if (!buffer_append(buff, data, len)) { return HTTP_ERROR(err); }
#define HTTP_PENDING (http_parser_status_t) { \
    .completed = false,                       \
    .extra_size = 0,                          \
    .error = 0                                \
}

http_parser_t http_parser_new(
        http_request_t* req, buffer_buffer_t* req_line_buff, buffer_buffer_t* header_buff
) {
    return (http_parser_t) {
        .request = req,
        .req_line_buff = req_line_buff,
        .header_buff = header_buff,
        .headers = 0,
        .state = ST_METHOD,
    };
}

static ssize_t find_char(const byte_t* data, const ssize_t len, const char ch) {
    // TODO: perfect candidate for vectorization. Check whether compilers agree

    for (ssize_t i = 0; i < len; i++) {
        if (data[i] == ch) {
            return i;
        }
    }

    return -1;
}

http_parser_status_t http_parse(http_parser_t* parser, const byte_t* data, const ssize_t len) {
    switch (parser->state) {
        case ST_METHOD:
            goto st_method;
        case ST_URI:
            goto st_uri;
        case ST_PROTO:
            goto st_proto;
        case ST_HEADER_KEY:
            goto st_header_key;
        case ST_HEADER_COLONSP:
            goto st_header_colonsp;
        case ST_HEADER_VALUE:
            goto st_header_value;
        default:
            puts("ERROR: net/http/parser.c: met unknown state");
            return HTTP_ERROR(HTTP_STATUS_INTERNAL_SERVER_ERROR);
    }

        st_method:
    {
        ssize_t space = find_char(data, len, ' ');
        if (space == -1) {
            HTTP_BUFFER_APPEND(parser->req_line_buff, data, len, HTTP_STATUS_NOT_IMPLEMENTED)

            return HTTP_PENDING;
        }

        data = data+space+1;
        slice_t segment = buffer_segment(parser->req_line_buff);
    }

        st_uri:
    {

    };
        st_proto:
    {

    };
        st_header_key:
    {

    };
        st_header_colonsp:
    {

    };
        st_header_value:
    {

    };
        cleanup:
    {

    };
}
