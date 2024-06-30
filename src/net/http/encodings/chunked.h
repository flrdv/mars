//
// Created by pavlo on 28.06.24.
//

#ifndef CHUNKED_H
#define CHUNKED_H

#include "net/client.h"
#include "encoding.h"

enum http_encoding_chunked_state {
    CHUNKED_ST_LENGTH,
    CHUNKED_ST_LENGTH_LF,
    CHUNKED_ST_CHUNK,
    CHUNKED_ST_CHUNK_FIN,
    CHUNKED_ST_CHUNK_LF,
    CHUNKED_ST_LAST_CHUNK,
    CHUNKED_ST_LAST_CHUNK_LF,
};

typedef struct http_encoding_chunked_t {
    enum http_encoding_chunked_state state;
    uint32_t chunk_remaining;
} http_encoding_chunked_t;

http_encoding_chunked_t http_encoding_chunked_new(void);
http_encode_status_t http_encoding_chunked_read(http_encoding_chunked_t* self, const net_client_t* client);

#endif //CHUNKED_H
