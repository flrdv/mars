//
// Created by floordiv on 23-Jun-24.
//

#include "misc/keyval.h"
#include "types.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

static slice_t strslice(const char* data) {
    return new_slice((byte_t*)data, strlen(data));
}

void test_prealloc_get(void) {
    const int prealloc = 5, step = 10;
    keyval_storage_t storage = new_keyval(prealloc, step);
    keyval_append(&storage, strslice("HELLO"), strslice("world"));
    keyval_append(&storage, strslice("abba"), strslice("baab"));
    TEST_ASSERT(slice_cmp(
            strslice("world"),
            keyval_get(&storage, strslice("hello"))->value)
    );
    keyval_free(&storage);
}

void test_grow(void) {
    const int prealloc = 2, step = 10;
    keyval_storage_t storage = new_keyval(prealloc, step);
    keyval_append(&storage, strslice("HELLO"), strslice("world"));
    keyval_append(&storage, strslice("abba"), strslice("baab"));
    keyval_append(&storage, strslice("lol"), strslice("rofl"));
    const keyval_pair_t* value = keyval_get(&storage, strslice("lol"));
    TEST_ASSERT(value != NULL);
    TEST_ASSERT(slice_cmp(
            strslice("rofl"),
            value->value)
    );
    keyval_clear(&storage);
    TEST_ASSERT(keyval_get(&storage, strslice("lol")) == NULL);
    keyval_free(&storage);
}

void test_values_iter(void) {
    const int prealloc = 5, step = 10;
    keyval_storage_t storage = new_keyval(prealloc, step);
    keyval_append(&storage, strslice("HELLO"), strslice("world"));
    keyval_append(&storage, strslice("lol"), strslice("rofl"));
    keyval_append(&storage, strslice("hello"), strslice("WORLD"));
    keyval_append(&storage, strslice("heLLo"), strslice("woRLd"));
    keyval_iterator_t values = keyval_values(&storage, strslice("Hello"));
    TEST_ASSERT(slice_cmp(keyval_values_next(&values)->value, strslice("world")));
    TEST_ASSERT(slice_cmp(keyval_values_next(&values)->value, strslice("WORLD")));
    TEST_ASSERT(slice_cmp(keyval_values_next(&values)->value, strslice("woRLd")));
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
