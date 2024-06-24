//
// Created by Paul on 24.06.24.
//

#include "memcmpfold.h"

bool memcmpfold(const char* const a, const char* const b, const size_t len) {
    // FIXME: the trick with a|0x20 collides on some non-letters, so generally
    //  isn't the best solution; replace with LUT
    for (size_t i = 0; i < len; i++) {
        if ((a[i]|0x20) != (b[i]|0x20)) return false;
    }

    return true;
}
