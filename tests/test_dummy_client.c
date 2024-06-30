//
// Created by pavlo on 29.06.24.
//

//
// Created by pavlo on 14.06.24.
//

#include <string.h>
#include "types.h"
#include "net/client.h"
#include "dummy/client.h"
#include "unity.h"

slice_t strslice(char* str) {
    return slice_new((byte_t*)str, strlen(str));
}

void setUp(void) {}
void tearDown(void) {}

void test_reads(void) {
    slice_t reads[4] = {
        strslice("Hel"), strslice("lo,"), strslice(" World"), strslice("!")
    };
    dummy_client_t* dummy = dummy_client_new(reads, 4);
    const net_client_t client = dummy_client_as_net(dummy);

#define TEST_READ(str)               \
    TEST_ASSERT(status.errno == 0);  \
    TEST_ASSERT(slice_cmp(           \
        status.data, strslice(str)   \
    ));

    net_status_t status = client.read(client.self);
    TEST_READ("Hel")
    status = client.read(client.self);
    TEST_READ("lo,")
    status = client.read(client.self);
    TEST_READ(" World")
    status = client.read(client.self);
    TEST_READ("!")
    status = client.read(client.self);
    TEST_ASSERT(status.errno == NET_EOF);
    dummy_client_free(dummy);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_reads);
    return UNITY_END();
}

