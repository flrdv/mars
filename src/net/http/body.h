//
// Created by pavlo on 28.06.24.
//

#ifndef MARS_NET_HTTP_BODY_H
#define MARS_NET_HTTP_BODY_H

#include "status.h"
#include "request.h"
#include "net/client.h"
#include "encodings/plain.h"
#include "encodings/encoding.h"

#define MAX_TRANSFER_ENCODINGS 5

typedef uint8_t http_body_status_code;

static const http_body_status_code HTTP_BODY_PENDING = 0;
static const http_body_status_code HTTP_BODY_DONE = 1;
static const http_body_status_code HTTP_BODY_ERR_PARSE = 2;
static const http_body_status_code HTTP_BODY_ERR_READ = 3;

typedef struct http_body {
    net_client_t* client;
    http_enc_plain_t plain;
    size_t encsc;
    http_enc_impl_t encs[MAX_TRANSFER_ENCODINGS];
} http_body_t;

typedef struct http_body_status {
    http_body_status_code status;
    slice_t result;
} http_body_status_t;

http_body_t        http_body_new(net_client_t* client);
http_body_status_t http_body_read(http_body_t* self);
http_status_t      http_body_reset(http_body_t* self, http_request_t* request);

#endif //MARS_NET_HTTP_BODY_H
