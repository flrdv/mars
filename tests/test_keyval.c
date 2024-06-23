//
// Created by floordiv on 23-Jun-24.
//

#include "misc/keyval.h"
#include "types.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

inline slice_t slice(const char* data, const size_t len) {
    return new_slice((byte_t*)data, len);
}

void test_prealloc(void) {
    const int prealloc = 10, step = 10;
    keyval_storage_t storage = new_keyval(prealloc, step);
    keyval_append(&storage, slice("Hello", 5), slice("world", 5));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_prealloc);
    return UNITY_END();
}
