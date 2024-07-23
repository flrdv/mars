//
// Created by Paul on 24.06.24.
//

#include <stdlib.h>
#include <string.h>

#include "unity.h"

#include "lib/slice.h"
#include "lib/buffer.h"
#include "net/http/parser.h"

void setUp(void) {}
void tearDown(void) {}

buffer_t* init_buffer(int prealloc, int max_size) {
    buffer_t* buff = malloc(sizeof(buffer_t));
    *buff = buffer_new(prealloc, max_size);

    return buff;
}

http_parser_t init_parser(void) {
    const int prealloc = 5, step = 5;
    http_request_t* req = malloc(sizeof(http_request_t));
    *req = http_request_new(keyval_new(prealloc, step));
    buffer_t* req_line_buff = init_buffer(256, 512);
    buffer_t* headers_buff = init_buffer(512, 2048);

    return http_new_parser(req, req_line_buff, headers_buff);
}

#define METHOD_ENUM(M) \
    parser.request->method.method == M
#define METHOD_REPR(M) \
    slice_cmp(parser.request->method.repr, slice_str(M))
#define PATH(P) \
    slice_cmp(parser.request->path, slice_str(P))
#define PROTOCOL(P) \
    slice_cmp(parser.request->protocol, slice_str(P))
#define HEADER(K, V) \
    slice_cmp( \
        keyval_get(&parser.request->headers, slice_str(K))->value, slice_str(V) \
    )

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef ARRAY(slice_t) slice_array_t;

slice_array_t divide(char* str, size_t step) {
    size_t len = strlen(str);
    size_t parts_count = len/step+1;
    slice_t* parts = malloc(parts_count * sizeof(slice_t)); //NOLINT

    for (size_t i = 0; i < parts_count; i++) {
        parts[i] = slice_new((byte_t*)&str[i*step], MIN(step, len-(i*step)));
    }

    return (slice_array_t) { parts_count, parts };
}

void test_no_headers(void) {
    http_parser_t parser = init_parser();
    char* sample = "GET / HTTP/1.1\r\n\r\n";
    http_parser_status_t status = http_parse(&parser, (byte_t*)sample, strlen(sample));
    TEST_ASSERT(status.completed);
    char* msg; asprintf(&msg, "parser returned error: %" PRIu16, status.error);
    TEST_ASSERT_MESSAGE(status.error == 0, msg);
    TEST_ASSERT(status.extra_size == 0);

    TEST_ASSERT(METHOD_ENUM(GET));
    TEST_ASSERT(METHOD_REPR("GET"));
    TEST_ASSERT(PATH("/"));
    TEST_ASSERT(PROTOCOL("HTTP/1.1"));
    TEST_ASSERT(keyval_len(&parser.request->headers) == 0);
}

const char* simple_sample = "POST / HTTP/1.1\r\nHello: World\r\nContent-Length: 13\r\n\r\n";

void test_partial_request(size_t step) {
    http_parser_t parser = init_parser();
    size_t len = strlen(simple_sample);
    size_t offset = 0;
    http_parser_status_t status = { 0 };

    for (size_t i = 0; i < len/step+1; i++) {
        slice_t piece = slice_new((byte_t*)&simple_sample[offset], MIN(step, len-i*step));
        offset += step;
        status = http_parse(&parser, piece.ptr, piece.len);

        if (status.completed) break;
    }

    TEST_ASSERT(status.completed); // fails only if no single iteration was done
    char* msg; asprintf(&msg, "parser returned error: %" PRIu16, status.error);
    TEST_ASSERT_MESSAGE(status.error == 0, msg);
    TEST_ASSERT(status.extra_size == 0);

    TEST_ASSERT(METHOD_ENUM(POST));
    TEST_ASSERT(METHOD_REPR("POST"));
    TEST_ASSERT(PATH("/"));
    TEST_ASSERT(PROTOCOL("HTTP/1.1"));
    TEST_ASSERT(keyval_len(&parser.request->headers) == 2);
    TEST_ASSERT(HEADER("hello", "World"));
    TEST_ASSERT(HEADER("content-length", "13"));
    TEST_ASSERT(parser.request->content_length == 13);
    http_parser_free(&parser);
}

void test_with_headers(void) {
    test_partial_request(strlen(simple_sample));
}

void test_partial(void) {
    for (size_t i = 1; i < strlen(simple_sample); i++)
        test_partial_request(i);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_no_headers);
    RUN_TEST(test_with_headers);
    RUN_TEST(test_partial);
    return UNITY_END();
}
