//
// Created by pavlo on 28.06.24.
//

#ifndef PLAIN_H
#define PLAIN_H

#include <stddef.h>
#include "encoding.h"

typedef struct http_enc_plain_t {
    size_t content_length;
} http_enc_plain_t;

http_enc_plain_t http_enc_plain_new(size_t);
http_enc_status_t http_enc_plain_read(http_enc_plain_t* self, slice_t data);

#endif //PLAIN_H
