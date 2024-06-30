//
// Created by pavlo on 28.06.24.
//

#include <stdio.h>
#include "plain.h"

http_encoding_plain_t http_encoding_plain_new(const size_t content_length) {
    return (http_encoding_plain_t) {
        .content_length = content_length
    };
}

http_encode_status_t http_encoding_plain_read(http_encoding_plain_t* self, const net_client_t* client) {
    if (self->content_length == 0) return HTTP_ENCODE_STATUS_DONE(SLICE_NULL);

    const net_status_t status = client->read(client->self);
    if (status.errno) return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_READ);

    slice_t data = status.data;
    if (data.len <= self->content_length) {
        self->content_length -= data.len;

        return HTTP_ENCODE_STATUS_PENDING(data);
    }

    client->preserve(client->self, slice_new(&data.data[self->content_length], data.len-self->content_length));
    data.len = self->content_length; // truncate the source slice a bit to avoid creating a new one
    self->content_length = 0;

    return HTTP_ENCODE_STATUS_DONE(data);
}
