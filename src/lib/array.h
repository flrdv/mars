//
// Created by pavlo on 01.07.24.
//

#ifndef MARS_SRC_LIB_ARRAY_H
#define MARS_SRC_LIB_ARRAY_H

#include <stddef.h>

#define ARRAY(T) struct { size_t len; T* ptr; }

#endif //MARS_SRC_LIB_ARRAY_H
