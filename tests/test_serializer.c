//
// Created by pavlo on 07.07.24.
//

#include <iso646.h>

#include "stdlib.h"
#include "string.h"

#include "unity.h"

#include "dummy/client.h"
#include "net/http/serializer.h"

void setUp(void) {}
void tearDown(void) {}

http_serializer_t get_serializer(net_client_t* client, size_t buffsize) {
    slice_t buff = slice_new(malloc(buffsize), buffsize);
    http_serializer_t serializer = http_serializer_new(client, buff);
    return serializer;
}

void test_no_headers(void) {
    const size_t buffsize = 4096;

    dummy_client_t* dummy = dummy_client_new(NULL, 0);
    net_client_t client = dummy_client_as_net(dummy);
    http_serializer_t serializer = get_serializer(&client, buffsize);

    http_request_t request = http_request_new(keyval_new(0, 5));
    http_response_t response = http_response_new(keyval_new(0, 5));
    response.status = HTTP_STATUS_OK;

    TEST_ASSERT_FALSE(http_serializer_write(&serializer, &request, &response));
    TEST_ASSERT(dummy->writes_len == 1);
    TEST_ASSERT(slice_cmp(
        dummy->writes[0],
        slice_str("HTTP/1.1 200 OK\r\nContent-Length: 0\r\n\r\n")
    ));
    dummy_client_free(dummy);
}

void test_headers(void) {
    const size_t buffsize = 4096;

    dummy_client_t* dummy = dummy_client_new(NULL, 0);
    net_client_t client = dummy_client_as_net(dummy);
    http_serializer_t serializer = get_serializer(&client, buffsize);

    http_request_t request = http_request_new(keyval_new(0, 5));
    http_response_t response = http_response_new(keyval_new(0, 5));
    keyval_append(&response.headers, slice_str("some header"), slice_str("some value"));
    response.status = HTTP_STATUS_OK;

    TEST_ASSERT_FALSE(http_serializer_write(&serializer, &request, &response));
    TEST_ASSERT(dummy->writes_len == 1);
    TEST_ASSERT(slice_cmp(
        dummy->writes[0],
        slice_str("HTTP/1.1 200 OK\r\nsome header: some value\r\nContent-Length: 0\r\n\r\n")
    ));
    dummy_client_free(dummy);
}

void test_body(void) {
    const size_t buffsize = 4096;

    dummy_client_t* dummy = dummy_client_new(NULL, 0);
    net_client_t client = dummy_client_as_net(dummy);
    http_serializer_t serializer = get_serializer(&client, buffsize);

    http_request_t request = http_request_new(keyval_new(0, 5));
    http_response_t response = http_response_new(keyval_new(0, 5));
    response.status = HTTP_STATUS_OK;
    response.body = slice_str("Hello, world!");

    TEST_ASSERT_FALSE(http_serializer_write(&serializer, &request, &response));
    TEST_ASSERT(dummy->writes_len == 1);
    TEST_ASSERT(slice_cmp(
        dummy->writes[0],
        slice_str("HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, world!")
    ));
    dummy_client_free(dummy);
}

slice_t concat(slice_t* writes, size_t writes_count) {
    size_t total = 0;

    for (size_t i = 0; i < writes_count; i++)
        total += writes[i].len;

    byte_t* concatenated = malloc(total);
    size_t offset = 0;

    for (size_t i = 0; i < writes_count; i++) {
        memcpy(&concatenated[offset], writes[i].ptr, writes[i].len);
        offset += writes[i].len;
    }

    return slice_new(concatenated, total);
}

void test_small_buffer(void) {
    const size_t buffsize = 24;

    dummy_client_t* dummy = dummy_client_new(NULL, 0);
    net_client_t client = dummy_client_as_net(dummy);
    http_serializer_t serializer = get_serializer(&client, buffsize);

    http_request_t request = http_request_new(keyval_new(0, 5));
    http_response_t response = http_response_new(keyval_new(0, 5));
    const char* long_header_key = "I-just-need-very-long-key-names-in-order-to-trigger-buffer-flushing";
    const char* long_header_value = "that's quite a long line, to be honest";
    keyval_append(&response.headers, slice_str(long_header_key), slice_str(long_header_value));
    keyval_append(&response.headers, slice_str(long_header_key), slice_str(long_header_value));
    keyval_append(&response.headers, slice_str(long_header_key), slice_str(long_header_value));
    response.status = HTTP_STATUS_OK;
    response.body = slice_str(
        "I probably need a long body either. But I'm not really sure, whether it's going to cover any more code"
    );

    TEST_ASSERT_FALSE(http_serializer_write(&serializer, &request, &response));
    slice_t complete_request = concat(dummy->writes, dummy->writes_len);
    TEST_ASSERT(slice_cmp(
        complete_request,
        slice_str(
            "HTTP/1.1 200 OK\r\n"
            "I-just-need-very-long-key-names-in-order-to-trigger-buffer-flushing: that's quite a long line, to be honest\r\n"
            "I-just-need-very-long-key-names-in-order-to-trigger-buffer-flushing: that's quite a long line, to be honest\r\n"
            "I-just-need-very-long-key-names-in-order-to-trigger-buffer-flushing: that's quite a long line, to be honest\r\n"
            "Content-Length: 102\r\n"
            "\r\n"
            "I probably need a long body either. But I'm not really sure, whether it's going to cover any more code")
    ));
    dummy_client_free(dummy);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_no_headers);
    RUN_TEST(test_headers);
    RUN_TEST(test_body);
    RUN_TEST(test_small_buffer);
    return UNITY_END();
}
