//
// Created by pavlo on 19.06.24.
//

#include "types.h"

slice_t new_slice(byte_t* data, size_t len) {
    return (slice_t) {
            .data = data,
            .len = len
    };
}
