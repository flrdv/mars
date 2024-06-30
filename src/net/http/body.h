//
// Created by pavlo on 28.06.24.
//

#ifndef BODY_H
#define BODY_H

#include <stdbool.h>
#include "net/client.h"

enum http_transfer_encoding {
    Chunked,
    GZIP,
    ZSTD
};

#define MAX_TRANSFER_ENCODINGS 5

typedef struct http_body_te_t {
    enum http_transfer_encoding encs[MAX_TRANSFER_ENCODINGS];
} http_body_te_t;

typedef struct http_body_t {
    http_body_te_t transfer_encodings;
    net_client_t client;
} http_body_t;

slice_t http_body_read(http_body_t* self);

#endif //BODY_H
