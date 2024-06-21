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

static inline void strip_cr(slice_t* slice) {
    if (slice->data[slice->len-1] == '\r') {
        slice->len--;
    }
}

http_parser_status_t http_parse(http_parser_t* parser, const byte_t* data, const ssize_t len) {
    ssize_t remains = len;

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
        const ssize_t space = find_char(data, remains, ' ');
        if (space == -1) {
            HTTP_BUFFER_APPEND(parser->req_line_buff, data, remains, HTTP_STATUS_NOT_IMPLEMENTED)

            return HTTP_PENDING;
        }

        HTTP_BUFFER_APPEND(parser->req_line_buff, data, space, HTTP_STATUS_NOT_IMPLEMENTED)
        data = data+space+1;
        remains -= space;
        slice_t segment = buffer_segment(parser->req_line_buff);
        parser->request->method = (http_method_t) {
            .method = http_parse_method(segment),
            .repr = segment
        };

        // fall through to st_uri
    }

        st_uri:
    {
        const ssize_t space = find_char(data, remains, ' ');
        if (space == -1) {
            HTTP_BUFFER_APPEND(parser->req_line_buff, data, remains, HTTP_STATUS_REQUEST_URI_TOO_LONG)
            parser->state = ST_URI;

            return HTTP_PENDING;
        }

        HTTP_BUFFER_APPEND(parser->req_line_buff, data, space, HTTP_STATUS_REQUEST_URI_TOO_LONG)
        data = data+space+1;
        remains -= space;
        const slice_t segment = buffer_segment(parser->req_line_buff);
        parser->request->path = segment;

        // fall through to st_proto
    };
        st_proto:
    {
        const ssize_t lf = find_char(data, remains, '\n');
        if (lf == -1) {
            HTTP_BUFFER_APPEND(parser->req_line_buff, data, remains, HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED)
            parser->state = ST_PROTO;

            return HTTP_PENDING;
        }

        HTTP_BUFFER_APPEND(parser->req_line_buff, data, lf, HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED)
        data = data+lf+1;
        remains -= lf;
        slice_t segment = buffer_segment(parser->req_line_buff);
        strip_cr(&segment);
        parser->request->path = segment;
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
