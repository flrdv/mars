//
// Created by pavlo on 28.06.24.
//

#include <assert.h>
#include "body.h"
#include "lib/keyval.h"
#include "encodings/chunked.h"

// #define ERROR(err) (http_body_status_t) { .status = err }
// #define PENDING(data) (http_body_status_t) { .status = HTTP_BODY_PENDING, .result = data }
// #define DONE(data) (http_body_status_t) { .status = HTTP_BODY_DONE, .result = data }
//
// http_body_t http_body_new(net_client_t* client) {
//     return (http_body_t) {
//         .client = client,
//         .encsc = 0
//     };
// }
//
// static http_body_status_t http_body_feed_encs(http_body_t* self, slice_t data) {
//     for (size_t i = self->encsc; i > 0 && data.len > 0; i--) {
//         http_enc_impl_t enc = self->encs[i-1];
//         http_enc_status_t status = enc.read(enc.self, data);
//         switch (status.status) {
//         case HTTP_ENCODE_PENDING:
//             data = status.data;
//             break;
//         case HTTP_ENCODE_DONE:
//             if (i != self->encsc) {
//                 // TODO: if some underlying encoding is done before those one that wraps it - it's
//                 // TODO: considered a poorly coded message body. However, it's not always true.
//                 // TODO: Such behaviour may blow a leg in future
//                 return ERROR(HTTP_BODY_ERR_PARSE);
//             }
//
//             data = status.data;
//             --self->encsc;
//             break;
//         case HTTP_ENCODE_ERR_READ:     return ERROR(HTTP_BODY_ERR_READ);
//         case HTTP_ENCODE_ERR_BAD_DATA: return ERROR(HTTP_BODY_ERR_PARSE);
//         default:
//             assert(false /* INTERNAL ERROR: received an unrecognized status code from an encoding */);
//         }
//     }
//
//     if (self->encsc == 0) {
//         // TODO: reset all encodings
//         // TODO: store all encodings somewhere where they can be easily obtained
//
//         return DONE(data);
//     }
//
//     return PENDING(data);
// }
//
// http_body_status_t http_body_read(http_body_t* self) {
//     net_status_t read = self->client->read(self->client->self);
//     if (read.error != 0) return ERROR(HTTP_BODY_ERR_READ);
//
//     slice_t data = read.data;
//
//     if (!self->encsc) {
//         http_enc_status_t plain_status = http_enc_plain_read(&self->plain, data);
//         if (plain_status.extra.len) self->client->preserve(self->client->self, plain_status.extra);
//
//         switch (plain_status.status) {
//         case HTTP_ENCODE_PENDING:      return PENDING(plain_status.data);
//         case HTTP_ENCODE_DONE:         return DONE(plain_status.data);
//         case HTTP_ENCODE_ERR_READ:     return ERROR(HTTP_BODY_ERR_READ);
//         case HTTP_ENCODE_ERR_BAD_DATA: return ERROR(HTTP_BODY_ERR_PARSE);
//         default:
//             assert(false /* INTERNAL ERROR: plain body parser returned an unrecognized status code */);
//         }
//     }
//
//     return http_body_feed_encs(self, data);
// }
//
// http_status_t http_body_reset(http_body_t* self, http_request_t* request) {
//     self->encsc = 0;
//     bool chunked = false;
//     keyval_iterator_t te_values = keyval_values(&request->headers, slice_str("transfer-encoding"));
//
//     for (;;) {
//         keyval_pair_t* token = keyval_values_next(&te_values);
//         if (token == NULL) break;
//
//         if (chunked) {
//             // If any transfer-encoding is used, it always MUST be wrapped into chunked.
//             // Chunked cannot be applied more than once.
//             // This means, chunked is ALWAYS the last one token, and can be preceded with
//             // 0 or more optional encodings. Therefore, return an error
//             return HTTP_STATUS_BAD_REQUEST;
//         }
//
//         ++self->encsc;
//         slice_t value = token->value; // TODO: lowercase right on the spot and get rid of cmpfold
//
//         // TODO: implement deflate and gzip encodings
//         if (slice_cmpfold(value, slice_str("chunked"))) {
//             chunked = true;
//             self->encs[self->encsc++] = http_enc_chunked_impl();
//         } else {
//             // return 501 Not Implemented for every request with an unrecognized transfer-encoding
//             // token
//             return HTTP_STATUS_NOT_IMPLEMENTED;
//         }
//     }
//
//     if (!self->encsc) {
//         self->plain = http_enc_plain_new(request->content_length);
//     }
//
//     return 0;
// }
