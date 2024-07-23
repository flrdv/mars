//
// Created by pavlo on 17.07.24.
//

#ifndef MARS_SRC_LIB_LIST_H
#define MARS_SRC_LIB_LIST_H

#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>

typedef struct {
    uint16_t elemsize;
    void* ptr;
    int len, cap;
} list_t;

list_t list_new(uint16_t elemsize);
list_t list_alloc(uint16_t elemsize, int n);
bool   list_append(list_t* self, void* elem);
bool   list_append_many(list_t* self, void* elems, int elemc);
void*  list_access(list_t* self, uint index);
void   list_free(list_t* self);

#endif //MARS_SRC_LIB_LIST_H
