//
// Created by pavlo on 01.07.24.
//

#ifndef MARS_SRC_MISC_ARRAY_H
#define MARS_SRC_MISC_ARRAY_H

#include <stddef.h>

#define ARRAY(T) struct { size_t len; T* elems; }

#endif //MARS_SRC_MISC_ARRAY_H
