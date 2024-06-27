//
// Created by pavlo on 19.06.24.
//

#include "net/http/request.h"
#include "types.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

static inline slice_t slice(const char* data, const size_t len) {
    return slice_new((byte_t*)data, len);
}

void test_method_parse(void) {
    TEST_ASSERT_EQUAL(GET, http_parse_method(slice("GET", 3)));
    TEST_ASSERT_EQUAL(HEAD, http_parse_method(slice("HEAD", 4)));
    TEST_ASSERT_EQUAL(POST, http_parse_method(slice("POST", 4)));
    TEST_ASSERT_EQUAL(PUT, http_parse_method(slice("PUT", 3)));
    TEST_ASSERT_EQUAL(DELETE, http_parse_method(slice("DELETE", 6)));
    TEST_ASSERT_EQUAL(CONNECT, http_parse_method(slice("CONNECT", 7)));
    TEST_ASSERT_EQUAL(OPTIONS, http_parse_method(slice("OPTIONS", 7)));
    TEST_ASSERT_EQUAL(TRACE, http_parse_method(slice("TRACE", 5)));
    TEST_ASSERT_EQUAL(PATCH, http_parse_method(slice("PATCH", 5)));
    TEST_ASSERT_EQUAL(UNKNOWN, http_parse_method(slice("FUCK", 4)));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_method_parse);
    return UNITY_END();
}
