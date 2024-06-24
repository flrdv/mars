//
// Created by floordiv on 23-Jun-24.
//

#include "misc/keyval.h"
#include "types.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

static inline slice_t slice(const char* data, const size_t len) {
    return new_slice((byte_t*)data, len);
}

void test_prealloc_get(void) {
    const int prealloc = 5, step = 10;
    keyval_storage_t storage = new_keyval(prealloc, step);
    keyval_append(&storage, slice("HELLO", 5), slice("world", 5));
    keyval_append(&storage, slice("abba", 4), slice("baab", 4));
    TEST_ASSERT(slice_cmp(
            slice("world", 5),
            keyval_get(&storage, slice("hello", 5))->value)
    );
    keyval_free(&storage);
}

void test_grow(void) {
    const int prealloc = 2, step = 10;
    keyval_storage_t storage = new_keyval(prealloc, step);
    keyval_append(&storage, slice("HELLO", 5), slice("world", 5));
    keyval_append(&storage, slice("abba", 4), slice("baab", 4));
    keyval_append(&storage, slice("lol", 3), slice("rofl", 4));
    TEST_ASSERT(slice_cmp(
            slice("rofl", 4),
            keyval_get(&storage, slice("lol", 3))->value)
    );
    keyval_clear(&storage);
    TEST_ASSERT(keyval_get(&storage, slice("lol", 3)) == NULL);
    keyval_free(&storage);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_prealloc_get);
    RUN_TEST(test_grow);
    return UNITY_END();
}
