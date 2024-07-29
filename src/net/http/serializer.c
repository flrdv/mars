//
// Created by pavlo on 28.06.24.
//

#include <string.h>
#include <stdlib.h>

#include "protocol.h"
#include "lib/itoa.h"
#include "serializer.h"

#define CRLF        /**/ (slice_t) { .ptr = (byte_t*)"\r\n",     .len = 2 }
#define DOUBLE_CRLF /**/ (slice_t) { .ptr = (byte_t*)"\r\n\r\n", .len = 4 }
#define SP          /**/ (slice_t) { .ptr = (byte_t*)" ",        .len = 1 }
#define COLONSP     /**/ (slice_t) { .ptr = (byte_t*)": ",       .len = 2 }
#define CHECK_ERR(expr) do { ssize_t err = (expr); if (err) return err; } while (0)

static http_serializer_buff_t buff_new(slice_t slice) {
    return (http_serializer_buff_t) {
        .cap = slice.len,
        .len = 0,
        .mem = slice.ptr
    };
}

http_serializer_t http_serializer_new(net_client_t* client, slice_t slice) {
    return (http_serializer_t) {
        .buff = buff_new(slice),
        .client = client
    };
}

void http_serializer_free(http_serializer_t* self) {
    free(self->buff.mem);
}

static ssize_t write_respline(http_serializer_t* self, http_version_t protocol, http_status_t status);
static ssize_t write_headers(http_serializer_t* self, keyval_t* headers);
static ssize_t write_body(http_serializer_t* self, slice_t body);
static ssize_t flush_buffer(http_serializer_t* self);

ssize_t http_serializer_write(http_serializer_t* self, http_request_t* request, http_response_t* resp) {
    // TODO: process HTTP/1.0 requests, too
    CHECK_ERR(write_respline(self, HTTP_VER_11, resp->status));
    CHECK_ERR(write_headers(self, &resp->headers));
    CHECK_ERR(write_body(self, resp->body));

    if (!self->buff.len) return 0;

    return flush_buffer(self);
}

static ssize_t flush_buffer(http_serializer_t* self) {
    ssize_t result = self->client->write(self->client->env, slice_new(self->buff.mem, self->buff.len));
    self->buff.len = 0;
    return result;
}

/// write data into the buffer. If there's not enough space, buffer will be flushed
/// to the client first
static ssize_t write(http_serializer_t* self, slice_t data) {
    int result = 0;

    if (self->buff.len + data.len > self->buff.cap) {
        uint32_t space_left = self->buff.cap - self->buff.len;
        memcpy(&self->buff.mem[self->buff.len], data.ptr, space_left);
        self->buff.len = self->buff.cap;
        if (result = flush_buffer(self), result) return result;

        return write(self, slice_new(&data.ptr[space_left], data.len - space_left));
    }

    memcpy(&self->buff.mem[self->buff.len], data.ptr, data.len);
    self->buff.len += data.len;
    return result;
}

/// reserve n bytes in the buffer. Flush it in case there's not enough space
static ssize_t reserve(http_serializer_t* self, size_t n) {
    if (self->buff.len + n > self->buff.cap) return flush_buffer(self);

    return 0;
}

static ssize_t write_status_code(http_serializer_t* self, http_status_t status) {
    if (status < HTTP_STATUS_TOSTR_LEN) {
        const slice_t code = HTTP_STATUS_TOSTR[status];
        if (code.len) return write(self, code);
    }

    // 17 = 5 (max uint16_t decimal representation length) + 1 (space) + 11 (length of NO STATUS\r\n)
    CHECK_ERR(reserve(self, 17));
    int n = itoa_u64(&self->buff.mem[self->buff.len], (uint64_t)status);
    memcpy(&self->buff.mem[self->buff.len+n], " NO STATUS\r\n", 12);
    self->buff.len += n + 12;

    return 0;
}

static ssize_t write_respline(http_serializer_t* self, http_version_t protocol, http_status_t status) {
    slice_t proto = HTTP_VER_TOSLICE[protocol];
    CHECK_ERR(write(self, proto));
    CHECK_ERR(write(self, SP));
    return write_status_code(self, status);
}

static ssize_t write_header(http_serializer_t* self, slice_t key, slice_t value) {
    CHECK_ERR(write(self, key));
    CHECK_ERR(write(self, COLONSP));
    CHECK_ERR(write(self, value));
    return write(self, CRLF);
}

static ssize_t write_headers(http_serializer_t* self, keyval_t* headers) {
    for (size_t i = 0; i < headers->len; i++) {
        keyval_pair_t pair = headers->pairs[i];
        CHECK_ERR(write_header(self, pair.key, pair.value));
    }

    return 0;
}

static ssize_t write_content_length(http_serializer_t* self, uint64_t value) {
    const size_t MAX_U64_STRLEN = 20; // strlen("18446744073709551616") == 20
    write(self, slice_new((byte_t*)"Content-Length: ", 16));
    // BUG: if response buffer size is small enough, out-of-bounds may happen.
    // BUG: Either in case of space deficite fall back to stack-allocated array
    // BUG: and write it then or just assert the buffer will always be big enough
    // BUG: (at least 20 bytes)
    CHECK_ERR(reserve(self, MAX_U64_STRLEN));
    int n = itoa_u64(&self->buff.mem[self->buff.len], value);
    self->buff.len += n;

    return write(self, DOUBLE_CRLF);
}

static ssize_t write_body(http_serializer_t* self, slice_t body) {
    CHECK_ERR(write_content_length(self, (uint64_t)body.len));

    return write(self, body);
}
