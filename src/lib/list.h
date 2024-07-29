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
    size_t len, cap;
} list_t;

list_t list_new(uint16_t elemsize);
list_t list_alloc(uint16_t elemsize, size_t n);
bool   list_push(list_t* self, void* elem);
bool   list_append(list_t* self, void* elems, size_t elemc);
void*  list_pop(list_t* self);
void   list_rstrip(list_t* self, size_t n);
void*  list_access(list_t* self, size_t index);
void   list_clear(list_t* self);
void   list_free(list_t* self);

#endif //MARS_SRC_LIB_LIST_H
