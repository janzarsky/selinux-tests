#include <stdio.h>
#include <semanage/semanage.h>
#include <semanage/handle.h>
#include <CUnit/Basic.h>

#include "test_node.h"
#include "functions.h"

extern semanage_handle_t *sh;

// TODO: current policies does not have any nodecon rules
#define NODE_COUNT 0

int node_test_init(void) {
    return 0;
}

int node_test_cleanup(void) {
    return 0;
}

int node_add_tests(CU_pSuite suite) {
    CU_add_test(suite, "node_compare", test_node_compare);
    CU_add_test(suite, "node_compare2", test_node_compare2);
    CU_add_test(suite, "node_key_create", test_node_key_create);
    CU_add_test(suite, "node_key_extract", test_node_key_extract);
    CU_add_test(suite, "node_get_set_addr", test_node_get_set_addr);
    CU_add_test(suite, "node_get_set_addr_bytes", test_node_get_set_addr_bytes);
    CU_add_test(suite, "node_get_set_mask", test_node_get_set_mask);
    CU_add_test(suite, "node_get_set_mask_bytes", test_node_get_set_mask_bytes);
    CU_add_test(suite, "node_get_set_proto", test_node_get_set_proto);
    CU_add_test(suite, "node_get_proto_str", test_node_get_proto_str);
    CU_add_test(suite, "node_get_set_con", test_node_get_set_con);
    CU_add_test(suite, "node_create", test_node_create);
    CU_add_test(suite, "node_clone", test_node_clone);

    CU_add_test(suite, "node_query", test_node_query);
    CU_add_test(suite, "node_exists", test_node_exists);
    CU_add_test(suite, "node_count", test_node_count);
    CU_add_test(suite, "node_iterate", test_node_iterate);
    CU_add_test(suite, "node_list", test_node_list);

    CU_add_test(suite, "node_modify_del_query_local",
                test_node_modify_del_query_local);
    CU_add_test(suite, "node_exists_local", test_node_exists_local);
    CU_add_test(suite, "node_count_local", test_node_count_local);
    CU_add_test(suite, "node_iterate_local", test_node_iterate_local);
    CU_add_test(suite, "node_list_local", test_node_list_local);

    return 0;
}

// Function semanage_node_compare

void test_node_compare(void) {
    semanage_node_t *node = NULL;
    semanage_node_key_t *key1 = NULL;
    semanage_node_key_t *key2 = NULL;
    int result = 42;

    // setup
    setup_handle(SH_CONNECT);

    node = get_node();

    key1 = get_node_key();

    CU_ASSERT(semanage_node_key_create(sh, "192.168.0.1", "255.255.0.0",
                                       SEMANAGE_PROTO_IP4, &key2) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key2);

    // test
    result = semanage_node_compare(node, key1);

    CU_ASSERT(result == 0);

    result = semanage_node_compare(node, key2);

    CU_ASSERT(result != 0);

    // cleanup
    semanage_node_free(node);
    semanage_node_key_free(key1);
    semanage_node_key_free(key2);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_compare2

void test_node_compare2(void) {
    semanage_node_t *node1 = NULL;
    semanage_node_t *node2 = NULL;
    semanage_node_t *node3 = NULL;
    int result = 42;

    // setup
    setup_handle(SH_CONNECT);

    node1 = get_node();
    node2 = get_node();

    node3 = get_node();
    CU_ASSERT(semanage_node_set_addr(sh, node3, SEMANAGE_PROTO_IP4,
                                     "192.168.0.1") >= 0);

    // test
    result = semanage_node_compare2(node1, node2);

    CU_ASSERT(result == 0);

    result = semanage_node_compare2(node1, node3);

    CU_ASSERT(result != 0);

    // cleanup
    semanage_node_free(node1);
    semanage_node_free(node2);
    semanage_node_free(node3);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_key_create

void test_node_key_create(void) {
    semanage_node_key_t *key = NULL;

    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_node_key_create(sh, "127.0.0.1", "255.255.255.255",
                                       SEMANAGE_PROTO_IP4, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_node_key_free(key);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_key_extract

void test_node_key_extract(void) {
    semanage_node_t *node = NULL;
    semanage_node_key_t *key = NULL;

    // setup
    setup_handle(SH_CONNECT);

    node = get_node();

    // test
    CU_ASSERT(semanage_node_key_extract(sh, node, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    // cleanup
    semanage_node_free(node);
    semanage_node_key_free(key);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_get_addr, semanage_node_set_addr

void test_node_get_set_addr(void) {
    semanage_node_t *node = NULL;
    char *addr = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_node_create(sh, &node) >= 0);

    // test
    CU_ASSERT(semanage_node_set_addr(sh, node, SEMANAGE_PROTO_IP4,
                                     "192.168.0.1") == 0);

    CU_ASSERT(semanage_node_get_addr(sh, node, &addr) >= 0);
    CU_ASSERT_PTR_NOT_NULL(addr);
    CU_ASSERT_STRING_EQUAL(addr, "192.168.0.1");

    // cleanup
    semanage_node_free(node);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_get_addr_bytes, semanage_node_set_addr_bytes

void test_node_get_set_addr_bytes(void) {
    semanage_node_t *node = NULL;
    char addr1[] = { 192, 168, 0, 1 };
    size_t addr1_size = sizeof(addr1);
    char *addr2 = NULL;
    size_t addr2_size = 0;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_node_create(sh, &node) >= 0);

    // test
    CU_ASSERT(semanage_node_set_addr_bytes(sh, node, addr1, addr1_size) == 0);

    CU_ASSERT(semanage_node_get_addr_bytes(sh, node, &addr2, &addr2_size) >= 0);
    CU_ASSERT_PTR_NOT_NULL(addr2);

    for (size_t i = 0; i < addr2_size; i++) {
        CU_ASSERT(addr1[i] == addr2[i]);
    }

    // cleanup
    semanage_node_free(node);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_get_mask, semanage_node_set_mask

void test_node_get_set_mask(void) {
    semanage_node_t *node = NULL;
    char *mask = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_node_create(sh, &node) >= 0);

    // test
    CU_ASSERT(semanage_node_set_mask(sh, node, SEMANAGE_PROTO_IP4,
                                     "255.255.255.0") == 0);

    CU_ASSERT(semanage_node_get_mask(sh, node, &mask) >= 0);
    CU_ASSERT_PTR_NOT_NULL(mask);
    CU_ASSERT_STRING_EQUAL(mask, "255.255.255.0");

    // cleanup
    semanage_node_free(node);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_get_mask_bytes, semanage_node_set_mask_bytes

void test_node_get_set_mask_bytes(void) {
    semanage_node_t *node = NULL;
    char mask1[] = { 255, 255, 255, 0 };
    size_t mask1_size = sizeof(mask1);
    char *mask2 = NULL;
    size_t mask2_size = 0;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_node_create(sh, &node) >= 0);

    // test
    CU_ASSERT(semanage_node_set_mask_bytes(sh, node, mask1, mask1_size) == 0);

    CU_ASSERT(semanage_node_get_mask_bytes(sh, node, &mask2, &mask2_size) >= 0);
    CU_ASSERT_PTR_NOT_NULL(mask2);

    for (size_t i = 0; i < mask2_size; i++) {
        CU_ASSERT(mask1[i] == mask2[i]);
    }

    // cleanup
    semanage_node_free(node);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_get_proto, semanage_node_set_proto

void test_node_get_set_proto(void) {
    semanage_node_t *node = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_node_create(sh, &node) >= 0);

    // test
    semanage_node_set_proto(node, SEMANAGE_PROTO_IP4);

    CU_ASSERT(semanage_node_get_proto(node) == SEMANAGE_PROTO_IP4);

    // cleanup
    semanage_node_free(node);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_get_proto_str

void test_node_get_proto_str(void) {
    CU_ASSERT_STRING_EQUAL(semanage_node_get_proto_str(SEMANAGE_PROTO_IP4),
                           "ipv4");
    CU_ASSERT_STRING_EQUAL(semanage_node_get_proto_str(SEMANAGE_PROTO_IP6),
                           "ipv6");
}

// Function semanage_node_get_con, semanage_node_set_con

void test_node_get_set_con(void) {
    semanage_node_t *node = NULL;
    semanage_context_t *con1 = NULL;
    semanage_context_t *con2 = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_node_create(sh, &node) >= 0);

    CU_ASSERT(semanage_context_from_string(sh,
                               "my_user_u:my_role_r:my_type_t:s0", &con1) >= 0);

    // test
    CU_ASSERT(semanage_node_set_con(sh, node, con1) == 0);

    con2 = semanage_node_get_con(node);

    CU_ASSERT_CONTEXT_EQUAL(con1, con2);

    // cleanup
    semanage_node_free(node);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_create

void test_node_create(void) {
    semanage_node_t *node = NULL;
    semanage_context_t *con = NULL;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_node_create(sh, &node) >= 0);

    CU_ASSERT(semanage_node_set_addr(sh, node, SEMANAGE_PROTO_IP4,
                                     "127.0.0.1") >= 0);

    CU_ASSERT(semanage_node_set_mask(sh, node, SEMANAGE_PROTO_IP4,
                                     "255.255.255.0") >= 0);

    semanage_node_set_proto(node, SEMANAGE_PROTO_IP4);

    CU_ASSERT(semanage_context_from_string(sh, "user_u:role_r:type_t:s0",
                                           &con) >= 0);
    CU_ASSERT(semanage_node_set_con(sh, node, con) >= 0);

    // cleanup
    semanage_node_free(node);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_clone

void test_node_clone(void) {
    semanage_node_t *node = NULL;
    semanage_node_t *node_clone = NULL;
    semanage_context_t *con = NULL;
    semanage_context_t *con2 = NULL;
    const char *addr1 = "127.0.0.1";
    char *addr2 = NULL;
    const char *mask1 = "255.255.255.0";
    char *mask2 = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_node_create(sh, &node) >= 0);

    CU_ASSERT(semanage_node_set_addr(sh, node, SEMANAGE_PROTO_IP4,
                                     addr1) >= 0);

    CU_ASSERT(semanage_node_set_mask(sh, node, SEMANAGE_PROTO_IP4,
                                     mask1) >= 0);

    semanage_node_set_proto(node, SEMANAGE_PROTO_IP4);

    CU_ASSERT(semanage_context_from_string(sh, "user_u:role_r:type_t:s0",
                                           &con) >= 0);
    CU_ASSERT(semanage_node_set_con(sh, node, con) >= 0);

    // test
    CU_ASSERT(semanage_node_clone(sh, node, &node_clone) >= 0);

    CU_ASSERT(semanage_node_get_addr(sh, node_clone, &addr2) >= 0);
    CU_ASSERT_PTR_NOT_NULL(addr2);
    CU_ASSERT_STRING_EQUAL(addr1, addr2);

    CU_ASSERT(semanage_node_get_mask(sh, node_clone, &mask2) >= 0);
    CU_ASSERT_PTR_NOT_NULL(mask2);
    CU_ASSERT_STRING_EQUAL(mask1, mask2);

    CU_ASSERT(semanage_node_get_proto(node_clone) == SEMANAGE_PROTO_IP4);

    con2 = semanage_node_get_con(node_clone);
    
    CU_ASSERT_CONTEXT_EQUAL(con, con2);

    // cleanup
    semanage_node_free(node);
    semanage_node_free(node_clone);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_query

void test_node_query(void) {
    semanage_node_t *node = NULL;
    semanage_node_key_t *key = NULL;

    // setup
    setup_handle(SH_CONNECT);

    key = get_node_key();

    // test
    CU_ASSERT(semanage_node_query(sh, key, &node) >= 0);

    CU_ASSERT(node == NULL);

    // cleanup
    semanage_node_free(node);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_exists

void test_node_exists(void) {
    semanage_node_key_t *key = NULL;
    int response = 42;

    // setup
    setup_handle(SH_CONNECT);

    key = get_node_key();

    // test
    CU_ASSERT(semanage_node_exists(sh, key, &response) >= 0);

    CU_ASSERT(!response);

    // cleanup
    semanage_node_key_free(key);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_count

void test_node_count(void) {
    unsigned int count = 42;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_node_count(sh, &count) >= 0);

    CU_ASSERT(count == NODE_COUNT);

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_iterate

unsigned int counter_node_iterate = 0;

int handler_node_iterate(const semanage_node_t *record, void *varg) {
    counter_node_iterate++;
    return 0;
}

void test_node_iterate(void) {
    // setup
    setup_handle(SH_CONNECT);

    // test
    semanage_node_iterate(sh, handler_node_iterate, NULL);

    CU_ASSERT(counter_node_iterate == NODE_COUNT);

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_list

void test_node_list(void) {
    semanage_node_t **records = NULL;
    unsigned int count = 42;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_node_list(sh, &records, &count) >= 0);

    CU_ASSERT(count == NODE_COUNT);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    for (unsigned int i = 0; i < count; i++) {
        semanage_node_free(records[i]);
    }

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_node_modify_local, semanage_node_del_local,
// semanage_node_query_local

void test_node_modify_del_query_local(void) {
    semanage_node_t *node;
    semanage_node_t *node_local;
    semanage_node_t *node_tmp;
    semanage_node_key_t *key = NULL;
    semanage_node_key_t *key_tmp = NULL;

    // setup
    setup_handle(SH_TRANS);

    node = get_node();

    CU_ASSERT(semanage_node_key_extract(sh, node, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    // add second record, so that semanage_node_compare2_qsort will be called
    node_tmp = get_node();

    CU_ASSERT(semanage_node_set_addr(sh, node_tmp, SEMANAGE_PROTO_IP4,
                                     "10.0.0.1") >= 0);

    CU_ASSERT(semanage_node_key_extract(sh, node_tmp, &key_tmp) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key_tmp);

    // test
    CU_ASSERT(semanage_node_modify_local(sh, key, node) >= 0);
    CU_ASSERT(semanage_node_modify_local(sh, key_tmp, node_tmp) >= 0);

    // write changes to file
    commit();
    begin_transaction();

    CU_ASSERT(semanage_node_query_local(sh, key, &node_local) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(node_local);

    CU_ASSERT(semanage_node_del_local(sh, key) >= 0);
    CU_ASSERT(semanage_node_del_local(sh, key_tmp) >= 0);

    CU_ASSERT(semanage_node_query_local(sh, key, &node_local) < 0);

    // cleanup
    semanage_node_free(node);
    semanage_node_free(node_tmp);
    cleanup_handle(SH_TRANS);
}

// Function semanage_node_exists_local

void test_node_exists_local(void) {
    semanage_node_t *node = NULL;
    semanage_node_key_t *key1 = NULL;
    semanage_node_key_t *key2 = NULL;
    semanage_context_t *con = NULL;
    int response = 42;

    // setup
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_node_create(sh, &node) >= 0);

    CU_ASSERT(semanage_node_set_addr(sh, node, SEMANAGE_PROTO_IP4,
                                     "127.0.0.1") >= 0);

    CU_ASSERT(semanage_node_set_mask(sh, node, SEMANAGE_PROTO_IP4,
                                     "255.255.255.0") >= 0);

    semanage_node_set_proto(node, SEMANAGE_PROTO_IP4);

    CU_ASSERT(semanage_context_from_string(sh, "user_u:role_r:type_t:s0",
                                           &con) >= 0);
    CU_ASSERT(semanage_node_set_con(sh, node, con) >= 0);

    CU_ASSERT(semanage_node_key_extract(sh, node, &key1) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key1);

    CU_ASSERT(semanage_node_modify_local(sh, key1, node) >= 0);

    CU_ASSERT(semanage_node_key_create(sh, "192.168.0.1", "255.255.0.0",
                                       SEMANAGE_PROTO_IP4, &key2) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key2);

    // test existing
    CU_ASSERT(semanage_node_exists_local(sh, key1, &response) >= 0);

    CU_ASSERT(response);

    // test nonexistent
    CU_ASSERT(semanage_node_exists_local(sh, key2, &response) >= 0);

    CU_ASSERT(!response);

    // cleanup
    CU_ASSERT(semanage_node_del_local(sh, key1) >= 0);
    semanage_node_free(node);
    semanage_node_key_free(key1);
    semanage_node_key_free(key2);
    cleanup_handle(SH_TRANS);
}

// Function semanage_node_count_local

void test_node_count_local(void) {
    unsigned int count = 42;

    // setup
    setup_handle(SH_TRANS);

    add_local_node(I_FIRST);
    add_local_node(I_SECOND);
    add_local_node(I_THIRD);

    // test
    CU_ASSERT(semanage_node_count_local(sh, &count) >= 0);

    CU_ASSERT(count == 3);

    // cleanup
    delete_local_node(I_FIRST);
    delete_local_node(I_SECOND);
    delete_local_node(I_THIRD);

    cleanup_handle(SH_TRANS);
}

// Function semanage_node_iterate_local

unsigned int counter_node_iterate_local = 0;

int handler_node_iterate_local(const semanage_node_t *record, void *varg) {
    counter_node_iterate_local++;
    return 0;
}

void test_node_iterate_local(void) {
    // setup
    setup_handle(SH_TRANS);

    add_local_node(I_FIRST);
    add_local_node(I_SECOND);
    add_local_node(I_THIRD);

    // test
    semanage_node_iterate_local(sh, handler_node_iterate_local, NULL);

    CU_ASSERT(counter_node_iterate_local == 3);

    // cleanup
    delete_local_node(I_FIRST);
    delete_local_node(I_SECOND);
    delete_local_node(I_THIRD);

    cleanup_handle(SH_TRANS);
}

// Function semanage_node_list_local

void test_node_list_local(void) {
    semanage_node_t **records = NULL;
    unsigned int count = 42;

    // setup
    setup_handle(SH_TRANS);

    add_local_node(I_FIRST);
    add_local_node(I_SECOND);
    add_local_node(I_THIRD);

    // test
    CU_ASSERT(semanage_node_list_local(sh, &records, &count) >= 0);

    CU_ASSERT(count == 3);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    // cleanup
    for (unsigned int i = 0; i < count; i++) {
        semanage_node_free(records[i]);
    }

    delete_local_node(I_FIRST);
    delete_local_node(I_SECOND);
    delete_local_node(I_THIRD);

    cleanup_handle(SH_TRANS);
}
