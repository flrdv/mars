//
// Created by pavlo on 07.07.24.
//

#ifndef MARS_SRC_LIB_ITOA_H
#define MARS_SRC_LIB_ITOA_H

#include "byte.h"

#include <stdint.h>

int itoa_u64(byte_t* into, uint64_t decimal);
int itoa_hex(byte_t* into, uint64_t hex);

#endif //MARS_SRC_LIB_ITOA_H
