//
// Created by pavlo on 28.06.24.
//

#include "plain.h"

http_enc_plain_t http_enc_plain_new(const size_t content_length) {
    return (http_enc_plain_t) {
        .content_length = content_length
    };
}

http_enc_status_t http_enc_plain_read(http_enc_plain_t* self, slice_t data) {
    if (self->content_length == 0)
        return HTTP_ENCODE_STATUS_DONE(SLICE_NULL, SLICE_NULL);

    if (data.len <= self->content_length) {
        self->content_length -= data.len;

        return HTTP_ENCODE_STATUS_PENDING(data, SLICE_NULL);
    }

    slice_t extra = slice_new(&data.elems[self->content_length], data.len-self->content_length);
    data.len = self->content_length; // truncate the source slice a bit to avoid creating a new one
    self->content_length = 0;

    return HTTP_ENCODE_STATUS_DONE(data, extra);
}
