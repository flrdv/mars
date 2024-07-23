//
// Created by floordiv on 23-Jun-24.
//

#include <string.h>
#include "lib/keyval.h"
#include "lib/slice.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_prealloc_get(void) {
    const int prealloc = 5, step = 10;
    keyval_t storage = keyval_new(prealloc, step);
    keyval_append(&storage, slice_str("HELLO"), slice_str("world"));
    keyval_append(&storage, slice_str("abba"), slice_str("baab"));
    TEST_ASSERT(slice_cmp(
            slice_str("world"),
            keyval_get(&storage, slice_str("hello"))->value)
    );
    keyval_free(&storage);
}

void test_grow(void) {
    const int prealloc = 2, step = 10;
    keyval_t storage = keyval_new(prealloc, step);
    keyval_append(&storage, slice_str("HELLO"), slice_str("world"));
    keyval_append(&storage, slice_str("abba"), slice_str("baab"));
    keyval_append(&storage, slice_str("lol"), slice_str("rofl"));
    const keyval_pair_t* value = keyval_get(&storage, slice_str("lol"));
    TEST_ASSERT(value != NULL);
    TEST_ASSERT(slice_cmp(
            slice_str("rofl"),
            value->value)
    );
    keyval_clear(&storage);
    TEST_ASSERT(keyval_get(&storage, slice_str("lol")) == NULL);
    keyval_free(&storage);
}

void test_values_iter(void) {
    const int prealloc = 5, step = 10;
    keyval_t storage = keyval_new(prealloc, step);
    keyval_append(&storage, slice_str("HELLO"), slice_str("world"));
    keyval_append(&storage, slice_str("lol"), slice_str("rofl"));
    keyval_append(&storage, slice_str("hello"), slice_str("WORLD"));
    keyval_append(&storage, slice_str("heLLo"), slice_str("woRLd"));
    keyval_iterator_t values = keyval_values(&storage, slice_str("Hello"));
    TEST_ASSERT(slice_cmp(keyval_values_next(&values)->value, slice_str("world")));
    TEST_ASSERT(slice_cmp(keyval_values_next(&values)->value, slice_str("WORLD")));
    TEST_ASSERT(slice_cmp(keyval_values_next(&values)->value, slice_str("woRLd")));
    TEST_ASSERT(keyval_values_next(&values) == NULL);
    keyval_free(&storage);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_prealloc_get);
    RUN_TEST(test_grow);
    RUN_TEST(test_values_iter);
    return UNITY_END();
}
