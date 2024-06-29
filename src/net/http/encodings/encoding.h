//
// Created by pavlo on 28.06.24.
//

#ifndef ENCODING_H
#define ENCODING_H

#include "types.h"

#define HTTP_ENCODE_PENDING 0
#define HTTP_ENCODE_DONE 1
#define HTTP_ENCODE_ERR_READ 2
#define HTTP_ENCODE_ERR_BAD_DATA 3

#define HTTP_ENCODE_STATUS_OK(out)   (http_encode_status) { .status = HTTP_ENCODE_PENDING, .data = out }
#define HTTP_ENCODE_STATUS_DONE(out) (http_encode_status) { .status = HTTP_ENCODE_DONE,    .data = out }
#define HTTP_ENCODE_STATUS_ERR(err)  (http_encode_status) { .status = err,          .data = SLICE_NULL }

typedef enum {
    HTTP_ENC_PLAIN,
    HTTP_ENC_CHUNKED
} http_encoding;

typedef struct http_encode_status {
    uint8_t status;
    slice_t data;
} http_encode_status;

#endif //ENCODING_H
