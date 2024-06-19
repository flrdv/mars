//
// Created by pavlo on 14.06.24.
//

#include <string.h>
#include "misc/buffer/buffer.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_buffer_no_overflow(void) {
    buffer_buffer_t buffer = buffer_new(10, 10);
    byte_t str1[] = "abc";
    TEST_ASSERT(buffer_append(&buffer, str1, 3));
    byte_t str2[] = "de";
    TEST_ASSERT(buffer_append(&buffer, str2, 2));

    slice_t segment = buffer_segment(&buffer);
    TEST_ASSERT(segment.len == strlen("abcde"));
    TEST_ASSERT(memcmp(segment.data, "abcde", segment.len) == 0);
    buffer_free(&buffer);
}

void test_buffer_overflow(void) {
    buffer_buffer_t buffer = buffer_new(1, 5);
    byte_t str1[] = "abc";
    TEST_ASSERT(buffer_append(&buffer, str1, 3));
    byte_t str2[] = "de";
    TEST_ASSERT(buffer_append(&buffer, str2, 2));

    slice_t segment = buffer_segment(&buffer);
    TEST_ASSERT(segment.len == strlen("abcde"));
    TEST_ASSERT(memcmp(segment.data, "abcde", segment.len) == 0);
    buffer_free(&buffer);
}

void test_buffer_exceed_cap(void) {
    buffer_buffer_t buffer = buffer_new(1, 3);
    byte_t str1[] = "abc";
    TEST_ASSERT(buffer_append(&buffer, str1, 3));
    byte_t str2[] = "a";
    TEST_ASSERT_FALSE(buffer_append(&buffer, str2, 1));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_buffer_no_overflow);
    RUN_TEST(test_buffer_overflow);
    RUN_TEST(test_buffer_exceed_cap);
    return UNITY_END();
}
