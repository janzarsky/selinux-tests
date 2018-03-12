#include <stdio.h>
#include <string.h>
#include <semanage/semanage.h>
#include <semanage/handle.h>
#include <CUnit/Basic.h>

#include "test_ibendport.h"
#include "test_ibendport_module.h"
#include "functions.h"

extern semanage_handle_t *sh;

int ibendport_test_init(void) {
#ifdef FEATURE_INFINIBAND

    semanage_handle_t *sh;

    sh = semanage_handle_create();

    if (semanage_connect(sh) < 0) {
        semanage_handle_destroy(sh);
        return -1;
    }

    if (semanage_begin_transaction(sh) < 0)
        goto err;

    if (semanage_module_install(sh, (char *) test_ibendport_module,
                                test_ibendport_module_len, "test_ibendport",
                                "cil") < 0) {
        semanage_disconnect(sh);
        semanage_handle_destroy(sh);
        return -1;
    }

    if (semanage_commit(sh) < 0) {
        semanage_disconnect(sh);
        semanage_handle_destroy(sh);
        return -1;
    }

    return 0;

err:
    semanage_disconnect(sh);
    semanage_handle_destroy(sh);
    return -1;

#else

    return 0;

#endif
}

int ibendport_test_cleanup(void) {
#ifdef FEATURE_INFINIBAND

    semanage_handle_t *sh;

    sh = semanage_handle_create();

    if (semanage_connect(sh) < 0)
        return -1;

    if (semanage_begin_transaction(sh) < 0) {
        semanage_disconnect(sh);
        semanage_handle_destroy(sh);
        return -1;
    }

    if (semanage_module_remove(sh, "test_ibendport") < 0) {
        semanage_disconnect(sh);
        semanage_handle_destroy(sh);
        return -1;
    }

    if (semanage_commit(sh) < 0) {
        semanage_disconnect(sh);
        semanage_handle_destroy(sh);
        return -1;
    }

    semanage_disconnect(sh);
    semanage_handle_destroy(sh);

#endif

    return 0;
}

int ibendport_add_tests(CU_pSuite suite) {
#ifdef FEATURE_INFINIBAND
    CU_add_test(suite, "ibendport_query", test_ibendport_query);
    CU_add_test(suite, "ibendport_exists", test_ibendport_exists);
    CU_add_test(suite, "ibendport_count", test_ibendport_count);
    CU_add_test(suite, "ibendport_iterate", test_ibendport_iterate);
    CU_add_test(suite, "ibendport_list", test_ibendport_list);

    CU_add_test(suite, "ibendport_modify_del_query_local",
                test_ibendport_modify_del_query_local);
    CU_add_test(suite, "ibendport_exists_local", test_ibendport_exists_local);
    CU_add_test(suite, "ibendport_count_local", test_ibendport_count_local);
    CU_add_test(suite, "ibendport_iterate_local", test_ibendport_iterate_local);
    CU_add_test(suite, "ibendport_list_local", test_ibendport_list_local);
#endif

    return 0;
}

#ifdef FEATURE_INFINIBAND

// Function semanage_ibendport_query

void test_ibendport_query(void) {
    semanage_ibendport_key_t *key1 = NULL;
    semanage_ibendport_key_t *key2 = NULL;
    semanage_ibendport_t *response1 = NULL;
    semanage_ibendport_t *response2 = NULL;
    char *name = NULL;
    semanage_context_t *con = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_ibendport_key_create(sh, IBENDPORT_INV_NAME,
                                            IBENDPORT_INV_PORT, &key1) >= 0);

    CU_ASSERT(semanage_ibendport_key_create(sh, IBENDPORT_1_NAME,
                                            IBENDPORT_1_PORT, &key2) >= 0);

    // test nonexisting ibendport
    CU_ASSERT(semanage_ibendport_query(sh, key1, &response1) >= 0);
    CU_ASSERT(response1 == NULL);

    // test existing ibendport
    CU_ASSERT(semanage_ibendport_query(sh, key2, &response2) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(response2);

    CU_ASSERT(semanage_ibendport_get_ibdev_name(sh, response2, &name) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(name);
    CU_ASSERT_STRING_EQUAL(name, IBENDPORT_1_NAME);

    CU_ASSERT(semanage_ibendport_get_port(response2) == IBENDPORT_1_PORT);

    con = semanage_ibendport_get_con(response2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(con);
    CU_ASSERT(context_compare(con, IBENDPORT_1_CON) == 0);

    // cleanup
    free(name);
    semanage_ibendport_key_free(key1);
    semanage_ibendport_key_free(key2);
    semanage_ibendport_free(response1);
    semanage_ibendport_free(response2);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_ibendport_exists

void test_ibendport_exists(void) {
    semanage_ibendport_key_t *key1 = NULL;
    semanage_ibendport_key_t *key2 = NULL;
    int response = 42;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_ibendport_key_create(sh, IBENDPORT_INV_NAME,
                                            IBENDPORT_INV_PORT, &key1) >= 0);

    CU_ASSERT(semanage_ibendport_key_create(sh, IBENDPORT_1_NAME,
                                            IBENDPORT_1_PORT, &key2) >= 0);

    // test nonexisting ibendport
    CU_ASSERT(semanage_ibendport_exists(sh, key1, &response) >= 0);
    CU_ASSERT(!response);

    // test existing ibendport
    CU_ASSERT(semanage_ibendport_exists(sh, key2, &response) >= 0);
    CU_ASSERT(response);

    // cleanup
    semanage_ibendport_key_free(key1);
    semanage_ibendport_key_free(key2);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_ibendport_count

void test_ibendport_count(void) {
    unsigned int count = 42;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_ibendport_count(sh, &count) >= 0);

    CU_ASSERT(count == IBENDPORT_COUNT);

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_ibendport_iterate

unsigned int helper_ibendport_iterate_counter = 0;

int helper_ibendport_iterate(const semanage_ibendport_t *ibendport,
                             void *fn_arg) {
    CU_ASSERT(fn_arg == (void *) 42);

    helper_ibendport_iterate_counter++;

    return 0;
}

int helper_ibendport_iterate_error(const semanage_ibendport_t *ibendport,
                                   void *fn_arg) {
    CU_ASSERT(fn_arg == (void *) 42);

    helper_ibendport_iterate_counter++;

    return -1;
}

int helper_ibendport_iterate_break(const semanage_ibendport_t *ibendport,
                                   void *fn_arg) {
    CU_ASSERT(fn_arg == (void *) 42);

    helper_ibendport_iterate_counter++;

    return 1;
}

void test_ibendport_iterate(void) {
    // setup
    setup_handle(SH_CONNECT);

    // test
    helper_ibendport_iterate_counter = 0;

    CU_ASSERT(semanage_ibendport_iterate(sh, helper_ibendport_iterate,
                                         (void *) 42) >= 0);
    CU_ASSERT(helper_ibendport_iterate_counter == IBENDPORT_COUNT);

    // test function which returns error
    helper_ibendport_iterate_counter = 0;

    CU_ASSERT(semanage_ibendport_iterate(sh, helper_ibendport_iterate_error,
                                         (void *) 42) < 0);
    CU_ASSERT(helper_ibendport_iterate_counter == 1);

    // test function which requests break
    helper_ibendport_iterate_counter = 0;

    CU_ASSERT(semanage_ibendport_iterate(sh, helper_ibendport_iterate_break,
                                         (void *) 42) >= 0);
    CU_ASSERT(helper_ibendport_iterate_counter == 1);

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_ibendport_list

void test_ibendport_list(void) {
    semanage_ibendport_t **records = NULL;
    unsigned int count = 42;
    char *name = NULL;
    semanage_context_t *con = NULL;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_ibendport_list(sh, &records, &count) >= 0);

    CU_ASSERT_PTR_NOT_NULL_FATAL(records);
    CU_ASSERT(count == IBENDPORT_COUNT);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL_FATAL(records[i]);

        CU_ASSERT(semanage_ibendport_get_ibdev_name(sh, records[i],
                                                    &name) >= 0);

        con = semanage_ibendport_get_con(records[i]);
        CU_ASSERT_PTR_NOT_NULL_FATAL(con);

        free(name);
    }

    // cleanup
    for (unsigned int i = 0; i < count; i++) {
        semanage_ibendport_free(records[i]);
    }

    free(records);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_ibendport_modify_local, semanage_ibendport_del_local,
// semanage_ibendport_query_local

void test_ibendport_modify_del_query_local(void) {
    semanage_ibendport_t *ibendport;
    semanage_ibendport_t *ibendport_local;
    semanage_ibendport_t *ibendport_tmp;
    semanage_ibendport_key_t *key = NULL;
    semanage_ibendport_key_t *key_tmp = NULL;
    semanage_context_t *con = NULL;
    semanage_context_t *con_tmp = NULL;

    // setup
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_ibendport_key_create(sh, "asdf", 1, &key) >= 0);

    CU_ASSERT(semanage_ibendport_create(sh, &ibendport) >= 0);
    CU_ASSERT(semanage_ibendport_set_ibdev_name(sh, ibendport, "asdf") >= 0);
    semanage_ibendport_set_port(ibendport, 1);
    CU_ASSERT(semanage_context_from_string(sh, "system_u:object_r:bin_t:s0",
                                           &con) >= 0);
    CU_ASSERT(semanage_ibendport_set_con(sh, ibendport, con) >= 0);

    // add second record, so that semanage_ibendport_compare2_qsort will be
    // called
    CU_ASSERT(semanage_ibendport_key_create(sh, "qwerty", 2, &key_tmp) >= 0);

    CU_ASSERT(semanage_ibendport_create(sh, &ibendport_tmp) >= 0);
    CU_ASSERT(semanage_ibendport_set_ibdev_name(sh, ibendport_tmp,
                                                "qwerty") >= 0);
    semanage_ibendport_set_port(ibendport_tmp, 2);
    CU_ASSERT(semanage_context_from_string(sh, "system_u:object_r:bin_t:s0",
                                           &con_tmp) >= 0);
    CU_ASSERT(semanage_ibendport_set_con(sh, ibendport_tmp, con_tmp) >= 0);

    // test
    CU_ASSERT(semanage_ibendport_modify_local(sh, key, ibendport) >= 0);
    CU_ASSERT(semanage_ibendport_modify_local(sh, key_tmp, ibendport_tmp) >= 0);

    // write changes to file
    commit();
    begin_transaction();

    CU_ASSERT(semanage_ibendport_query_local(sh, key, &ibendport_local) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(ibendport_local);

    CU_ASSERT(semanage_ibendport_del_local(sh, key) >= 0);
    CU_ASSERT(semanage_ibendport_del_local(sh, key_tmp) >= 0);

    CU_ASSERT(semanage_ibendport_query_local(sh, key, &ibendport_local) < 0);

    // cleanup
    semanage_context_free(con);
    semanage_context_free(con_tmp);
    semanage_ibendport_key_free(key);
    semanage_ibendport_key_free(key_tmp);
    semanage_ibendport_free(ibendport);
    semanage_ibendport_free(ibendport_local);
    semanage_ibendport_free(ibendport_tmp);
    cleanup_handle(SH_TRANS);
}

// Function semanage_ibendport_exists_local

void test_ibendport_exists_local(void) {
    semanage_ibendport_key_t *key1 = NULL;
    semanage_ibendport_key_t *key2 = NULL;
    int response = 42;

    // setup
    setup_handle(SH_TRANS);

    add_local_ibendport(I_FIRST);

    CU_ASSERT(semanage_ibendport_key_create(sh, IBENDPORT_INV_NAME,
                                            IBENDPORT_INV_PORT, &key1) >= 0);

    CU_ASSERT(semanage_ibendport_key_create(sh, "ibendport0", 0, &key2) >= 0);

    // test nonexisting ibendport
    CU_ASSERT(semanage_ibendport_exists_local(sh, key1, &response) >= 0);
    CU_ASSERT(!response);

    // test existing ibendport
    CU_ASSERT(semanage_ibendport_exists_local(sh, key2, &response) >= 0);
    CU_ASSERT(response);

    // cleanup
    
    delete_local_ibendport(I_FIRST);

    semanage_ibendport_key_free(key1);
    semanage_ibendport_key_free(key2);
    cleanup_handle(SH_TRANS);
}

// Function semanage_ibendport_count_local

void test_ibendport_count_local(void) {
    unsigned int count = 42;

    // setup
    setup_handle(SH_TRANS);

    add_local_ibendport(I_FIRST);
    add_local_ibendport(I_SECOND);
    add_local_ibendport(I_THIRD);

    // test
    CU_ASSERT(semanage_ibendport_count_local(sh, &count) >= 0);

    CU_ASSERT(count == 3);

    // cleanup
    delete_local_ibendport(I_FIRST);
    delete_local_ibendport(I_SECOND);
    delete_local_ibendport(I_THIRD);

    cleanup_handle(SH_TRANS);
}

// Function semanage_ibendport_iterate_local

unsigned int helper_ibendport_iterate_local_counter = 0;

int helper_ibendport_iterate_local(const semanage_ibendport_t *ibendport,
                                   void *fn_arg) {
    CU_ASSERT(fn_arg == (void *) 42);

    helper_ibendport_iterate_local_counter++;

    return 0;
}

int helper_ibendport_iterate_local_error(const semanage_ibendport_t *ibendport,
                                         void *fn_arg) {
    CU_ASSERT(fn_arg == (void *) 42);

    helper_ibendport_iterate_local_counter++;

    return -1;
}

int helper_ibendport_iterate_local_break(const semanage_ibendport_t *ibendport,
                                         void *fn_arg) {
    CU_ASSERT(fn_arg == (void *) 42);

    helper_ibendport_iterate_local_counter++;

    return 1;
}

void test_ibendport_iterate_local(void) {
    // setup
    setup_handle(SH_TRANS);

    add_local_ibendport(I_FIRST);
    add_local_ibendport(I_SECOND);
    add_local_ibendport(I_THIRD);

    // test
    helper_ibendport_iterate_local_counter = 0;

    CU_ASSERT(semanage_ibendport_iterate_local(sh,
                             helper_ibendport_iterate_local, (void *) 42) >= 0);
    CU_ASSERT(helper_ibendport_iterate_local_counter == 3);

    // test function which returns error
    helper_ibendport_iterate_local_counter = 0;

    CU_ASSERT(semanage_ibendport_iterate_local(sh,
                        helper_ibendport_iterate_local_error, (void *) 42) < 0);
    CU_ASSERT(helper_ibendport_iterate_local_counter == 1);

    // test function which requests break
    helper_ibendport_iterate_local_counter = 0;

    CU_ASSERT(semanage_ibendport_iterate_local(sh,
                       helper_ibendport_iterate_local_break, (void *) 42) >= 0);
    // TODO: there is a bug
    //CU_ASSERT(helper_ibendport_iterate_local_counter == 1);

    // cleanup
    delete_local_ibendport(I_FIRST);
    delete_local_ibendport(I_SECOND);
    delete_local_ibendport(I_THIRD);

    cleanup_handle(SH_TRANS);
}

// Function semanage_ibendport_list_local

void test_ibendport_list_local(void) {
    semanage_ibendport_t **records = NULL;
    unsigned int count = 42;
    char *name = NULL;
    semanage_context_t *con = NULL;

    // setup
    setup_handle(SH_TRANS);

    add_local_ibendport(I_FIRST);
    add_local_ibendport(I_SECOND);
    add_local_ibendport(I_THIRD);

    // test
    CU_ASSERT(semanage_ibendport_list_local(sh, &records, &count) >= 0);

    CU_ASSERT_PTR_NOT_NULL_FATAL(records);
    CU_ASSERT(count == 3);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL_FATAL(records[i]);

        CU_ASSERT(semanage_ibendport_get_ibdev_name(sh, records[i],
                                                    &name) >= 0);

        con = semanage_ibendport_get_con(records[i]);
        CU_ASSERT_PTR_NOT_NULL_FATAL(con);

        free(name);
    }

    // cleanup
    for (unsigned int i = 0; i < count; i++) {
        semanage_ibendport_free(records[i]);
    }

    free(records);

    delete_local_ibendport(I_FIRST);
    delete_local_ibendport(I_SECOND);
    delete_local_ibendport(I_THIRD);

    cleanup_handle(SH_TRANS);
}

#endif
