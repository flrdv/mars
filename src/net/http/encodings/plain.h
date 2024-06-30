//
// Created by pavlo on 28.06.24.
//

#ifndef PLAIN_H
#define PLAIN_H

#include <stddef.h>
#include "net/client.h"
#include "encoding.h"

typedef struct http_encoding_plain_t {
    size_t content_length;
} http_encoding_plain_t;

http_encoding_plain_t http_encoding_plain_new(size_t);
http_encode_status_t http_encoding_plain_read(http_encoding_plain_t* self, const net_client_t* client);

#endif //PLAIN_H
