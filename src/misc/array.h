//
// Created by pavlo on 01.07.24.
//

#ifndef MARS_SRC_MISC_ARRAY_H
#define MARS_SRC_MISC_ARRAY_H

#include <stddef.h>

#define ARRAY(T) struct { T* data; size_t len; }

#endif //MARS_SRC_MISC_ARRAY_H
