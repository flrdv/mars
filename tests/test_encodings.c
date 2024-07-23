//
// Created by pavlo on 28.06.24.
//

#include <string.h>
#include <stdlib.h>

#include "unity.h"

#include "lib/slice.h"
#include "lib/buffer.h"
#include "dummy/client.h"
#include "net/http/encodings/plain.h"
#include "net/http/encodings/chunked.h"
#include "net/http/encodings/encoding.h"

void setUp(void) {}
void tearDown(void) {}

slice_t strslice(char* str) {
    return slice_new((byte_t*)str, strlen(str));
}

void test_plain(void) {
    http_enc_plain_t enc = http_enc_plain_new(13);

#define TEST_STATUS(s, str, ex) \
    TEST_ASSERT(status.status == s); \
    TEST_ASSERT(slice_cmp(strslice(str), status.data)); \
    TEST_ASSERT(slice_cmp(status.extra, ex))

    http_enc_status_t status = http_enc_plain_read(&enc, strslice("Hel"));
    TEST_STATUS(HTTP_ENCODE_PENDING, "Hel", SLICE_NULL);

    status = http_enc_plain_read(&enc, strslice("lo, "));
    TEST_STATUS(HTTP_ENCODE_PENDING, "lo, ", SLICE_NULL);

    status = http_enc_plain_read(&enc, strslice("World"));
    TEST_STATUS(HTTP_ENCODE_PENDING, "World", SLICE_NULL);

    status = http_enc_plain_read(&enc, strslice("!extra"));
    TEST_STATUS(HTTP_ENCODE_DONE, "!", strslice("extra"));
}

size_t min(const size_t a, const size_t b) {
    return a < b ? a : b;
}

dummy_client_t* mock_client(char* str, size_t step) {
    size_t len = strlen(str);
    size_t parts_count = len/step+1;
    slice_t* parts = malloc(sizeof(slice_t) * parts_count);

    for (size_t i = 0; i < parts_count; i++) {
        parts[i] = slice_new((byte_t*)&str[i*step], min(step, len-(i*step)));
    }

    return dummy_client_new(parts, parts_count);
}

bool feed_data(http_enc_chunked_t* reader, net_client_t* client, buffer_t* buffer) {
    for (;;) {
        net_status_t read = client->read(client->self);
        TEST_ASSERT(read.error == NET_OK);

        http_enc_status_t status = http_enc_chunked_read(reader, read.data);
        client->preserve(client->self, status.extra);
        switch (status.status) {
        case HTTP_ENCODE_DONE:
            TEST_ASSERT(buffer_append(buffer, status.data.ptr, status.data.len));
            return true;
        case HTTP_ENCODE_PENDING:
            TEST_ASSERT(buffer_append(buffer, status.data.ptr, status.data.len));
            break;
        case HTTP_ENCODE_ERR_READ:     TEST_FAIL_MESSAGE("received unexpected ERR_READ");
        case HTTP_ENCODE_ERR_BAD_DATA: TEST_FAIL_MESSAGE("received unexpected ERR_BAD_DATA");
        default:                       TEST_FAIL_MESSAGE("received unrecognized encoding status");
        }
    }
}

slice_t test_partial_chunked(char* sample, size_t step) {
    buffer_t buffer = buffer_new(strlen(sample), strlen(sample)*2);
    http_enc_chunked_t reader = http_enc_chunked_new();
    dummy_client_t* dummy = mock_client(sample, step);
    net_client_t client = dummy_client_as_net(dummy);
    feed_data(&reader, &client, &buffer);
    free(dummy->reads);
    dummy_client_free(dummy);
    slice_t data = slice_clone(buffer_segment(&buffer));
    buffer_free(&buffer);

    return data;
}

void test_chunked(void) {
    char* sample = "d\r\nHello, world!\r\n5\r\npavlo\r\n0\r\n\r\n";

    for (size_t i = 1; i <= strlen(sample); i++) {
        slice_t result = test_partial_chunked(sample, i);
        char* out; asprintf(&out, "failed on step size: %lu\n", i);
        TEST_ASSERT_MESSAGE(memcmp("Hello, world!pavlo", result.ptr, result.len) == 0, out);
    }
}

void test_chunked_extra(void) {
    char* sample = "d\r\nHello, world!\r\n5\r\npavlo\r\n0\r\n\r\nextra";
    dummy_client_t* dummy = mock_client(sample, strlen(sample));
    net_client_t client = dummy_client_as_net(dummy);
    buffer_t buff = buffer_new(0, 1024);
    http_enc_chunked_t reader = http_enc_chunked_new();
    feed_data(&reader, &client, &buff);

    TEST_ASSERT(slice_cmp(dummy->preserved, strslice("extra")));
    buffer_free(&buff);
    dummy_client_free(dummy);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_plain);
    RUN_TEST(test_chunked);
    RUN_TEST(test_chunked_extra);
    return UNITY_END();
}
