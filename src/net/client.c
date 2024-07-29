//
// Created by pavlo on 26.07.24.
//

#include "client.h"

int net_read(const net_client_t* self, slice_t buff) {
    return self->read(self->env, buff);
}

int net_write(const net_client_t* self, slice_t data) {
    return self->write(self->env, data);
}

void net_preserve(const net_client_t* self, slice_t data) {
    self->preserve(self->env, data);
}

struct sockaddr_in net_remote(const net_client_t* self) {
    return self->remote(self->env);
}

int net_close(const net_client_t* self) {
    return self->close(self->env);
}
