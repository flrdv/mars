//
// Created by pavlo on 28.06.24.
//

#include "client.h"

#include <stdlib.h>
#include <string.h>
#include "lib/slice.h"

dummy_client_t* dummy_client_new(slice_t* reads, size_t len) {
    dummy_client_t* dummy = malloc(sizeof(dummy_client_t));
    *dummy = (dummy_client_t) {
        .reads = reads,
        .reads_cap = len,
        .reads_head = 0,
        .writes = NULL,
        .writes_cap = 0,
        .writes_len = 0,
        .preserved = SLICE_NULL
    };

    return dummy;
}

static int dummy_client_write(void* s, slice_t data) {
    dummy_client_t* self = s;
    if (self->writes_len >= self->writes_cap) {
        self->writes_cap += 10;
        slice_t* new_writes = malloc(self->writes_cap * sizeof(self->writes[0]));
        memcpy(new_writes, self->writes, self->writes_len * sizeof(self->writes[0]));
        free(self->writes);
        self->writes = new_writes;
    }

    self->writes[self->writes_len++] = slice_clone(data);
    return 0;
}

static void dummy_client_preserve(void* self, slice_t data) {
    ((dummy_client_t*)self)->preserved = data;
}

static int dummy_client_read(void* s, slice_t buff) {
    dummy_client_t* self = s;
    if (self->preserved.len) {
        size_t n = slice_copyto(self->preserved, buff);
        self->preserved = slice_offset(self->preserved, n);

        return n;
    }

    if (self->reads_head >= self->reads_cap)
        return NET_EOF;

    slice_t chunk = self->reads[self->reads_head++];
    size_t n = slice_copyto(chunk, buff);
    if (n < chunk.len)
        dummy_client_preserve(s, slice_offset(chunk, n));

    return n;
}

static int dummy_client_close(void* s) {
    // so next calls will return EOF
    dummy_client_t* self = s;
    self->reads_head = self->reads_cap;
    return 0;
}

void dummy_client_free(void* s) {
    // don't free reads, as it belongs to caller. In case we try, it may
    // result in corruption, e.g. reads are stack-allocated
    dummy_client_t* self = s;

    for (size_t i = 0; i < self->writes_len; i++)
        slice_free(self->writes[i]);

    free(self->writes);
    // free self, as constructor always heap-allocates it, and event loop
    // has no direct access to dummy by itself.
    free(self);
}

net_client_t dummy_client_as_net(dummy_client_t* self) {
    return (net_client_t) {
        .env = self,
        .read = dummy_client_read,
        .write = dummy_client_write,
        .preserve = dummy_client_preserve,
        .close = dummy_client_close
    };
}
