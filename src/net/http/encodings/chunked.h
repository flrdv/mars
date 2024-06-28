//
// Created by pavlo on 28.06.24.
//

#ifndef CHUNKED_H
#define CHUNKED_H

#include <stdint.h>
#include <stdbool.h>
#include "net/client.h"
#include "encoding.h"

enum http_encoding_chunked_state;

typedef struct http_encoding_chunked http_encoding_chunked;

http_encoding_chunked http_encoding_chunked_new(void);
http_encode_status http_encoding_chunked_read(http_encoding_chunked* self, const net_client* client);

#endif //CHUNKED_H
