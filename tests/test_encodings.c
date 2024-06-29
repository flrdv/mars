//
// Created by pavlo on 28.06.24.
//

#include <string.h>
#include "net/http/encodings/plain.h"
#include "types.h"
#include "unity.h"
#include "dummy/client.h"

void setUp(void) {}
void tearDown(void) {}

slice_t strslice(char* str) {
    return slice_new((byte_t*)str, strlen(str));
}

void test_plain(void) {
    slice_t reads[4] = {
        strslice("Hel"), strslice("lo, "), strslice("World"), strslice("!extra")
    };
    dummy_client* dummy = dummy_client_new(reads, 4);
    const net_client client = dummy_client_as_net_client(dummy);
    http_encoding_plain enc = http_encoding_plain_new(13);

    http_encode_status status = http_encoding_plain_read(&enc, &client);
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

    TEST_ASSERT(slice_cmp(dummy->preserve, strslice("extra")));

    dummy_client_free(dummy);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_plain);
    return UNITY_END();
}
