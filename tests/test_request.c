//
// Created by pavlo on 19.06.24.
//

#include "net/http/request.h"
#include "types.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_method_parse(void) {
    TEST_ASSERT_EQUAL(GET, http_parse_method(new_slice("GET", 3)));
    TEST_ASSERT_EQUAL(GET, http_parse_method(new_slice("GET", 3)));
    TEST_ASSERT_EQUAL(GET, http_parse_method(new_slice("GET", 3)));
    TEST_ASSERT_EQUAL(GET, http_parse_method(new_slice("GET", 3)));
    TEST_ASSERT_EQUAL(GET, http_parse_method(new_slice("GET", 3)));
    TEST_ASSERT_EQUAL(GET, http_parse_method(new_slice("GET", 3)));
    TEST_ASSERT_EQUAL(GET, http_parse_method(new_slice("GET", 3)));
    TEST_ASSERT_EQUAL(GET, http_parse_method(new_slice("GET", 3)));
    TEST_ASSERT_EQUAL(GET, http_parse_method(new_slice("GET", 3)));
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_method_parse);
    return UNITY_END();
}
