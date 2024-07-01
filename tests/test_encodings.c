//
// Created by pavlo on 28.06.24.
//

#include <string.h>
#include <stdlib.h>
#include "net/http/encodings/plain.h"
#include "net/http/encodings/chunked.h"
#include "types.h"
#include "unity.h"
#include "dummy/client.h"
#include "misc/buffer.h"

void setUp(void) {}
void tearDown(void) {}

slice_t strslice(char* str) {
    return slice_new((byte_t*)str, strlen(str));
}

void test_plain(void) {
    slice_t reads[4] = {
        strslice("Hel"), strslice("lo, "), strslice("World"), strslice("!extra")
    };
    dummy_client_t* dummy = dummy_client_new(reads, 4);
    const net_client_t client = dummy_client_as_net(dummy);
    http_encoding_plain_t enc = http_encoding_plain_new(13);

    http_encode_status_t status = http_encoding_plain_read(&enc, &client);
    TEST_ASSERT(status.status == HTTP_ENCODE_PENDING);
    TEST_ASSERT(status.data.len == 3);

    status = http_encoding_plain_read(&enc, &client);
    TEST_ASSERT(status.status == HTTP_ENCODE_PENDING);
    TEST_ASSERT(status.data.len == 4);

    status = http_encoding_plain_read(&enc, &client);
    TEST_ASSERT(status.status == HTTP_ENCODE_PENDING);
    TEST_ASSERT(status.data.len == 5);

    status = http_encoding_plain_read(&enc, &client);
    TEST_ASSERT(status.status == HTTP_ENCODE_DONE);
    TEST_ASSERT(status.data.len == 1);

    TEST_ASSERT(slice_cmp(dummy->preserved, strslice("extra")));

    dummy_client_free(dummy);
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

slice_t test_partial_chunked(char* sample, size_t step) {
    buffer_t buffer = buffer_new(strlen(sample), strlen(sample)*2);
    http_encoding_chunked_t reader = http_encoding_chunked_new();
    dummy_client_t* dummy = mock_client(sample, step);
    net_client_t client = dummy_client_as_net(dummy);

    for (;;) {
        http_encode_status_t status = http_encoding_chunked_read(&reader, &client);
        switch (status.status) {
        case HTTP_ENCODE_DONE:
            TEST_ASSERT(buffer_append(&buffer, status.data.data, status.data.len));
            goto exit;
        case HTTP_ENCODE_PENDING:
            TEST_ASSERT(buffer_append(&buffer, status.data.data, status.data.len));
            break;
        case HTTP_ENCODE_ERR_READ:     TEST_FAIL_MESSAGE("received unexpected ERR_READ");
        case HTTP_ENCODE_ERR_BAD_DATA: TEST_FAIL_MESSAGE("received unexpected ERR_BAD_DATA");
        default:                       TEST_FAIL_MESSAGE("received unrecognized encoding status");
        }
    }

exit:
    free(dummy->reads);
    dummy_client_free(dummy);
    slice_t data = slice_clone(buffer_segment(&buffer));
    buffer_free(&buffer);

    return data;
}

void test_chunked(void) {
    char* sample = "d\r\nHello, world!\r\n5\r\npavlo\r\n0\r\n\r\nextra";

    for (size_t i = 1; i <= strlen(sample); i++) {
        slice_t result = test_partial_chunked(sample, i);
        char* out; asprintf(&out, "failed on step size: %lu\n", i);
        TEST_ASSERT_MESSAGE(memcmp("Hello, world!pavlo", result.data, result.len) == 0, out);
    }
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_plain);
    RUN_TEST(test_chunked);
    return UNITY_END();
}
