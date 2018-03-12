#include <stdio.h>
#include <semanage/semanage.h>
#include <semanage/handle.h>
#include <CUnit/Basic.h>

#include "test_iface.h"
#include "functions.h"

extern semanage_handle_t *sh;

// TODO: current policies does not have any netifcon rules
#define IFACE_COUNT 0

int iface_test_init(void) {
	return 0;
}

int iface_test_cleanup(void) {
	return 0;
}

int iface_add_tests(CU_pSuite suite) {
    CU_add_test(suite, "iface_compare", test_iface_compare);
    CU_add_test(suite, "iface_compare2", test_iface_compare2);
    CU_add_test(suite, "iface_key_create", test_iface_key_create);
    CU_add_test(suite, "iface_key_extract", test_iface_key_extract);
    CU_add_test(suite, "iface_get_set_name", test_iface_get_set_name);
    CU_add_test(suite, "iface_get_set_ifcon", test_iface_get_set_ifcon);
    CU_add_test(suite, "iface_get_set_msgcon", test_iface_get_set_msgcon);
    CU_add_test(suite, "iface_create)", test_iface_create);
    CU_add_test(suite, "iface_clone);", test_iface_clone);

    CU_add_test(suite, "iface_query", test_iface_query);
    CU_add_test(suite, "iface_exists", test_iface_exists);
    CU_add_test(suite, "iface_count", test_iface_count);
    CU_add_test(suite, "iface_iterate", test_iface_iterate);
    CU_add_test(suite, "iface_list", test_iface_list);

    CU_add_test(suite, "iface_modify_del_query_local",
                test_iface_modify_del_query_local);
    CU_add_test(suite, "iface_exists_local", test_iface_exists_local);
    CU_add_test(suite, "iface_count_local", test_iface_count_local);
    CU_add_test(suite, "iface_iterate_local", test_iface_iterate_local);
    CU_add_test(suite, "iface_list_local", test_iface_list_local);

	return 0;
}

// Function semanage_iface_compare

void test_iface_compare(void) {
    semanage_iface_t *iface = NULL;
    semanage_iface_key_t *key1 = NULL;
    semanage_iface_key_t *key2 = NULL;
    int result = 42;

    // setup
    setup_handle(SH_CONNECT);

    iface = get_iface();

    key1 = get_iface_key();

    CU_ASSERT(semanage_iface_key_create(sh, "qwerty", &key2) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key2);

    // test
    result = semanage_iface_compare(iface, key1);

    CU_ASSERT(result == 0);

    result = semanage_iface_compare(iface, key2);

    CU_ASSERT(result != 0);

    // cleanup
    semanage_iface_free(iface);
    semanage_iface_key_free(key1);
    semanage_iface_key_free(key2);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_compare2

void test_iface_compare2(void) {
    semanage_iface_t *iface1 = NULL;
    semanage_iface_t *iface2 = NULL;
    semanage_iface_t *iface3 = NULL;
    int result = 42;

    // setup
    setup_handle(SH_CONNECT);

    iface1 = get_iface();
    iface2 = get_iface();

    iface3 = get_iface();
    CU_ASSERT(semanage_iface_set_name(sh, iface3, "qwerty") >= 0);

    // test
    result = semanage_iface_compare2(iface1, iface2);

    CU_ASSERT(result == 0);

    result = semanage_iface_compare2(iface1, iface3);

    CU_ASSERT(result != 0);

    // cleanup
    semanage_iface_free(iface1);
    semanage_iface_free(iface2);
    semanage_iface_free(iface3);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_create

void test_iface_key_create(void) {
    semanage_iface_key_t *key = NULL;

    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_iface_key_create(sh, "asdf", &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_iface_key_free(key);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_extract

void test_iface_key_extract(void) {
    semanage_iface_t *iface = NULL;
    semanage_iface_key_t *key = NULL;

    // setup
    setup_handle(SH_CONNECT);

    iface = get_iface();

    // test
    CU_ASSERT(semanage_iface_key_extract(sh, iface, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    // cleanup
    semanage_iface_free(iface);
    semanage_iface_key_free(key);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_get_name, semanage_iface_set_name

void test_iface_get_set_name(void) {
    semanage_iface_t *iface = NULL;

    // setup
    setup_handle(SH_CONNECT);

    iface = get_iface();

    // test
    CU_ASSERT(semanage_iface_set_name(sh, iface, "my_asdf") == 0);

    CU_ASSERT_STRING_EQUAL(semanage_iface_get_name(iface), "my_asdf");

    // cleanup
    semanage_iface_free(iface);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_get_ifcon, semanage_iface_set_ifcon

void test_iface_get_set_ifcon(void) {
    semanage_iface_t *iface = NULL;
    semanage_context_t *con1 = NULL;
    semanage_context_t *con2 = NULL;

    // setup
    setup_handle(SH_CONNECT);

    iface = get_iface();

    CU_ASSERT(semanage_context_from_string(sh,
                               "my_user_u:my_role_r:my_type_t:s0", &con1) >= 0);

    // test
    CU_ASSERT(semanage_iface_set_ifcon(sh, iface, con1) == 0);

    con2 = semanage_iface_get_ifcon(iface);

    CU_ASSERT_CONTEXT_EQUAL(con1, con2);

    // cleanup
    semanage_iface_free(iface);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_get_msgcon, semanage_iface_set_msgcon

void test_iface_get_set_msgcon(void) {
    semanage_iface_t *iface = NULL;
    semanage_context_t *con1 = NULL;
    semanage_context_t *con2 = NULL;

    // setup
    setup_handle(SH_CONNECT);

    iface = get_iface();

    CU_ASSERT(semanage_context_from_string(sh,
                              "my_user_u:my_role_r:my_type_t:s0", &con1) >= 0);

    // test
    CU_ASSERT(semanage_iface_set_msgcon(sh, iface, con1) == 0);

    con2 = semanage_iface_get_msgcon(iface);

    CU_ASSERT_CONTEXT_EQUAL(con1, con2);

    // cleanup
    semanage_iface_free(iface);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_create

void test_iface_create(void) {
    semanage_iface_t *iface = NULL;
    semanage_context_t *ifcon = NULL;
    semanage_context_t *msgcon = NULL;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_iface_create(sh, &iface) >= 0);

    CU_ASSERT(semanage_iface_set_name(sh, iface, "asdf") >= 0);

    CU_ASSERT(semanage_context_from_string(sh, "user_u:role_r:type_t:s0",
                                           &ifcon) >= 0);
    CU_ASSERT(semanage_iface_set_ifcon(sh, iface, ifcon) >= 0);

    CU_ASSERT(semanage_context_from_string(sh, "user_u:role_r:type_t:s0",
                                           &msgcon) >= 0);
    CU_ASSERT(semanage_iface_set_msgcon(sh, iface, msgcon) >= 0);

    // cleanup
    semanage_iface_free(iface);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_clone

void test_iface_clone(void) {
    semanage_iface_t *iface = NULL;
    semanage_iface_t *iface_clone = NULL;
    semanage_context_t *ifcon = NULL;
    semanage_context_t *ifcon2 = NULL;
    semanage_context_t *msgcon = NULL;
    semanage_context_t *msgcon2 = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_iface_create(sh, &iface) >= 0);

    CU_ASSERT(semanage_iface_set_name(sh, iface, "asdf") >= 0);

    CU_ASSERT(semanage_context_from_string(sh, "user_u:role_r:if_type_t:s0",
                                           &ifcon) >= 0);
    CU_ASSERT(semanage_iface_set_ifcon(sh, iface, ifcon) >= 0);

    CU_ASSERT(semanage_context_from_string(sh, "user_u:role_r:msg_type_t:s0",
                                           &msgcon) >= 0);
    CU_ASSERT(semanage_iface_set_msgcon(sh, iface, msgcon) >= 0);

    // test
    CU_ASSERT(semanage_iface_clone(sh, iface, &iface_clone) >= 0);

    CU_ASSERT_STRING_EQUAL(semanage_iface_get_name(iface_clone), "asdf");

    ifcon2 = semanage_iface_get_ifcon(iface_clone);
    
    CU_ASSERT_CONTEXT_EQUAL(ifcon, ifcon2);

    msgcon2 = semanage_iface_get_msgcon(iface_clone);

    CU_ASSERT_CONTEXT_EQUAL(msgcon, msgcon2);

    // cleanup
    semanage_iface_free(iface);
    semanage_iface_free(iface_clone);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_query

void test_iface_query(void) {
    semanage_iface_t *iface = NULL;
    semanage_iface_key_t *key = NULL;

    // setup
    setup_handle(SH_CONNECT);

    key = get_iface_key();

    // test
    CU_ASSERT(semanage_iface_query(sh, key, &iface) >= 0);

    CU_ASSERT(iface == NULL);

    // cleanup
    semanage_iface_free(iface);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_exists

void test_iface_exists(void) {
    semanage_iface_key_t *key = NULL;
    int response = 42;

    // setup
    setup_handle(SH_CONNECT);

    key = get_iface_key();

    // test
    CU_ASSERT(semanage_iface_exists(sh, key, &response) >= 0);

    CU_ASSERT(!response);

    // cleanup
    semanage_iface_key_free(key);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_count

void test_iface_count(void) {
    unsigned int count = 42;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_iface_count(sh, &count) >= 0);

    CU_ASSERT(count == IFACE_COUNT);

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_iterate

unsigned int counter_iface_iterate = 0;

int handler_iface_iterate(const semanage_iface_t *record, void *varg) {
    counter_iface_iterate++;
    return 0;
}

void test_iface_iterate(void) {
    // setup
    setup_handle(SH_CONNECT);

    // test
    semanage_iface_iterate(sh, handler_iface_iterate, NULL);

    CU_ASSERT(counter_iface_iterate == IFACE_COUNT);

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_list

void test_iface_list(void) {
    semanage_iface_t **records = NULL;
    unsigned int count = 42;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_iface_list(sh, &records, &count) >= 0);

    CU_ASSERT(count == IFACE_COUNT);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    for (unsigned int i = 0; i < count; i++) {
        semanage_iface_free(records[i]);
    }

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_iface_modify_local, semanage_iface_del_local,
// semanage_iface_query_local

void test_iface_modify_del_query_local(void) {
    semanage_iface_t *iface;
    semanage_iface_t *iface_local;
    semanage_iface_key_t *key = NULL;

    // setup
    setup_handle(SH_TRANS);

    iface = get_iface();

    CU_ASSERT(semanage_iface_key_extract(sh, iface, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    // test
    CU_ASSERT(semanage_iface_modify_local(sh, key, iface) >= 0);

    // write changes to file
    helper_commit();
    helper_begin_transaction();

    CU_ASSERT(semanage_iface_query_local(sh, key, &iface_local) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iface_local);

    CU_ASSERT(semanage_iface_del_local(sh, key) >= 0);

    CU_ASSERT(semanage_iface_query_local(sh, key, &iface_local) < 0);

    // cleanup
    semanage_iface_free(iface);
    cleanup_handle(SH_TRANS);
}

// Function semanage_iface_exists_local

void test_iface_exists_local(void) {
    semanage_iface_t *iface1 = NULL;
    semanage_iface_key_t *key1 = NULL;
    semanage_iface_key_t *key2 = NULL;
    int response = 42;

    // setup
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_iface_create(sh, &iface1) >= 0);
    CU_ASSERT(semanage_iface_set_name(sh, iface1, "asdf") >= 0);

    CU_ASSERT(semanage_iface_key_extract(sh, iface1, &key1) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key1);

    CU_ASSERT(semanage_iface_modify_local(sh, key1, iface1) >= 0);

    CU_ASSERT(semanage_iface_key_create(sh, "qwerty", &key2) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key2);

    // test existing
    CU_ASSERT(semanage_iface_exists_local(sh, key1, &response) >= 0);

    CU_ASSERT(response);

    // test nonexistent
    CU_ASSERT(semanage_iface_exists_local(sh, key2, &response) >= 0);

    CU_ASSERT(!response);

    // cleanup
    CU_ASSERT(semanage_iface_del_local(sh, key1) >= 0);
    semanage_iface_free(iface1);
    semanage_iface_key_free(key1);
    semanage_iface_key_free(key2);
    cleanup_handle(SH_TRANS);
}

// Function semanage_iface_count_local

void test_iface_count_local(void) {
    unsigned int count = 42;

    // setup
    setup_handle(SH_TRANS);

    add_local_iface(I_FIRST);
    add_local_iface(I_SECOND);
    add_local_iface(I_THIRD);

    // test
    CU_ASSERT(semanage_iface_count_local(sh, &count) >= 0);

    CU_ASSERT(count == 3);

    // cleanup
    delete_local_iface(I_FIRST);
    delete_local_iface(I_SECOND);
    delete_local_iface(I_THIRD);

    cleanup_handle(SH_TRANS);
}

// Function semanage_iface_iterate_local

unsigned int counter_iface_iterate_local = 0;

int handler_iface_iterate_local(const semanage_iface_t *record, void *varg) {
    counter_iface_iterate_local++;
    return 0;
}

void test_iface_iterate_local(void) {
    // setup
    setup_handle(SH_TRANS);

    add_local_iface(I_FIRST);
    add_local_iface(I_SECOND);
    add_local_iface(I_THIRD);

    // test
    semanage_iface_iterate_local(sh, handler_iface_iterate_local, NULL);

    CU_ASSERT(counter_iface_iterate_local == 3);

    // cleanup
    delete_local_iface(I_FIRST);
    delete_local_iface(I_SECOND);
    delete_local_iface(I_THIRD);

    cleanup_handle(SH_TRANS);
}

// Function semanage_iface_list_local

void test_iface_list_local(void) {
    semanage_iface_t **records = NULL;
    unsigned int count = 42;

    // setup
    setup_handle(SH_TRANS);

    add_local_iface(I_FIRST);
    add_local_iface(I_SECOND);
    add_local_iface(I_THIRD);

    // test
    CU_ASSERT(semanage_iface_list_local(sh, &records, &count) >= 0);

    CU_ASSERT(count == 3);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    // cleanup
    for (unsigned int i = 0; i < count; i++) {
        semanage_iface_free(records[i]);
    }

    delete_local_iface(I_FIRST);
    delete_local_iface(I_SECOND);
    delete_local_iface(I_THIRD);

    cleanup_handle(SH_TRANS);
}
