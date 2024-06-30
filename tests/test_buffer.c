//
// Created by pavlo on 14.06.24.
//

#include <string.h>
#include "src/misc/buffer.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_buffer_no_overflow(void) {
    buffer_t buffer = buffer_new(10, 10);
    TEST_ASSERT(buffer_append_char(&buffer, "abc", 3));
    TEST_ASSERT(buffer_append_char(&buffer, "de", 2));

    slice_t segment = buffer_segment(&buffer);
    TEST_ASSERT(segment.len == strlen("abcde"));
    TEST_ASSERT(memcmp(segment.data, "abcde", segment.len) == 0);
    buffer_free(&buffer);
}

void test_buffer_overflow(void) {
    buffer_t buffer = buffer_new(1, 5);
    TEST_ASSERT(buffer_append_char(&buffer, "abc", 3));
    TEST_ASSERT(buffer_append_char(&buffer, "de", 2));

    slice_t segment = buffer_segment(&buffer);
    TEST_ASSERT(segment.len == strlen("abcde"));
    TEST_ASSERT(memcmp(segment.data, "abcde", segment.len) == 0);
    buffer_free(&buffer);
}

void test_buffer_exceed_cap(void) {
    buffer_t buffer = buffer_new(1, 3);
    TEST_ASSERT(buffer_append_char(&buffer, "abc", 3));
    TEST_ASSERT_FALSE(buffer_append_char(&buffer, "a", 1));
    buffer_free(&buffer);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_buffer_no_overflow);
    RUN_TEST(test_buffer_overflow);
    RUN_TEST(test_buffer_exceed_cap);
    return UNITY_END();
}
