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

void test_reads_bigbuffer(void) {
    const int buffsize = 1024;

    slice_t reads[4] = {
        slice_str("Hel"), slice_str("lo,"), slice_str(" World"), slice_str("!")
    };
    dummy_client_t* dummy = dummy_client_new(reads, 4);
    const net_client_t client = dummy_client_as_net(dummy);
    byte_t* buff[buffsize];

#define TEST_READ(str)                     \
    TEST_ASSERT(n > 0);                    \
    TEST_ASSERT(slice_cmp(                 \
        slice_new(buff, n), slice_str(str) \
    ));

    int n = client.read(client.self, slice_new(buff, buffsize));
    TEST_READ("Hel")
    n = client.read(client.self, slice_new(buff, buffsize));
    TEST_READ("lo,")
    n = client.read(client.self, slice_new(buff, buffsize));
    TEST_READ(" World")
    n = client.read(client.self, slice_new(buff, buffsize));
    TEST_READ("!")
    n = client.read(client.self, slice_new(buff, buffsize));
    TEST_ASSERT(n == NET_EOF);
    net_free(&client);
}

void test_reads_smallbuffer(void) {
    char* teststr = "Hello, world!";
    slice_t reads[4] = {
        slice_str("Hel"), slice_str("lo,"), slice_str(" world"), slice_str("!")
    };
    dummy_client_t* dummy = dummy_client_new(reads, 4);
    const net_client_t client = dummy_client_as_net(dummy);
    byte_t byte;

    for (int i = 0; i < strlen(teststr); i++) {
        int n = net_read(&client, slice_new(&byte, 1));
        char* buff[512];
        snprintf(buff, 512, "failed at char %d", i);
        TEST_ASSERT(n == 1);
        TEST_ASSERT_MESSAGE(byte == teststr[i], buff);
    }

    int n = net_read(&client, slice_new(&byte, 1));
    TEST_ASSERT(n == NET_EOF);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_reads_bigbuffer);
    RUN_TEST(test_reads_smallbuffer);
    return UNITY_END();
}

