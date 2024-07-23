//
// Created by pavlo on 29.06.24.
//

//
// Created by pavlo on 14.06.24.
//

#include <string.h>
#include "lib/slice.h"
#include "net/client.h"
#include "dummy/client.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_reads(void) {
    slice_t reads[4] = {
        slice_str("Hel"), slice_str("lo,"), slice_str(" World"), slice_str("!")
    };
    dummy_client_t* dummy = dummy_client_new(reads, 4);
    const net_client_t client = dummy_client_as_net(dummy);

#define TEST_READ(str)               \
    TEST_ASSERT(status.error == 0);  \
    TEST_ASSERT(slice_cmp(           \
        status.data, slice_str(str)   \
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
    TEST_ASSERT(status.error == NET_EOF);
    dummy_client_free(dummy);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_reads);
    return UNITY_END();
}

