//
// Created by pavlo on 28.06.24.
//

#include <stdlib.h>
#include <stdio.h>
#include "chunked.h"

#define ADVANCE(i)    \
    data.data += (i); \
    data.len -= (i)

#define MOVE(i, lbl) \
    ADVANCE(i);      \
    goto lbl

http_enc_chunked_t http_enc_chunked_new(void) {
    return (http_enc_chunked_t) {
        .state = CHUNKED_ST_LENGTH,
        .chunk_remaining = 0
    };
}

// hex decode table. Shifted by one in order to catch invalid values, as valid ones
// are guaranteed to be greater than zero
static const byte_t HEX_DECODE_LUT[256] = {
    ['0'] = 0x0 + 1,
    ['1'] = 0x1 + 1,
    ['2'] = 0x2 + 1,
    ['3'] = 0x3 + 1,
    ['4'] = 0x4 + 1,
    ['5'] = 0x5 + 1,
    ['6'] = 0x6 + 1,
    ['7'] = 0x7 + 1,
    ['8'] = 0x8 + 1,
    ['9'] = 0x9 + 1,
    ['a'] = 0xa + 1,
    ['b'] = 0xb + 1,
    ['c'] = 0xc + 1,
    ['d'] = 0xd + 1,
    ['e'] = 0xe + 1,
    ['f'] = 0xf + 1,
    ['A'] = 0xA + 1,
    ['B'] = 0xB + 1,
    ['C'] = 0xC + 1,
    ['D'] = 0xD + 1,
    ['E'] = 0xE + 1,
    ['F'] = 0xF + 1,
};

http_enc_status_t http_enc_chunked_read(http_enc_chunked_t* self, slice_t data) {
    switch (self->state) {
    case CHUNKED_ST_LENGTH:        goto st_length;
    case CHUNKED_ST_LENGTH_LF:     goto st_length_lf;
    case CHUNKED_ST_CHUNK:         goto st_chunk;
    case CHUNKED_ST_CHUNK_FIN:     goto st_chunk_fin;
    case CHUNKED_ST_CHUNK_LF:      goto st_chunk_lf;
    case CHUNKED_ST_LAST_CHUNK:    goto st_last_chunk;
    case CHUNKED_ST_LAST_CHUNK_LF: goto st_last_chunk_lf;
    }

    st_length:
        for (size_t i = 0; i < data.len; i++) {
            switch (data.data[i]) {
            case '\r': MOVE(i+1, st_length_lf);
            case '\n': MOVE(i+1, st_length_next_state);
            default: ;
            }

            const uint8_t halfbyte = HEX_DECODE_LUT[data.data[i]];
            if (!halfbyte) return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);

            self->chunk_remaining = (self->chunk_remaining << 4) | (halfbyte-1);
        }

        self->state = CHUNKED_ST_LENGTH;
        return HTTP_ENCODE_STATUS_PENDING(SLICE_NULL, SLICE_NULL);
    
    st_length_lf:
        if (data.len == 0) {
            self->state = CHUNKED_ST_LENGTH_LF;

            return HTTP_ENCODE_STATUS_PENDING(SLICE_NULL, SLICE_NULL);
        }

        if (data.data[0] != '\n')
            return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);

        ADVANCE(1);
        // fall through

    st_length_next_state:
        if (self->chunk_remaining == 0)
            goto st_last_chunk;

        // fall through

    st_chunk:
        if (data.len < self->chunk_remaining) {
            self->state = CHUNKED_ST_CHUNK;
            self->chunk_remaining -= data.len;
            return HTTP_ENCODE_STATUS_PENDING(data, SLICE_NULL);
        }

        slice_t extra = slice_new(&data.data[self->chunk_remaining], data.len-self->chunk_remaining);
        data.len = self->chunk_remaining;
        self->state = CHUNKED_ST_CHUNK_FIN;
        self->chunk_remaining = 0;

        return HTTP_ENCODE_STATUS_PENDING(data, extra);

    st_chunk_fin:
        if (data.len == 0)
            return HTTP_ENCODE_STATUS_PENDING(SLICE_NULL, SLICE_NULL);

        switch (data.data[0]) {
        case '\r': MOVE(1, st_chunk_lf);
        case '\n': MOVE(1, st_length);
        default: return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);
        }

    st_chunk_lf:
        if (data.len == 0) {
            self->state = CHUNKED_ST_CHUNK_LF;

            return HTTP_ENCODE_STATUS_PENDING(SLICE_NULL, SLICE_NULL);
        }

        if (data.data[0] != '\n')
            return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);

        MOVE(1, st_length);

    st_last_chunk:
        if (data.len == 0) {
            self->state = CHUNKED_ST_LAST_CHUNK;

            return HTTP_ENCODE_STATUS_PENDING(SLICE_NULL, SLICE_NULL);
        }

        switch (data.data[0]) {
        case '\r': MOVE(1, st_last_chunk_lf);
        case '\n': MOVE(1, st_done);
        default: return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);
        }

    st_last_chunk_lf:
        if (data.len == 0) {
            self->state = CHUNKED_ST_LAST_CHUNK_LF;

            return HTTP_ENCODE_STATUS_PENDING(SLICE_NULL, SLICE_NULL);
        }

        // TODO: support trailer
        if (data.data[0] != '\n')
            return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_BAD_DATA);

        ADVANCE(1);
        // fall through

    st_done:
        return HTTP_ENCODE_STATUS_DONE(SLICE_NULL, data);
}
