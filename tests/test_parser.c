//
// Created by Paul on 24.06.24.
//

#include <stdlib.h>
#include "types.h"
#include "misc/keyval.h"
#include "net/http/parser.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

static buffer_t* init_buffer(int prealloc, int max_size) {
    buffer_t* buff = malloc(sizeof(buffer_t));
    *buff = buffer_new(prealloc, max_size);

    return buff;
}

static http_parser_t init_parser(void) {
    const int prealloc = 5, step = 5;
    http_request_t* req = malloc(sizeof(http_request_t));
    *req = http_request_new(new_keyval(prealloc, step));
    buffer_t* req_line_buff = init_buffer(256, 512);
    buffer_t* headers_buff = init_buffer(512, 2048);

    return http_new_parser(req, req_line_buff, headers_buff);
}

void test_no_headers(void) {
    http_parser_t parser = init_parser();
    char* sample = "GET / HTTP/1.1\r\n\r\n";
    http_parser_status_t status = http_parse(&parser, (byte_t*)sample, strlen(sample));
    TEST_ASSERT(status.completed);
    char* msg; asprintf(&msg, "parser returned error: %" PRIu16, status.error);
    TEST_ASSERT_MESSAGE(status.error == 0, msg);
    TEST_ASSERT(status.extra_size == 0);

    TEST_ASSERT(parser.request->method.method == GET);
    TEST_ASSERT(slice_cmp(parser.request->method.repr, slice_new("GET", 3)));
    TEST_ASSERT(slice_cmp(parser.request->path, slice_new("/", 1)));
    TEST_ASSERT(slice_cmp(parser.request->protocol, slice_new("HTTP/1.1", 8)));
    TEST_ASSERT(keyval_len(&parser.request->headers) == 0);
}

void test_with_headers(void) {
    http_parser_t parser = init_parser();
    char* sample = "GET / HTTP/1.1\r\nHello: World\r\nContent-Length: 13\r\n\r\n";
    http_parser_status_t status = http_parse(&parser, (byte_t*)sample, (ssize_t)strlen(sample));
    TEST_ASSERT(status.completed);
    char* msg; asprintf(&msg, "parser returned error: %" PRIu16, status.error);
    TEST_ASSERT_MESSAGE(status.error == 0, msg);
    TEST_ASSERT(status.extra_size == 0);

    TEST_ASSERT(parser.request->method.method == GET);
    TEST_ASSERT(slice_cmp(parser.request->method.repr, slice_new("GET", 3)));
    TEST_ASSERT(slice_cmp(parser.request->path, slice_new("/", 1)));
    TEST_ASSERT(slice_cmp(parser.request->protocol, slice_new("HTTP/1.1", 8)));
    TEST_ASSERT(keyval_len(&parser.request->headers) == 2);
    TEST_ASSERT(slice_cmp(
        keyval_get(&parser.request->headers, slice_new("hello", 5))->value,
        slice_new("World", 5)
    ));
    TEST_ASSERT(slice_cmp(
        keyval_get(&parser.request->headers, slice_new("content-length", 14))->value,
        slice_new("13", 2)
    ));
    TEST_ASSERT(parser.request->content_length == 13);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_no_headers);
    RUN_TEST(test_with_headers);
    return UNITY_END();
}
