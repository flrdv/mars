//
// Created by pavlo on 12-Jun-24.
//

#include <stdio.h>
#include "parser.h"
#include "misc/memcmpfold.h"

#define HTTP_BUFFER_APPEND(buff, data, len, err) if (!buffer_append(buff, data, len)) { return HTTP_ERROR(err); }
#define HTTP_ADVANCE(n) \
    data = data+n;      \
    remains -= n
#define HTTP_ERROR(err) (http_parser_status_t) { \
    .completed = false,                          \
    .extra_size = 0,                             \
    .error = err                                 \
}
#define HTTP_PENDING (http_parser_status_t) { \
    .completed = false,                       \
    .extra_size = 0,                          \
    .error = 0                                \
}
#define HTTP_DONE(extra) (http_parser_status_t) { \
    .completed = true,                            \
    .extra_size = extra,                          \
    .error = 0                                    \
}

http_parser_t http_new_parser(
        http_request_t* req, buffer_t* req_line_buff, buffer_t* header_buff
) {
    return (http_parser_t) {
        .request = req,
        .req_line_buff = req_line_buff,
        .header_buff = header_buff,
        .headers_count = 0,
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

static uint32_t slicetou32(const slice_t slice) {
    uint32_t value;

    for (size_t i = 0; i < slice.len; i++) {
        if (slice.data[i] == ' ') continue;

        if (slice.data[i] >= '0' && slice.data[i] <= '9') {
            value = value * 10 + (slice.data[i] - '0');
            continue;
        }

        return UINT32_MAX;
    }

    return value;
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
        case ST_HEADER_SPACE:
            goto st_header_space;
        case ST_HEADER_VALUE:
            goto st_header_value;
        case ST_LAST_LF:
            goto st_last_lf;
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
        HTTP_ADVANCE(space+1);
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
        HTTP_ADVANCE(space+1);
        const slice_t path = buffer_segment(parser->req_line_buff);
        parser->request->path = path;

        // fall through to st_proto
    }

    st_proto:
    {
        const ssize_t lf = find_char(data, remains, '\n');
        if (lf == -1) {
            HTTP_BUFFER_APPEND(parser->req_line_buff, data, remains, HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED)
            parser->state = ST_PROTO;

            return HTTP_PENDING;
        }

        HTTP_BUFFER_APPEND(parser->req_line_buff, data, lf, HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED)
        HTTP_ADVANCE(lf+1);
        slice_t protocol = buffer_segment(parser->req_line_buff);
        strip_cr(&protocol);
        parser->request->protocol = protocol;
    }

    st_header_key:
    {
        if (!remains) {
            parser->state = ST_HEADER_KEY;

            return HTTP_PENDING;
        }

        switch (data[0]) {
            case '\r':
                goto st_last_lf;
            case '\n':
                goto complete_request;
        }

        const ssize_t colon = find_char(data, remains, ':');
        if (colon == -1) {
            HTTP_BUFFER_APPEND(parser->header_buff, data, remains, HTTP_STATUS_HEADER_FIELDS_TOO_LARGE)
            parser->state = ST_HEADER_KEY;

            return HTTP_PENDING;
        }

        HTTP_BUFFER_APPEND(parser->header_buff, data, colon, HTTP_STATUS_HEADER_FIELDS_TOO_LARGE)
        HTTP_ADVANCE(colon+1);
        slice_t header_key = buffer_segment(parser->header_buff);
        parser->current_header_key = header_key;

        // fall through to st_header_space
    }

    st_header_space:
    {
        if (!remains) {
            parser->state = ST_HEADER_SPACE;

            return HTTP_PENDING;
        }

        // optionally skip the following space. If there are multiple of them,
        // all of them, except the first one, are treated as a part of the value

        if (data[0] == ' ') {
            HTTP_ADVANCE(1);
        }

        // fall through to st_header_value
    }

    st_header_value:
    {
        const ssize_t lf = find_char(data, remains, '\n');
        if (lf == -1) {
            HTTP_BUFFER_APPEND(parser->header_buff, data, lf, HTTP_STATUS_HEADER_FIELDS_TOO_LARGE)
            parser->state = ST_HEADER_VALUE;

            return HTTP_PENDING;
        }

        HTTP_BUFFER_APPEND(parser->header_buff, data, lf, HTTP_STATUS_HEADER_FIELDS_TOO_LARGE)
        HTTP_ADVANCE(lf+1);
        slice_t value = buffer_segment(parser->header_buff);
        strip_cr(&value);
        slice_t key = parser->current_header_key;

        switch (key.len) {
            case 14: // content-length
                if (memcmpfold("content-length", (char*)key.data, 14)) {
                    uint32_t content_length = slicetou32(value);
                    if (content_length == UINT32_MAX) {
                        return HTTP_ERROR(HTTP_STATUS_BAD_REQUEST);
                    }

                    parser->request->content_length = content_length;
                }

                break;
            // TODO: add Content-Type, Transfer-Encoding
        }

        keyval_append(&parser->request->headers, key, value);
        parser->headers_count++;
        goto st_header_key;
    }
    st_last_lf:
    {
        if (!remains) {
            parser->state = ST_LAST_LF;

            return HTTP_PENDING;
        }

        if (data[0] != '\n') {
            return HTTP_ERROR(HTTP_STATUS_BAD_REQUEST);
        }

        // fall through to complete_request
    }

    complete_request:
    buffer_clear(parser->req_line_buff);
    buffer_clear(parser->header_buff);
    parser->headers_count = 0;
    parser->state = ST_METHOD;
    return HTTP_DONE(remains);
}

void http_parser_free(http_parser_t* parser) {
    buffer_free(parser->req_line_buff);
    buffer_free(parser->header_buff);
    http_request_free(parser->request);
}
