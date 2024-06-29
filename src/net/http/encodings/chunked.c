//
// Created by pavlo on 28.06.24.
//

#include <stdlib.h>
#include "chunked.h"

#define JUMP(i, label) \
    data.data += (i);  \
    data.len -= (i);   \
    goto label

enum http_encoding_chunked_state {
    ST_LENGTH,
    ST_LENGTH_LF,
    ST_CHUNK,
    ST_CHUNK_FIN,
    ST_CHUNK_LF,
    ST_LAST_CHUNK,
    ST_LAST_CHUNK_LF,
};

struct http_encoding_chunked {
    bool done;
    enum http_encoding_chunked_state state;
    uint32_t chunk_remaining;
};

http_encoding_chunked http_encoding_chunked_new(void) {
    return (http_encoding_chunked) {
        .done = false,
        .state = ST_LENGTH,
        .chunk_remaining = 0
    };
}

http_encode_status http_encoding_chunked_read(http_encoding_chunked* self, const net_client* client) {
    if (self->done) return HTTP_ENCODE_STATUS_DONE(SLICE_NULL);

    net_status read_status = client->read(client->self);
    // TODO: replace ok by errno and return it instead of bool flag "something went wrong".
    // TODO: This leads to ambiguous errors
    if (read_status.errno) return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_READ);

    slice_t data = read_status.data;

    switch (self->state) {
    case ST_LENGTH:        goto st_length;
    case ST_LENGTH_LF:     goto st_length_lf;
    case ST_CHUNK:         goto st_chunk;
    case ST_CHUNK_FIN:     goto st_chunk_fin;
    case ST_CHUNK_LF:      goto st_chunk_lf;
    case ST_LAST_CHUNK:    goto st_last_chunk;
    case ST_LAST_CHUNK_LF: goto st_last_chunk_lf;
    }

    st_length:
        for (size_t i = 0; i < data.len; i++) {
            switch (data.data[i]) {
            case '\r': JUMP(i+1, st_length_lf);
            case '\n': JUMP(i+1, st_chunk);
            default:
                if (!(data.data[i] >= '0' && data.data[i] <= '9'))
                    return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);

                self->chunk_remaining = (self->chunk_remaining * 10) + data.data[i] - '0';
            }
        }

        self->state = ST_LENGTH;
        return HTTP_ENCODE_STATUS_OK(SLICE_NULL);  // pending data

    st_length_lf:
        if (data.len == 0) {
            self->state = ST_LENGTH_LF;

            return HTTP_ENCODE_STATUS_OK(SLICE_NULL);
        }

        if (data.data[0] != '\n')
            return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);

        JUMP(1, st_chunk);

    st_chunk:
        if (self->chunk_remaining == 0)
            // last chunk
            goto st_last_chunk;

        if (data.len < self->chunk_remaining) {
            self->state = ST_CHUNK;
            self->chunk_remaining -= data.len;
            return HTTP_ENCODE_STATUS_OK(data);
        }

        slice_t extra = slice_new(&data.data[self->chunk_remaining], data.len-self->chunk_remaining);
        client->preserve(client->self, extra);
        data.len = self->chunk_remaining;
        self->state = ST_CHUNK_FIN;
        return HTTP_ENCODE_STATUS_OK(data);

    st_chunk_fin:
        if (data.len == 0)
            return HTTP_ENCODE_STATUS_OK(SLICE_NULL);

        switch (data.data[0]) {
        case '\r': JUMP(1, st_chunk_lf);
        case '\n': JUMP(1, st_length);
        default: return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);
        }

    st_chunk_lf:
        if (data.len == 0) {
            self->state = ST_CHUNK_LF;

            return HTTP_ENCODE_STATUS_OK(SLICE_NULL);
        }

        if (data.data[0] != '\n')
            return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);

        JUMP(1, st_length);

    st_last_chunk:
        if (data.len == 0) {
            self->state = ST_LAST_CHUNK;

            return HTTP_ENCODE_STATUS_OK(SLICE_NULL);
        }

        switch (data.data[0]) {
        case '\r': JUMP(1, st_last_chunk_lf);
        case '\n': JUMP(1, st_length);
        default: return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);
        }

    st_last_chunk_lf:
        if (data.len == 0) {
            self->state = ST_LAST_CHUNK;

            return HTTP_ENCODE_STATUS_OK(SLICE_NULL);
        }

        // TODO: support trailer
        if (data.data[0] != '\n')
            return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);

        JUMP(1, st_length);
}
