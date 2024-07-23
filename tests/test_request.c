//
// Created by pavlo on 19.06.24.
//

#include "unity.h"

#include "net/http/request.h"
#include "lib/slice.h"

void setUp(void) {}
void tearDown(void) {}

void test_method_parse(void) {
    TEST_ASSERT_EQUAL(GET,     http_parse_method(slice_str("GET")));
    TEST_ASSERT_EQUAL(HEAD,    http_parse_method(slice_str("HEAD")));
    TEST_ASSERT_EQUAL(POST,    http_parse_method(slice_str("POST")));
    TEST_ASSERT_EQUAL(PUT,     http_parse_method(slice_str("PUT")));
    TEST_ASSERT_EQUAL(DELETE,  http_parse_method(slice_str("DELETE")));
    TEST_ASSERT_EQUAL(CONNECT, http_parse_method(slice_str("CONNECT")));
    TEST_ASSERT_EQUAL(OPTIONS, http_parse_method(slice_str("OPTIONS")));
    TEST_ASSERT_EQUAL(TRACE,   http_parse_method(slice_str("TRACE")));
    TEST_ASSERT_EQUAL(PATCH,   http_parse_method(slice_str("PATCH")));
    TEST_ASSERT_EQUAL(UNKNOWN, http_parse_method(slice_str("FUCK")));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_method_parse);
    return UNITY_END();
}
