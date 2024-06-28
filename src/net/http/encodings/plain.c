//
// Created by pavlo on 28.06.24.
//

#include "plain.h"

http_encoding_plain http_encoding_plain_new(void) {
    return (http_encoding_plain) {
        .content_length = 0
    };
}

http_encode_status http_encoding_plain_read(http_encoding_plain* self, const net_client* client) {
    if (self->content_length == 0) return HTTP_ENCODE_STATUS_OK(SLICE_NULL);

    net_status status = client->read(client->self);
    if (!status.ok) return HTTP_ENCODE_STATUS_ERR(HTTP_ENCODE_ERR_READ);

    slice_t data = status.data;
    if (data.len <= self->content_length) {
        self->content_length -= data.len;

        return HTTP_ENCODE_STATUS_OK(data);
    }

    client->preserve(client->self, slice_new(&data.data[self->content_length], data.len-self->content_length));
    data.len = self->content_length; // truncate the source slice a bit to avoid creating a new one
    self->content_length = 0;

    return HTTP_ENCODE_STATUS_OK(data);
}
