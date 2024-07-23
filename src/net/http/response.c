//
// Created by pavlo on 07.07.24.
//

#include "response.h"

http_response_t http_response_new(keyval_t headers) {
    return (http_response_t){
        .headers = headers,
    };
}
