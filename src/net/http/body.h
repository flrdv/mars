//
// Created by pavlo on 28.06.24.
//

#ifndef MARS_NET_HTTP_BODY_H
#define MARS_NET_HTTP_BODY_H

#include <stdbool.h>
#include "request.h"
#include "net/client.h"
#include "encodings/encoding.h"

enum te_token_t {
    Chunked,
    GZIP,
    ZSTD
};

#define MAX_TRANSFER_ENCODINGS 5

typedef struct http_body_t {
    net_client_t* client;
    size_t encsc;
    http_encoding_t encs[MAX_TRANSFER_ENCODINGS];
} http_body_t;

typedef struct http_body_read_t {
    size_t errno;
    slice_t result;
} http_body_read_t;

http_body_t http_body_new(net_client_t* client);
http_body_read_t http_body_read(http_body_t* self);
void http_body_reset(http_body_t* self, http_request_t* request);

#endif //MARS_NET_HTTP_BODY_H
