//
// Created by pavlo on 28.06.24.
//

#ifndef ENCODING_H
#define ENCODING_H

#include "lib/slice.h"

#define HTTP_ENCODE_PENDING 0
#define HTTP_ENCODE_DONE 1
#define HTTP_ENCODE_ERR_READ 2
#define HTTP_ENCODE_ERR_BAD_DATA 3

#define HTTP_ENCODE_STATUS_PENDING(out, ex) \
    (http_enc_status_t) { .status = HTTP_ENCODE_PENDING, .data = out, .extra = ex }
#define HTTP_ENCODE_STATUS_DONE(out, ex) \
    (http_enc_status_t) { .status = HTTP_ENCODE_DONE, .data = out, .extra = ex }
#define HTTP_ENCODE_STATUS_ERR(err) \
    (http_enc_status_t) { .status = err, .data = SLICE_NULL, .extra = SLICE_NULL }

typedef struct http_enc_status_t {
    uint8_t status;
    slice_t data;
    slice_t extra;
} http_enc_status_t;

typedef struct http_enc_impl_t {
    void* self;
    http_enc_status_t (*read)(void* self, slice_t data);
    void (*reset)(void* self);
} http_enc_impl_t;

#endif //ENCODING_H
