#include <stdio.h>
#include <semanage/semanage.h>
#include <semanage/handle.h>
#include <CUnit/Basic.h>

#include "test_user.h"
#include "functions.h"

extern semanage_handle_t *sh;

int user_test_init(void) {
    return 0;
}

int user_test_cleanup(void) {
    return 0;
}

int user_add_tests(CU_pSuite suite) {
    CU_add_test(suite, "user_compare", test_user_compare);
    CU_add_test(suite, "user_compare2", test_user_compare2);
    CU_add_test(suite, "user_key_create", test_user_key_create);
    CU_add_test(suite, "user_key_extract", test_user_key_extract);
    CU_add_test(suite, "user_get_set_name", test_user_get_set_name);
    CU_add_test(suite, "user_get_set_prefix", test_user_get_set_prefix);
    CU_add_test(suite, "user_get_set_mlslevel", test_user_get_set_mlslevel);
    CU_add_test(suite, "user_get_set_mlsrange", test_user_get_set_mlsrange);
    CU_add_test(suite, "user_roles", test_user_roles);
    CU_add_test(suite, "user_create", test_user_create);
    CU_add_test(suite, "user_clone", test_user_clone);

    CU_add_test(suite, "user_query", test_user_query);
    CU_add_test(suite, "user_exists", test_user_exists);
    CU_add_test(suite, "user_count", test_user_count);
    CU_add_test(suite, "user_iterate", test_user_iterate);
    CU_add_test(suite, "user_list", test_user_list);

    CU_add_test(suite, "user_modify_del_query_local",
                test_user_modify_del_query_local);
    CU_add_test(suite, "user_exists_local", test_user_exists_local);
    CU_add_test(suite, "user_count_local", test_user_count_local);
    CU_add_test(suite, "user_iterate_local", test_user_iterate_local);
    CU_add_test(suite, "user_list_local", test_user_list_local);

    return 0;
}

// Function semanage_user_compare

void test_user_compare(void) {
    semanage_user_t *user = NULL;
    semanage_user_key_t *key1 = NULL;
    semanage_user_key_t *key2 = NULL;
    int result = 42;

    // setup
    setup_handle(SH_CONNECT);

    user = get_user_nth(I_FIRST);
    key1 = get_user_key_nth(I_FIRST);
    key2 = get_user_key_nth(I_SECOND);

    // test
    result = semanage_user_compare(user, key1);

    CU_ASSERT(result == 0);

    result = semanage_user_compare(user, key2);

    CU_ASSERT(result != 0);

    // cleanup
    semanage_user_free(user);
    semanage_user_key_free(key1);
    semanage_user_key_free(key2);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_compare2

void test_user_compare2(void) {
    semanage_user_t *user1 = NULL;
    semanage_user_t *user2 = NULL;
    semanage_user_t *user3 = NULL;
    int result = 42;

    // setup
    setup_handle(SH_CONNECT);

    user1 = get_user_nth(I_FIRST);
    user2 = get_user_nth(I_FIRST);
    user3 = get_user_nth(I_SECOND);

    // test
    result = semanage_user_compare2(user1, user2);

    CU_ASSERT(result == 0);

    result = semanage_user_compare2(user1, user3);

    CU_ASSERT(result != 0);

    // cleanup
    semanage_user_free(user1);
    semanage_user_free(user2);
    semanage_user_free(user3);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_key_create

void test_user_key_create(void) {
    semanage_user_key_t *key = NULL;

    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_user_key_create(sh, "asdf", &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_user_key_free(key);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_key_extract

void test_user_key_extract(void) {
    semanage_user_t *user = NULL;
    semanage_user_key_t *key = NULL;

    // setup
    setup_handle(SH_CONNECT);

    user = get_user_nth(I_FIRST);

    // test
    CU_ASSERT(semanage_user_key_extract(sh, user, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    // cleanup
    semanage_user_free(user);
    semanage_user_key_free(key);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_get_name, semanage_user_set_name

void test_user_get_set_name(void) {
    semanage_user_t *user = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_user_create(sh, &user) >= 0);

    // test
    CU_ASSERT(semanage_user_set_name(sh, user, "user_u") == 0);
    CU_ASSERT_STRING_EQUAL(semanage_user_get_name(user), "user_u");

    // cleanup
    semanage_user_free(user);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_get_prefix, semanage_user_set_prefix

void test_user_get_set_prefix(void) {
    semanage_user_t *user = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_user_create(sh, &user) >= 0);

    // test
    CU_ASSERT(semanage_user_set_prefix(sh, user, "user") == 0);
    CU_ASSERT_STRING_EQUAL(semanage_user_get_prefix(user), "user");

    // cleanup
    semanage_user_free(user);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_get_mlslevel, semanage_user_set_mlslevel

void test_user_get_set_mlslevel(void) {
    semanage_user_t *user = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_user_create(sh, &user) >= 0);

    // test
    CU_ASSERT(semanage_user_set_mlslevel(sh, user, "s0") == 0);
    CU_ASSERT_STRING_EQUAL(semanage_user_get_mlslevel(user), "s0");

    // cleanup
    semanage_user_free(user);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_get_mlsrange, semanage_user_set_mlsrange

void test_user_get_set_mlsrange(void) {
    semanage_user_t *user = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_user_create(sh, &user) >= 0);

    // test
    CU_ASSERT(semanage_user_set_mlsrange(sh, user, "s0-s15") == 0);
    CU_ASSERT_STRING_EQUAL(semanage_user_get_mlsrange(user), "s0-s15");

    // cleanup
    semanage_user_free(user);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_get_num_roles, semanage_user_add_role,
// semanage_user_del_role, semanage_user_has_role, semanage_user_get_roles
// semanage_user_set_roles

void test_user_roles(void) {
    semanage_user_t *user = NULL;
    const char **roles_arr = NULL;
    unsigned int num_roles = 42;
    const char *new_roles_arr[] = { "new_role_r", "new_my_role_r" };
    unsigned int new_num_roles = 2;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_user_create(sh, &user) >= 0);

    // test
    CU_ASSERT(semanage_user_get_num_roles(user) == 0);

    CU_ASSERT(semanage_user_add_role(sh, user, "role_r") == 0);
    CU_ASSERT(semanage_user_get_num_roles(user) == 1);

    CU_ASSERT(semanage_user_has_role(user, "role_r"));
    CU_ASSERT(!semanage_user_has_role(user, "my_role_r"));

    CU_ASSERT(semanage_user_add_role(sh, user, "my_role_r") == 0);
    CU_ASSERT(semanage_user_get_num_roles(user) == 2);

    CU_ASSERT(semanage_user_get_roles(sh, user, &roles_arr, &num_roles) >= 0);
    CU_ASSERT(num_roles == 2);
    CU_ASSERT_STRING_EQUAL(roles_arr[0], "role_r");
    CU_ASSERT_STRING_EQUAL(roles_arr[1], "my_role_r");

    CU_ASSERT(semanage_user_set_roles(sh, user, new_roles_arr,
                                      new_num_roles) >= 0);

    CU_ASSERT(semanage_user_has_role(user, "new_role_r"));
    CU_ASSERT(semanage_user_has_role(user, "new_my_role_r"));

    CU_ASSERT(!semanage_user_has_role(user, "role_r"));
    CU_ASSERT(!semanage_user_has_role(user, "my_role_r"));

    semanage_user_del_role(user, "new_my_role_r");
    CU_ASSERT(semanage_user_get_num_roles(user) == 1);

    semanage_user_del_role(user, "new_role_r");
    CU_ASSERT(semanage_user_get_num_roles(user) == 0);

    // cleanup
    semanage_user_free(user);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_create

void test_user_create(void) {
    semanage_user_t *user = NULL;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_user_create(sh, &user) >= 0);

    CU_ASSERT(semanage_user_set_name(sh, user, "user_u") >= 0);
    CU_ASSERT(semanage_user_set_prefix(sh, user, "user") >= 0);
    CU_ASSERT(semanage_user_set_mlslevel(sh, user, "s0") >= 0);
    CU_ASSERT(semanage_user_set_mlsrange(sh, user, "s0-s15") >= 0);

    // cleanup
    semanage_user_free(user);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_clone

void test_user_clone(void) {
    semanage_user_t *user = NULL;
    semanage_user_t *user_clone = NULL;

    // setup
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_user_create(sh, &user) >= 0);

    CU_ASSERT(semanage_user_set_name(sh, user, "user_u") >= 0);
    CU_ASSERT(semanage_user_set_prefix(sh, user, "user") >= 0);
    CU_ASSERT(semanage_user_set_mlslevel(sh, user, "s0") >= 0);
    CU_ASSERT(semanage_user_set_mlsrange(sh, user, "s0-s15") >= 0);

    // test
    CU_ASSERT(semanage_user_clone(sh, user, &user_clone) >= 0);

    CU_ASSERT_STRING_EQUAL(semanage_user_get_name(user), "user_u");
    CU_ASSERT_STRING_EQUAL(semanage_user_get_prefix(user), "user");
    CU_ASSERT_STRING_EQUAL(semanage_user_get_mlslevel(user), "s0");
    CU_ASSERT_STRING_EQUAL(semanage_user_get_mlsrange(user), "s0-s15");

    // cleanup
    semanage_user_free(user);
    semanage_user_free(user_clone);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_query

void test_user_query(void) {
    semanage_user_t *user = NULL;
    semanage_user_key_t *key = NULL;

    // setup
    setup_handle(SH_CONNECT);

    key = get_user_key_nth(I_FIRST);

    // test
    CU_ASSERT(semanage_user_query(sh, key, &user) >= 0);

    // TODO: test values
    CU_ASSERT_PTR_NOT_NULL(user);

    // cleanup
    semanage_user_free(user);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_exists

void test_user_exists(void) {
    semanage_user_key_t *key1 = NULL;
    semanage_user_key_t *key2 = NULL;
    int response = 42;

    // setup
    setup_handle(SH_CONNECT);

    key1 = get_user_key_nth(I_FIRST);
    
    CU_ASSERT(semanage_user_key_create(sh, "asdf", &key2) >= 0);

    // test
    CU_ASSERT(semanage_user_exists(sh, key1, &response) >= 0);

    CU_ASSERT(response);

    CU_ASSERT(semanage_user_exists(sh, key2, &response) >= 0);

    CU_ASSERT(!response);

    // cleanup
    semanage_user_key_free(key1);
    semanage_user_key_free(key2);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_count

void test_user_count(void) {
    unsigned int count = 42;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_user_count(sh, &count) >= 0);
    
    // TODO: check real count
    CU_ASSERT(count > 0);

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_iterate

unsigned int counter_user_iterate = 0;

int handler_user_iterate(const semanage_user_t *record, void *varg) {
    counter_user_iterate++;
    return 0;
}

void test_user_iterate(void) {
    // setup
    setup_handle(SH_CONNECT);

    // test
    semanage_user_iterate(sh, handler_user_iterate, NULL);

    // TODO: check real count
    CU_ASSERT(counter_user_iterate > 0);

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_list

void test_user_list(void) {
    semanage_user_t **records = NULL;
    unsigned int count = 42;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_user_list(sh, &records, &count) >= 0);

    // TODO: check real count
    CU_ASSERT(count > 0);

    // TODO: check real values
    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    for (unsigned int i = 0; i < count; i++) {
        semanage_user_free(records[i]);
    }

    // cleanup
    cleanup_handle(SH_CONNECT);
}

// Function semanage_user_modify_local, semanage_user_del_local,
// semanage_user_query_local

void test_user_modify_del_query_local(void) {
    semanage_user_t *user;
    semanage_user_t *user_local;
    semanage_user_key_t *key = NULL;

    // setup
    setup_handle(SH_TRANS);

    user = get_user_nth(I_FIRST);

    CU_ASSERT(semanage_user_key_extract(sh, user, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    // test
    CU_ASSERT(semanage_user_modify_local(sh, key, user) >= 0);

    // write changes to file
    commit();
    begin_transaction();

    CU_ASSERT(semanage_user_query_local(sh, key, &user_local) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(user_local);

    CU_ASSERT(semanage_user_del_local(sh, key) >= 0);

    CU_ASSERT(semanage_user_query_local(sh, key, &user_local) < 0);

    // cleanup
    semanage_user_free(user);
    cleanup_handle(SH_TRANS);
}

// Function semanage_user_exists_local

void test_user_exists_local(void) {
    semanage_user_t *user = NULL;
    semanage_user_key_t *key1 = NULL;
    semanage_user_key_t *key2 = NULL;
    int response = 42;

    // setup
    setup_handle(SH_TRANS);

    add_local_user(I_FIRST);

    key1 = get_user_key_nth(I_FIRST);

    CU_ASSERT(semanage_user_key_create(sh, "asdf", &key2) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key2);

    // test existing
    CU_ASSERT(semanage_user_exists_local(sh, key1, &response) >= 0);

    CU_ASSERT(response);

    // test nonexistent
    CU_ASSERT(semanage_user_exists_local(sh, key2, &response) >= 0);

    CU_ASSERT(!response);

    // cleanup
    CU_ASSERT(semanage_user_del_local(sh, key1) >= 0);
    semanage_user_free(user);
    semanage_user_key_free(key1);
    semanage_user_key_free(key2);
    cleanup_handle(SH_TRANS);
}

// Function semanage_user_count_local

void test_user_count_local(void) {
    unsigned int count = 42;

    // setup
    setup_handle(SH_TRANS);

    add_local_user(I_FIRST);
    add_local_user(I_SECOND);
    add_local_user(I_THIRD);

    // test
    CU_ASSERT(semanage_user_count_local(sh, &count) >= 0);

    CU_ASSERT(count == 3);

    // cleanup
    delete_local_user(I_FIRST);
    delete_local_user(I_SECOND);
    delete_local_user(I_THIRD);

    cleanup_handle(SH_TRANS);
}

// Function semanage_user_iterate_local

unsigned int counter_user_iterate_local = 0;

int handler_user_iterate_local(const semanage_user_t *record, void *varg) {
    counter_user_iterate_local++;
    return 0;
}

void test_user_iterate_local(void) {
    // setup
    setup_handle(SH_TRANS);

    add_local_user(I_FIRST);
    add_local_user(I_SECOND);
    add_local_user(I_THIRD);

    // test
    semanage_user_iterate_local(sh, handler_user_iterate_local, NULL);

    CU_ASSERT(counter_user_iterate_local == 3);

    // cleanup
    delete_local_user(I_FIRST);
    delete_local_user(I_SECOND);
    delete_local_user(I_THIRD);

    cleanup_handle(SH_TRANS);
}

// Function semanage_user_list_local

void test_user_list_local(void) {
    semanage_user_t **records = NULL;
    unsigned int count = 42;

    // setup
    setup_handle(SH_TRANS);

    add_local_user(I_FIRST);
    add_local_user(I_SECOND);
    add_local_user(I_THIRD);

    // test
    CU_ASSERT(semanage_user_list_local(sh, &records, &count) >= 0);

    CU_ASSERT(count == 3);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    // cleanup
    for (unsigned int i = 0; i < count; i++) {
        semanage_user_free(records[i]);
    }

    delete_local_user(I_FIRST);
    delete_local_user(I_SECOND);
    delete_local_user(I_THIRD);

    cleanup_handle(SH_TRANS);
}

