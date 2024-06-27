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
    http_parse(&parser, (byte_t*)sample, strlen(sample));
    printf("method: %d\n", parser.request->method.method);
    TEST_ASSERT(parser.request->method.method == GET);
    TEST_ASSERT(slice_cmp(parser.request->method.repr, new_slice("GET", 3)));
    TEST_ASSERT(slice_cmp(parser.request->path, new_slice("/", 1)));
    TEST_ASSERT(slice_cmp(parser.request->protocol, new_slice("HTTP/1.1", 8)));
    TEST_ASSERT(keyval_len(&parser.request->headers) == 0);
}

void test_with_headers(void) {
    http_parser_t parser = init_parser();
    char* sample = "GET / HTTP/1.1\r\nHello: World\r\nContent-Length: 13\r\n\r\n";
    http_parse(&parser, (byte_t*)sample, (ssize_t)strlen(sample));
    printf("method: %d\n", parser.request->method.method);
    TEST_ASSERT(parser.request->method.method == GET);
    TEST_ASSERT(slice_cmp(parser.request->method.repr, new_slice("GET", 3)));
    TEST_ASSERT(slice_cmp(parser.request->path, new_slice("/", 1)));
    TEST_ASSERT(slice_cmp(parser.request->protocol, new_slice("HTTP/1.1", 8)));
    TEST_ASSERT(keyval_len(&parser.request->headers) == 2);
    TEST_ASSERT(slice_cmp(
        keyval_get(&parser.request->headers, new_slice("hello", 5))->value,
        new_slice("World", 5)
    ));
    TEST_ASSERT(slice_cmp(
        keyval_get(&parser.request->headers, new_slice("content-length", 14))->value,
        new_slice("13", 2)
    ));
    TEST_ASSERT(parser.request->content_length == 13);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_no_headers);
    RUN_TEST(test_with_headers);
    return UNITY_END();
}
