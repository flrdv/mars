//
// Created by pavlo on 28.06.24.
//

#include <assert.h>
#include "body.h"

http_body_t http_body_new(net_client_t* client) {
    return (http_body_t) {
        .client = client,
        .encsc = 0,
    };
}



http_body_read_t http_body_read(http_body_t* self) {
    assert(self->encsc > 0);

    net_status_t read = self->client->read(self->client->self);
    if (read.errno != 0)
        return (http_body_read_t) { .errno = read.errno };

    for (size_t i = 0; i < self->encsc; i++) {

    }
}

void http_body_reset(http_body_t* self, http_request_t* request) {
    // TODO: initialize encodings
}
