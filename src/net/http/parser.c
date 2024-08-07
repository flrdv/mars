//
// Created by pavlo on 12-Jun-24.
//

#include <stdio.h>
#include "parser.h"
#include "lib/memcmpfold.h"

#define BUFFER_APPEND(buff, data, len, err) if (!buffer_append(buff, data, len)) { return ERROR(err); }
#define ERROR(err) (http_parser_status_t) { \
    .completed = false,                     \
    .extra = 0,                             \
    .error = err                            \
}
#define PENDING (http_parser_status_t) { \
    .completed = false,                  \
    .extra = 0,                          \
    .error = 0                           \
}
#define DONE(ex) (http_parser_status_t) { \
    .completed = true,                    \
    .extra = ex,                          \
    .error = 0                            \
}

http_parser_t http_parser_new(http_request_t* req, buffer_t* req_line_buff, buffer_t* header_buff) {
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

static void strip_cr(slice_t* slice) {
    if (slice->ptr[slice->len-1] == '\r')
        slice->len--;
}

static uint32_t slicetou32(const slice_t slice) {
    uint32_t value = 0;

    for (size_t i = 0; i < slice.len; i++) {
        if (slice.ptr[i] >= '0' && slice.ptr[i] <= '9') {
            value = value * 10 + (slice.ptr[i] - '0');
            continue;
        }

        return UINT32_MAX;
    }

    return value;
}

http_parser_status_t http_parse(http_parser_t* self, const byte_t* data, const ssize_t len) {
#define ADVANCE(n)     \
    do {               \
        data = data+n; \
        remains -= n;  \
    } while (0)

    ssize_t remains = len;

    switch (self->state) {
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
            puts("ERROR: net/http/parser.c: received unknown state");
            return ERROR(HTTP_STATUS_INTERNAL_SERVER_ERROR);
    }

    st_method:
    {
        const ssize_t space = find_char(data, remains, ' ');
        if (space == -1) {
            BUFFER_APPEND(self->req_line_buff, data, remains, HTTP_STATUS_NOT_IMPLEMENTED)

            return PENDING;
        }

        BUFFER_APPEND(self->req_line_buff, data, space, HTTP_STATUS_NOT_IMPLEMENTED)
        ADVANCE(space+1);
        slice_t segment = buffer_segment(self->req_line_buff);
        self->request->method = (http_method_t) {
            .method = http_parse_method(segment),
            .repr = segment
        };

        // fall through to st_uri
    }

    st_uri:
    {
        const ssize_t space = find_char(data, remains, ' ');
        if (space == -1) {
            BUFFER_APPEND(self->req_line_buff, data, remains, HTTP_STATUS_REQUEST_URI_TOO_LONG)
            self->state = ST_URI;

            return PENDING;
        }

        BUFFER_APPEND(self->req_line_buff, data, space, HTTP_STATUS_REQUEST_URI_TOO_LONG)
        ADVANCE(space+1);
        const slice_t path = buffer_segment(self->req_line_buff);
        self->request->path = path;

        // fall through to st_proto
    }

    st_proto:
    {
        const ssize_t lf = find_char(data, remains, '\n');
        if (lf == -1) {
            BUFFER_APPEND(self->req_line_buff, data, remains, HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED)
            self->state = ST_PROTO;

            return PENDING;
        }

        BUFFER_APPEND(self->req_line_buff, data, lf, HTTP_STATUS_HTTP_VERSION_NOT_SUPPORTED)
        ADVANCE(lf+1);
        slice_t protocol = buffer_segment(self->req_line_buff);
        strip_cr(&protocol);
        self->request->protocol = protocol;
    }

    st_header_key:
    {
        if (!remains) {
            self->state = ST_HEADER_KEY;

            return PENDING;
        }

        switch (data[0]) {
        case '\r':
            ADVANCE(1);
            goto st_last_lf;
        case '\n':
            ADVANCE(1);
            goto complete_request;
        default: ;
        }

        const ssize_t colon = find_char(data, remains, ':');
        if (colon == -1) {
            BUFFER_APPEND(self->header_buff, data, remains, HTTP_STATUS_HEADER_FIELDS_TOO_LARGE)
            self->state = ST_HEADER_KEY;

            return PENDING;
        }

        BUFFER_APPEND(self->header_buff, data, colon, HTTP_STATUS_HEADER_FIELDS_TOO_LARGE)
        ADVANCE(colon+1);
        slice_t header_key = buffer_segment(self->header_buff);
        self->current_header_key = header_key;

        // fall through to st_header_space
    }

    st_header_space:
    {
        if (!remains) {
            self->state = ST_HEADER_SPACE;

            return PENDING;
        }

        // optionally skip the following space. If there are multiple of them,
        // all of them, except the first one, are treated as a part of the value

        if (data[0] == ' ') {
            ADVANCE(1);
        }

        // fall through to st_header_value
    }

    st_header_value:
    {
        const ssize_t lf = find_char(data, remains, '\n');
        if (lf == -1) {
            BUFFER_APPEND(self->header_buff, data, remains, HTTP_STATUS_HEADER_FIELDS_TOO_LARGE)
            self->state = ST_HEADER_VALUE;

            return PENDING;
        }

        BUFFER_APPEND(self->header_buff, data, lf, HTTP_STATUS_HEADER_FIELDS_TOO_LARGE)
        ADVANCE(lf+1);
        slice_t value = buffer_segment(self->header_buff);
        strip_cr(&value);
        slice_t key = self->current_header_key;

        switch (key.len) {
        case 14: // content-length
            if (memcmpfold("content-length", (char*)key.ptr, 14)) {
                uint32_t content_length = slicetou32(value);
                if (content_length == UINT32_MAX) {
                    return ERROR(HTTP_STATUS_BAD_REQUEST);
                }

                self->request->content_length = content_length;
            }

            // TODO: add Content-Type, Transfer-Encoding
            break;
        default: ;
        }

        keyval_append(&self->request->headers, key, value);
        self->headers_count++;
        goto st_header_key;
    }
    st_last_lf:
    {
        if (!remains) {
            self->state = ST_LAST_LF;

            return PENDING;
        }

        if (data[0] != '\n') {
            return ERROR(HTTP_STATUS_BAD_REQUEST);
        }

        ADVANCE(1);
        // fall through to complete_request
    }

    complete_request:
    buffer_clear(self->req_line_buff);
    buffer_clear(self->header_buff);
    self->headers_count = 0;
    self->state = ST_METHOD;
    return DONE(remains);
}

void http_parser_free(http_parser_t* self) {
    buffer_free(self->req_line_buff);
    buffer_free(self->header_buff);
    http_request_free(self->request);
}
