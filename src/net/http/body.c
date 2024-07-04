//
// Created by pavlo on 28.06.24.
//

#include <assert.h>
#include "body.h"

#define ERROR(no) (http_body_read_t) { .errno = no }

http_body_t http_body_new(net_client_t* client) {
    return (http_body_t) {
        .client = client,
        .encsc = 0
    };
}

http_body_read_t http_body_read(http_body_t* self) {
    assert(self->encsc > 0);

    net_status_t read = self->client->read(self->client->self);
    if (read.errno != 0) return ERROR(HTTP_STATUS_BROKEN_PIPE);

    slice_t data = read.data;
    size_t done = 0;

    for (size_t i = 0; i < self->encsc; i++) {
        http_enc_impl_t enc = self->encs[i];
        http_enc_status_t status = enc.read(enc.self, data);

        self->client->preserve(self->client->self, status.extra);
        switch (status.status) {
        case HTTP_ENCODE_PENDING:
            data = status.data;
            break;
        case HTTP_ENCODE_DONE:
            data = status.data;
            ++done;
            break;
        case HTTP_ENCODE_ERR_READ:
        case HTTP_ENCODE_ERR_BAD_DATA:
        default:
            assert(false /* INTERNAL ERROR: received unrecognized status code */);
        }
    }

    if (done) {
        // all encodings MUST
        if (done != self->encsc) return ERROR(HTTP_STATUS_BAD_REQUEST);
    }
}

void http_body_reset(http_body_t* self, http_request_t* request) {
    // TODO: initialize encodings
}
