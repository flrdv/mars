//
// Created by pavlo on 28.06.24.
//

#ifndef MARS_NET_HTTP_SERIALIZER_H
#define MARS_NET_HTTP_SERIALIZER_H

#include "request.h"
#include "response.h"
#include "lib/slice.h"
#include "net/client.h"

typedef struct {
    uint32_t cap;
    uint32_t len;
    byte_t* mem;
} http_serializer_buff_t;

typedef struct {
    net_client_t* client;
    http_serializer_buff_t buff;
} http_serializer_t;

http_serializer_t http_serializer_new(net_client_t* client, slice_t slice);
ssize_t http_serializer_write(http_serializer_t* self, http_request_t* request, http_response_t* resp);
void http_serializer_free(http_serializer_t* self);

#endif //MARS_NET_HTTP_SERIALIZER_H
