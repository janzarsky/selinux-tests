#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <semanage/semanage.h>
#include <semanage/handle.h>
#include <selinux/selinux.h>
#include <CUnit/Basic.h>

#include "test_bool.h"
#include "functions.h"

/*  The following functions does not have 100 % code coverage because we do not
 *  test bad mallocs:
 *  bool_read_list
 *  bool_activedb_base_init
 */

// TODO: test semanage_bool_compare2_qsort
// TODO: bool_commit_list: for loop after continue
// TODO: bool_commit_list: make security_set_boolean_list return error
// TODO: bool_commit_list: free blist loop
// TODO: test bool_print
// TODO: test bool_parse
// TODO: bool_file_dbase_init: make dbase_file_init return error
// TODO: bool_policydb_dbase_init: make dbase_policydb_init return error

extern semanage_handle_t *sh;

char *BOOL;
char *BOOL2;
char *BOOL_NONEXISTENT;
unsigned int BOOLS_COUNT;

int bool_test_init(void) {
#if VERS_CHECK(2, 5)
    char *bools_path = "/sys/fs/selinux/booleans";

    DIR *dp;
    struct dirent *ep;

    dp = opendir(bools_path);

    if (dp == NULL) {
        fprintf(stderr, "Error reading booleans\n");
        return 1;
    }

    BOOLS_COUNT = 0;

    while ((ep = readdir(dp))) {
        BOOLS_COUNT++;

        if (BOOLS_COUNT == 3) {
            BOOL = strdup(ep->d_name);
        }
        else if (BOOLS_COUNT == 4) {
            BOOL2 = strdup(ep->d_name);
        }
    }
    
    // ignore . and ..
    BOOLS_COUNT -= 2;

    BOOL_NONEXISTENT = "nonexistentbool";

    closedir(dp);

    if (BOOLS_COUNT == 0) {
        fprintf(stderr, "BOOLS_COUNT is zero\n");
        return 1;
    }
#else
    BOOL = "secure_mode";
    BOOL2 = "httpd_tmp_exec";
    BOOL_NONEXISTENT = "nonexistentbool";
#endif

	return 0;
}

int bool_test_cleanup(void) {
	return 0;
}

int bool_add_tests(CU_pSuite suite) {
    CU_add_test(suite, "bool_key_create", test_bool_key_create);
    CU_add_test(suite, "bool_key_extract", test_bool_key_extract);
    CU_add_test(suite, "bool_compare", test_bool_compare);
    CU_add_test(suite, "bool_compare2", test_bool_compare2);
    CU_add_test(suite, "bool_get_set_name", test_bool_get_set_name);
    CU_add_test(suite, "bool_get_set_value", test_bool_get_set_value);
    CU_add_test(suite, "bool_create", test_bool_create);
    CU_add_test(suite, "bool_clone", test_bool_clone);
                                 
    CU_add_test(suite, "bool_query", test_bool_query);
    CU_add_test(suite, "bool_exists", test_bool_exists);
    CU_add_test(suite, "bool_count", test_bool_count);
    CU_add_test(suite, "bool_iterate", test_bool_iterate);
    CU_add_test(suite, "bool_list", test_bool_list);

    CU_add_test(suite, "bool_modify_del_local", test_bool_modify_del_local);
    CU_add_test(suite, "bool_query_local", test_bool_query_local);
    CU_add_test(suite, "bool_exists_local", test_bool_exists_local);
    CU_add_test(suite, "bool_count_local", test_bool_count_local);
    CU_add_test(suite, "bool_iterate_local", test_bool_iterate_local);
    CU_add_test(suite, "bool_list_local", test_bool_list_local);
                                 
    CU_add_test(suite, "bool_set_active", test_bool_set_active);
    CU_add_test(suite, "bool_query_active", test_bool_query_active);
    CU_add_test(suite, "bool_exists_active", test_bool_exists_active);
    CU_add_test(suite, "bool_count_active", test_bool_count_active);
    CU_add_test(suite, "bool_iterate_active", test_bool_iterate_active);
    CU_add_test(suite, "bool_list_active", test_bool_list_active);
     
	return 0;
}

// Function bool_key_create

void helper_bool_key_create(level_t level) {
    semanage_bool_key_t *key = NULL;

    setup_handle(level);

    CU_ASSERT(semanage_bool_key_create(sh, "", &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_bool_key_free(key);

    key = NULL;

    CU_ASSERT(semanage_bool_key_create(sh, "testbool", &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_bool_key_free(key);

    CU_ASSERT_SIGNAL(semanage_bool_key_create(sh, "testbool", NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_bool_key_create(void) {
    helper_bool_key_create(SH_CONNECT);
    helper_bool_key_create(SH_TRANS);
}

// Function bool_key_extract

#define SK_NULL 1
#define SK_NEW 2
#define SK_INDEX 3
#define SK_KEY_NULL 4
void helper_bool_key_extract(level_t level, int mode) {
    semanage_bool_t *boolean;
    semanage_bool_key_t *key;
    int result;
    
    setup_handle(level);

    switch (mode) {
        case SK_NULL:
            boolean = NULL;
            break;
        case SK_NEW:
            boolean = get_bool_new();
            break;
        case SK_INDEX:
            boolean = get_bool_nth(0);
            break;
        case SK_KEY_NULL:
            boolean = get_bool_nth(0);
            break;
        default:
            CU_FAIL_FATAL("Invalid mode\n");
    }

    if (mode == SK_KEY_NULL)
        result = semanage_bool_key_extract(sh, boolean, NULL);
    else
        result = semanage_bool_key_extract(sh, boolean, &key);

    CU_ASSERT(result >= 0);

    result = semanage_bool_compare(boolean, key);

    CU_ASSERT(result == 0);

    semanage_bool_key_free(key);
    semanage_bool_free(boolean);

    cleanup_handle(level);
}

void test_bool_key_extract(void) {
    CU_ASSERT_SIGNAL(helper_bool_key_extract(SH_CONNECT, SK_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_key_extract(SH_CONNECT, SK_KEY_NULL), SIGSEGV);

    // FIXME
    //helper_bool_key_extract(SH_CONNECT, SK_NEW);
    CU_ASSERT_SIGNAL(helper_bool_key_extract(SH_CONNECT, SK_NEW), SIGSEGV);

    helper_bool_key_extract(SH_CONNECT, SK_INDEX);

    CU_ASSERT_SIGNAL(helper_bool_key_extract(SH_TRANS, SK_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_key_extract(SH_TRANS, SK_KEY_NULL), SIGSEGV);

    // FIXME
    //helper_bool_key_extract(SH_TRANS, SK_NEW);
    CU_ASSERT_SIGNAL(helper_bool_key_extract(SH_TRANS, SK_NEW), SIGSEGV);

    helper_bool_key_extract(SH_TRANS, SK_INDEX);
}
#undef SK_NULL
#undef SK_NEW
#undef SK_INDEX
#undef SK_KEY_NULL

// Function bool_compare

void helper_bool_compare(level_t level, int bool_index1, int bool_index2) {
    semanage_bool_t *boolean;
    semanage_bool_key_t *key;
    int result;

    setup_handle(level);

    boolean = get_bool_nth(bool_index1);
    key = get_bool_key_nth(bool_index2);

    result = semanage_bool_compare(boolean, key);

    if (bool_index1 == bool_index2) {
        CU_ASSERT(result == 0);
    }
    else {
        CU_ASSERT(result != 0);
    }

    semanage_bool_free(boolean);
    semanage_bool_key_free(key);
    cleanup_handle(level);
}

void test_bool_compare(void) {
    helper_bool_compare(SH_CONNECT, I_FIRST,  I_FIRST);
    helper_bool_compare(SH_CONNECT, I_FIRST,  I_SECOND);
    helper_bool_compare(SH_CONNECT, I_SECOND, I_FIRST);
    helper_bool_compare(SH_CONNECT, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_bool_compare(SH_CONNECT, I_NULL, I_FIRST), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_compare(SH_CONNECT, I_FIRST, I_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_compare(SH_CONNECT, I_NULL,  I_NULL), SIGSEGV);

    helper_bool_compare(SH_TRANS, I_FIRST,  I_FIRST);
    helper_bool_compare(SH_TRANS, I_FIRST,  I_SECOND);
    helper_bool_compare(SH_TRANS, I_SECOND, I_FIRST);
    helper_bool_compare(SH_TRANS, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_bool_compare(SH_TRANS, I_NULL,  I_FIRST), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_compare(SH_TRANS, I_FIRST, I_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_compare(SH_TRANS, I_NULL,  I_NULL), SIGSEGV);
}

// Function bool_compare2

void helper_bool_compare2(level_t level, int bool_index1, int bool_index2) {
    semanage_bool_t *bool1;
    semanage_bool_t *bool2;
    int result;

    setup_handle(level);

    bool1 = get_bool_nth(bool_index1);
    bool2 = get_bool_nth(bool_index2);

    result = semanage_bool_compare2(bool1, bool2);

    if (bool_index1 == bool_index2) {
        CU_ASSERT(result == 0);
    }
    else {
        CU_ASSERT(result != 0);
    }

    semanage_bool_free(bool1);
    semanage_bool_free(bool2); 
    cleanup_handle(level);
}

void test_bool_compare2(void) {
    helper_bool_compare2(SH_CONNECT, I_FIRST,  I_FIRST);
    helper_bool_compare2(SH_CONNECT, I_FIRST,  I_SECOND);
    helper_bool_compare2(SH_CONNECT, I_SECOND, I_FIRST);
    helper_bool_compare2(SH_CONNECT, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_bool_compare2(SH_CONNECT, I_NULL, I_FIRST),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_compare2(SH_CONNECT, I_FIRST, I_NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_compare2(SH_CONNECT, I_NULL, I_NULL),
                     SIGSEGV);

    helper_bool_compare2(SH_TRANS, I_FIRST,  I_FIRST);
    helper_bool_compare2(SH_TRANS, I_FIRST,  I_SECOND);
    helper_bool_compare2(SH_TRANS, I_SECOND, I_FIRST);
    helper_bool_compare2(SH_TRANS, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_bool_compare2(SH_TRANS, I_NULL, I_FIRST), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_compare2(SH_TRANS, I_FIRST, I_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_compare2(SH_TRANS, I_NULL, I_NULL), SIGSEGV);
}

// Function bool_get_name, bool_set_name

void helper_bool_get_set_name(level_t level, int bool_index, const char *name) {
    semanage_bool_t *boolean;
    const char *new_name = NULL;
    
    setup_handle(level);

    boolean = get_bool_nth(bool_index);

    CU_ASSERT(semanage_bool_set_name(sh, boolean, name) >= 0);

    new_name = semanage_bool_get_name(boolean);

    CU_ASSERT_PTR_NOT_NULL(new_name);
    CU_ASSERT_STRING_EQUAL(new_name, name);

    semanage_bool_free(boolean);
    cleanup_handle(level);
}

void test_bool_get_set_name(void) {
    CU_ASSERT_SIGNAL(helper_bool_get_set_name(SH_CONNECT, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_get_set_name(SH_CONNECT, I_NULL, "testbool"),
                     SIGSEGV);

    helper_bool_get_set_name(SH_CONNECT, I_FIRST, "testbool");
    helper_bool_get_set_name(SH_CONNECT, I_FIRST, "");
    helper_bool_get_set_name(SH_CONNECT, I_SECOND, "testbool");
    helper_bool_get_set_name(SH_CONNECT, I_SECOND, "");

    CU_ASSERT_SIGNAL(helper_bool_get_set_name(SH_TRANS, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_bool_get_set_name(SH_TRANS, I_NULL, "testbool"),
                     SIGSEGV);

    helper_bool_get_set_name(SH_TRANS, I_FIRST, "testbool");
    helper_bool_get_set_name(SH_TRANS, I_FIRST, "");
    helper_bool_get_set_name(SH_TRANS, I_SECOND, "testbool");
    helper_bool_get_set_name(SH_TRANS, I_SECOND, "");
}

// Function bool_get_value, bool_set_value

void helper_bool_get_set_value(int bool_index, int value) {
    semanage_bool_t *boolean;
    int new_value = 0;

    setup_handle(SH_CONNECT);
    boolean = get_bool_nth(bool_index);
    cleanup_handle(SH_CONNECT);

    semanage_bool_set_value(boolean, value);

    new_value = semanage_bool_get_value(boolean);

    CU_ASSERT(new_value == value);

    semanage_bool_free(boolean);
}

void test_bool_get_set_value(void) {
    CU_ASSERT_SIGNAL(helper_bool_get_set_value(I_NULL, 0), SIGSEGV);

    helper_bool_get_set_value(I_FIRST, 1);
    helper_bool_get_set_value(I_FIRST, 0);
    helper_bool_get_set_value(I_SECOND, 1);
    helper_bool_get_set_value(I_SECOND, 0);
}

// Function bool_create

void helper_bool_create(level_t level) {
    semanage_bool_t *boolean;
    
    setup_handle(level);

    CU_ASSERT(semanage_bool_create(sh, &boolean) >= 0);

    CU_ASSERT_PTR_NULL(semanage_bool_get_name(boolean));
    CU_ASSERT(semanage_bool_get_value(boolean) == 0);

    cleanup_handle(level);
}

void test_bool_create(void) {
    CU_ASSERT_SIGNAL(helper_bool_create(SH_NULL), SIGSEGV);
    helper_bool_create(SH_HANDLE);
    helper_bool_create(SH_CONNECT);
    helper_bool_create(SH_TRANS);
}

// Function bool_clone

void helper_bool_clone(level_t level, int bool_index) {
    semanage_bool_t *boolean;
    semanage_bool_t *boolean_clone;
    const char *str;
    const char *str_clone;
    int value;
    int value_clone;
    
    setup_handle(level);

    boolean = get_bool_nth(bool_index);

    CU_ASSERT(semanage_bool_clone(sh, boolean, &boolean_clone) >= 0);

    str = semanage_bool_get_name(boolean);
    str_clone = semanage_bool_get_name(boolean_clone);

    CU_ASSERT_STRING_EQUAL(str, str_clone);

    value = semanage_bool_get_value(boolean);
    value_clone = semanage_bool_get_value(boolean_clone);

    CU_ASSERT_EQUAL(value, value_clone);

    cleanup_handle(level);
}

void test_bool_clone(void) {
    CU_ASSERT_SIGNAL(helper_bool_clone(SH_CONNECT, I_NULL), SIGSEGV);
    helper_bool_clone(SH_CONNECT, I_FIRST);
    helper_bool_clone(SH_CONNECT, I_SECOND);

    CU_ASSERT_SIGNAL(helper_bool_clone(SH_TRANS, I_NULL), SIGSEGV);
    helper_bool_clone(SH_TRANS, I_FIRST);
    helper_bool_clone(SH_TRANS, I_SECOND);
}

// Function bool_query

void helper_bool_query(level_t level, const char *bool_str, int exp_result) {
    semanage_bool_key_t *key;
    // some non-null value
    semanage_bool_t *response = (void *) 42;
    const char *name;

    setup_handle(level);

    key = get_bool_key_from_str(bool_str);

    CU_ASSERT(semanage_bool_query(sh, key, &response) >= 0);

    if (exp_result >= 0) {
        name = semanage_bool_get_name(response);

        CU_ASSERT_STRING_EQUAL(name, bool_str);
    }
    else {
        CU_ASSERT_PTR_NULL(response);
    }

    cleanup_handle(level);
}

void test_bool_query(void) {
    CU_ASSERT_SIGNAL(helper_bool_query(SH_CONNECT, NULL, -1), SIGSEGV);

    helper_bool_query(SH_CONNECT, BOOL,  1);
    helper_bool_query(SH_CONNECT, BOOL2, 1);
    helper_bool_query(SH_CONNECT, BOOL_NONEXISTENT, -1);

    CU_ASSERT_SIGNAL(helper_bool_query(SH_TRANS, NULL, -1), SIGSEGV);

    helper_bool_query(SH_TRANS, BOOL,  1);
    helper_bool_query(SH_TRANS, BOOL2, 1);
    helper_bool_query(SH_TRANS, BOOL_NONEXISTENT, -1);
}

// Functon bool_exists

void helper_bool_exists(level_t level, char * bool_str, int exp_response) {
    semanage_bool_key_t *key;
    int response;

    setup_handle(level);

    key = get_bool_key_from_str(bool_str);

    CU_ASSERT(semanage_bool_exists(sh, key, &response) >= 0);
    CU_ASSERT(response == exp_response);

    CU_ASSERT_SIGNAL(semanage_bool_exists(sh, key, NULL), SIGSEGV);

    semanage_bool_key_free(key);

    cleanup_handle(level);
}

void test_bool_exists(void) {
    CU_ASSERT_SIGNAL(helper_bool_exists(SH_CONNECT, NULL, 0), SIGSEGV);

    helper_bool_exists(SH_CONNECT, BOOL,  1);
    helper_bool_exists(SH_CONNECT, BOOL2, 1);
    helper_bool_exists(SH_CONNECT, BOOL_NONEXISTENT, 0);

    CU_ASSERT_SIGNAL(helper_bool_exists(SH_TRANS, NULL, 0), SIGSEGV);

    helper_bool_exists(SH_TRANS, BOOL,  1);
    helper_bool_exists(SH_TRANS, BOOL2, 1);
    helper_bool_exists(SH_TRANS, BOOL_NONEXISTENT, 0);
}

// Function bool_count

void test_bool_count(void) {
    unsigned int response;

    CU_ASSERT_SIGNAL(semanage_bool_count(NULL, &response), SIGSEGV);

    //handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_bool_count(sh, &response) < 0);

    CU_ASSERT(semanage_bool_count(sh, NULL) < 0);
    
    cleanup_handle(SH_HANDLE);
    
    //connect
    response = 0;
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_bool_count(sh, &response) >= 0);
#if VERS_CHECK(2, 5)
    CU_ASSERT(response == BOOLS_COUNT);
#else
    CU_ASSERT(response > 0);
#endif

    CU_ASSERT_SIGNAL(semanage_bool_count(sh, NULL), SIGSEGV);

    cleanup_handle(SH_CONNECT);

    //trans
    response = 0;
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_bool_count(sh, &response) >= 0);
#if VERS_CHECK(2, 5)
    CU_ASSERT(response == BOOLS_COUNT);
#else
    CU_ASSERT(response > 0);
#endif

    CU_ASSERT_SIGNAL(semanage_bool_count(sh, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function bool_iterate

unsigned int counter_bool_iterate = 0;

int handler_bool_iterate(const semanage_bool_t *record, void *varg) {
    counter_bool_iterate++;
    return 0;
}

void helper_bool_iterate_invalid(void) {
    CU_ASSERT_SIGNAL(semanage_bool_iterate(NULL, &handler_bool_iterate, NULL),
                     SIGSEGV);

    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_bool_iterate(sh, &handler_bool_iterate, NULL) < 0);

    CU_ASSERT(semanage_bool_iterate(sh, NULL, NULL) < 0);

    cleanup_handle(SH_HANDLE);
}

void helper_bool_iterate(level_t level) {
    setup_handle(level);

    counter_bool_iterate = 0;

    CU_ASSERT(semanage_bool_iterate(sh, &handler_bool_iterate, NULL) >= 0);
#if VERS_CHECK(2, 5)
    CU_ASSERT(counter_bool_iterate == BOOLS_COUNT);
#else
    CU_ASSERT(counter_bool_iterate > 0);
#endif

    CU_ASSERT_SIGNAL(semanage_bool_iterate(sh, NULL, NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_bool_iterate(void) {
    helper_bool_iterate_invalid();
    helper_bool_iterate(SH_CONNECT);
    helper_bool_iterate(SH_TRANS);
}

// Function bool_list

void helper_bool_list_invalid(void) {
    semanage_bool_t **records;
    unsigned int count;

    CU_ASSERT_SIGNAL(semanage_bool_list(NULL, &records, &count), SIGSEGV);

    setup_handle(SH_HANDLE);
    
    CU_ASSERT(semanage_bool_list(sh, &records, &count) < 0);

    CU_ASSERT(semanage_bool_list(sh, NULL, &count) < 0);
    CU_ASSERT(semanage_bool_list(sh, &records, NULL) < 0);

    cleanup_handle(SH_HANDLE);
}

void helper_bool_list(level_t level) {
    semanage_bool_t **records;
    unsigned int count;

    setup_handle(level);
    
    CU_ASSERT(semanage_bool_list(sh, &records, &count) >= 0);
#if VERS_CHECK(2, 5)
    CU_ASSERT(count == BOOLS_COUNT);
#else
    CU_ASSERT(count > 0);
#endif

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    for (unsigned int i = 0; i < count; i++) {
        semanage_bool_free(records[i]);
    }

    CU_ASSERT_SIGNAL(semanage_bool_list(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_bool_list(sh, &records, NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_bool_list(void) {
    helper_bool_list_invalid();
    helper_bool_list(SH_CONNECT);
    helper_bool_list(SH_TRANS);
}

// Function bool_modify_local, bool_del_local

void helper_bool_modify_del_local(level_t level, int bool_index,
                                  int exp_result) {
    semanage_bool_t *boolean;
    semanage_bool_t *boolean_local;
    semanage_bool_key_t *key = NULL;
    int result;
    int new_value;

    setup_handle(level);

    boolean = get_bool_nth(bool_index);

    CU_ASSERT(semanage_bool_key_extract(sh, boolean, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    new_value = !semanage_bool_get_value(boolean);
    semanage_bool_set_value(boolean, new_value);

    result = semanage_bool_modify_local(sh, key, boolean);

    if (exp_result < 0) {
        CU_ASSERT(result < 0);
    }
    else {
        CU_ASSERT(result >= 0);

        // write changes to file
        if (level == SH_TRANS) {
            helper_commit();
            helper_begin_transaction();
        }

        CU_ASSERT(semanage_bool_query_local(sh, key, &boolean_local) >= 0);
        CU_ASSERT(semanage_bool_compare2(boolean_local, boolean) == 0);

        CU_ASSERT(semanage_bool_del_local(sh, key) >= 0);

        CU_ASSERT(semanage_bool_query_local(sh, key, &boolean_local) < 0);
    }

    semanage_bool_key_free(key);
    semanage_bool_free(boolean);

    cleanup_handle(level);
}

void test_bool_modify_del_local(void) {
    helper_bool_modify_del_local(SH_CONNECT, I_FIRST,  -1);
    helper_bool_modify_del_local(SH_CONNECT, I_SECOND, -1);

    helper_bool_modify_del_local(SH_TRANS, I_FIRST,  1);
    helper_bool_modify_del_local(SH_TRANS, I_SECOND, 1);
}

// Function bool_query_local

void test_bool_query_local(void) {
    semanage_bool_key_t *key = NULL;
    semanage_bool_t *response = NULL;

    // connect
    setup_handle(SH_CONNECT);

    key = get_bool_key_nth(I_FIRST);

    CU_ASSERT(semanage_bool_query_local(sh, key, &response) < 0);
    CU_ASSERT_PTR_NULL(response);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    key = get_bool_key_nth(I_FIRST);

    CU_ASSERT(semanage_bool_query_local(sh, key, &response) < 0);
    CU_ASSERT_PTR_NULL(response);

    add_local_bool(I_FIRST);

    CU_ASSERT(semanage_bool_query_local(sh, key, &response) >= 0);
    CU_ASSERT_PTR_NOT_NULL(response);

    semanage_bool_key_free(key);
    key = get_bool_key_nth(I_SECOND);

    add_local_bool(I_SECOND);

    CU_ASSERT(semanage_bool_query_local(sh, key, &response) >= 0);
    CU_ASSERT_PTR_NOT_NULL(response);

    delete_local_bool(I_FIRST);
    delete_local_bool(I_SECOND);
    cleanup_handle(SH_TRANS);
}

// Function bool_exists_local

void test_bool_exists_local(void) {
    int response = -1;
    semanage_bool_key_t *key;
    setup_handle(SH_TRANS);

    key = get_bool_key_nth(I_FIRST);

    CU_ASSERT(semanage_bool_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 0);

    add_local_bool(I_FIRST);
    response = -1;

    CU_ASSERT(semanage_bool_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 1);

    delete_local_bool(I_FIRST);
    response = -1;

    CU_ASSERT(semanage_bool_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 0);

    response = -1;

    //CU_ASSERT(semanage_bool_exists_local(sh, NULL, &response) >= 0);
    //CU_ASSERT(response == 0);

    //CU_ASSERT_SIGNAL(semanage_bool_exists_local(sh, key, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function bool_count_local

void test_bool_count_local(void) {
    unsigned int response;
    unsigned int init_count;

    // null
    CU_ASSERT_SIGNAL(semanage_bool_count_local(NULL, &response), SIGSEGV);

    // handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_bool_count_local(sh, &response) < 0);

    cleanup_handle(SH_HANDLE);
    
    // connect
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_bool_count_local(sh, &init_count) >= 0);

    CU_ASSERT_SIGNAL(semanage_bool_count_local(sh, NULL), SIGSEGV);

    cleanup_handle(SH_CONNECT);
    
    // transaction
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_bool_count_local(sh, &response) >= 0);
    CU_ASSERT(response == init_count);

    add_local_bool(I_FIRST);

    CU_ASSERT(semanage_bool_count_local(sh, &response) >= 0);
    CU_ASSERT(response == init_count + 1);

    add_local_bool(I_SECOND);

    CU_ASSERT(semanage_bool_count_local(sh, &response) >= 0);
    CU_ASSERT(response == init_count + 2);
    
    delete_local_bool(I_SECOND);

    CU_ASSERT(semanage_bool_count_local(sh, &response) >= 0);
    CU_ASSERT(response == init_count + 1);

    delete_local_bool(I_FIRST);

    CU_ASSERT_SIGNAL(semanage_bool_count_local(sh, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function bool_iterate_local

unsigned int counter_bool_iterate_local = 0;

int handler_bool_iterate_local(const semanage_bool_t *record, void *varg) {
    counter_bool_iterate_local++;
    return 0;
}

void test_bool_iterate_local(void) {
    unsigned int init_count;

    // null
    CU_ASSERT_SIGNAL(semanage_bool_iterate_local(NULL,
                                   &handler_bool_iterate_local, NULL), SIGSEGV);

    // handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_bool_iterate_local(sh, &handler_bool_iterate_local,
                                          NULL) < 0);

    CU_ASSERT(semanage_bool_iterate_local(sh, NULL, NULL) < 0);

    cleanup_handle(SH_HANDLE);
    
    // connect
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_bool_count_local(sh, &init_count) >= 0);

    counter_bool_iterate_local = 0;
    CU_ASSERT(semanage_bool_iterate_local(sh, &handler_bool_iterate_local,
                                          NULL) >= 0);
    CU_ASSERT(counter_bool_iterate_local == init_count);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    counter_bool_iterate_local = 0;
    CU_ASSERT(semanage_bool_iterate_local(sh, &handler_bool_iterate_local,
                                          NULL) >= 0);
    CU_ASSERT(counter_bool_iterate_local == init_count);

    add_local_bool(I_FIRST);

    counter_bool_iterate_local = 0;
    CU_ASSERT(semanage_bool_iterate_local(sh, &handler_bool_iterate_local,
                                          NULL) >= 0);
    CU_ASSERT(counter_bool_iterate_local == init_count + 1);

    add_local_bool(I_SECOND);

    counter_bool_iterate_local = 0;
    CU_ASSERT(semanage_bool_iterate_local(sh, &handler_bool_iterate_local,
                                          NULL) >= 0);
    CU_ASSERT(counter_bool_iterate_local == init_count + 2);

    //CU_ASSERT_SIGNAL(semanage_bool_iterate_local(sh, NULL, NULL), SIGSEGV);

    delete_local_bool(I_FIRST);
    delete_local_bool(I_SECOND);
    cleanup_handle(SH_TRANS);
}

// Functtion bool_list_local

void test_bool_list_local(void) {
    semanage_bool_t **records;
    unsigned int count;
    unsigned int init_count;

    // null
    CU_ASSERT_SIGNAL(semanage_bool_list_local(NULL, &records, &count), SIGSEGV);

    // handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_bool_list_local(sh, &records, &count) < 0);

    CU_ASSERT(semanage_bool_list_local(sh, NULL, &count) < 0);
    CU_ASSERT(semanage_bool_list_local(sh, &records, NULL) < 0);

    cleanup_handle(SH_HANDLE);

    // connect
    setup_handle(SH_CONNECT);
    
    CU_ASSERT(semanage_bool_count_local(sh, &init_count) >= 0);

    CU_ASSERT(semanage_bool_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == init_count);

    CU_ASSERT_SIGNAL(semanage_bool_list_local(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_bool_list_local(sh, &records, NULL), SIGSEGV);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_bool_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == init_count);

    add_local_bool(I_FIRST);
    
    CU_ASSERT(semanage_bool_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == init_count + 1);
    CU_ASSERT_PTR_NOT_NULL(records[0]);

    add_local_bool(I_SECOND);
    
    CU_ASSERT(semanage_bool_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == init_count + 2);
    CU_ASSERT_PTR_NOT_NULL(records[0]);
    CU_ASSERT_PTR_NOT_NULL(records[1]);

    CU_ASSERT_SIGNAL(semanage_bool_list_local(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_bool_list_local(sh, &records, NULL), SIGSEGV);

    delete_local_bool(I_FIRST);
    delete_local_bool(I_SECOND);
    cleanup_handle(SH_TRANS);
}

// Function semanage_bool_set_active

void test_bool_set_active(void) {
    semanage_bool_key_t *key;
    semanage_bool_t *boolean;
    int response;

    setup_handle(SH_TRANS);

    key = get_bool_key_nth(0);
    boolean = get_bool_nth(0);

    semanage_bool_set_value(boolean, !semanage_bool_get_value(boolean));

    CU_ASSERT(semanage_bool_set_active(sh, key, boolean) >= 0);

    CU_ASSERT(semanage_bool_exists_active(sh, key, &response) >= 0);
    CU_ASSERT(response == 1);

    semanage_bool_set_value(boolean, !semanage_bool_get_value(boolean));

    CU_ASSERT(semanage_bool_set_active(sh, key, boolean) >= 0);

    cleanup_handle(SH_TRANS);
}

// Function semanage_bool_query_active

void helper_bool_query_active(level_t level, const char *bool_str,
                              int exp_result) {
    semanage_bool_key_t *key;
    // some non-null value
    semanage_bool_t *response = (void *) 42;
    const char *name;
    int result;

    setup_handle(level);

    key = get_bool_key_from_str(bool_str);

    result = semanage_bool_query_active(sh, key, &response);

    if (exp_result >= 0) {
        CU_ASSERT(result >= 0);

        name = semanage_bool_get_name(response);

        CU_ASSERT_STRING_EQUAL(name, bool_str);
    }
    else {
        CU_ASSERT(result < 0);
    }

    cleanup_handle(level);
}

void test_bool_query_active(void) {
    CU_ASSERT_SIGNAL(helper_bool_query_active(SH_CONNECT, NULL, -1), SIGSEGV);

    helper_bool_query_active(SH_CONNECT, BOOL,  1);
    helper_bool_query_active(SH_CONNECT, BOOL2, 1);
    helper_bool_query_active(SH_CONNECT, BOOL_NONEXISTENT, -1);

    CU_ASSERT_SIGNAL(helper_bool_query_active(SH_TRANS, NULL, -1), SIGSEGV);

    helper_bool_query_active(SH_TRANS, BOOL,  1);
    helper_bool_query_active(SH_TRANS, BOOL2, 1);
    helper_bool_query_active(SH_TRANS, BOOL_NONEXISTENT, -1);
}

// Function bool_exists_active

void helper_bool_exists_active(level_t level, char * bool_str,
                               int exp_response) {
    semanage_bool_key_t *key;
    int response;

    setup_handle(level);

    key = get_bool_key_from_str(bool_str);

    CU_ASSERT(semanage_bool_exists_active(sh, key, &response) >= 0);
    CU_ASSERT(response == exp_response);

    CU_ASSERT_SIGNAL(semanage_bool_exists_active(sh, key, NULL), SIGSEGV);

    semanage_bool_key_free(key);

    cleanup_handle(level);
}

void test_bool_exists_active(void) {
    CU_ASSERT_SIGNAL(helper_bool_exists_active(SH_CONNECT, NULL, 0), SIGSEGV);

    helper_bool_exists_active(SH_CONNECT, BOOL,  1);
    helper_bool_exists_active(SH_CONNECT, BOOL2, 1);
    helper_bool_exists_active(SH_CONNECT, BOOL_NONEXISTENT, 0);

    CU_ASSERT_SIGNAL(helper_bool_exists_active(SH_TRANS, NULL, 0), SIGSEGV);

    helper_bool_exists_active(SH_TRANS, BOOL,  1);
    helper_bool_exists_active(SH_TRANS, BOOL2, 1);
    helper_bool_exists_active(SH_TRANS, BOOL_NONEXISTENT, 0);
}

// Function semanage_bool_count_active

void test_bool_count_active(void) {
    unsigned int response;

    // null
    CU_ASSERT_SIGNAL(semanage_bool_count_active(NULL, &response), SIGSEGV);

    // handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_bool_count_active(sh, &response) < 0);

    cleanup_handle(SH_HANDLE);
    
    // connect
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_bool_count_active(sh, &response) >= 0);
#if VERS_CHECK(2, 5)
    CU_ASSERT(response == BOOLS_COUNT);
#else
    CU_ASSERT(response > 0);
#endif

    CU_ASSERT_SIGNAL(semanage_bool_count_active(sh, NULL), SIGSEGV);

    cleanup_handle(SH_CONNECT);
    
    // transaction
    setup_handle(SH_TRANS);
    
    CU_ASSERT(semanage_bool_count_active(sh, &response) >= 0);
#if VERS_CHECK(2, 5)
    CU_ASSERT(response == BOOLS_COUNT);
#else
    CU_ASSERT(response > 0);
#endif

    CU_ASSERT_SIGNAL(semanage_bool_count_active(sh, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function semanage_bool_iterate_active
unsigned int counter_bool_iterate_active = 0;

int handler_bool_iterate_active(const semanage_bool_t *record, void *varg) {
    counter_bool_iterate_active++;
    return 0;
}

void test_bool_iterate_active(void) {
    // null
    CU_ASSERT_SIGNAL(semanage_bool_iterate_active(NULL,
                                  &handler_bool_iterate_active, NULL), SIGSEGV);

    // handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_bool_iterate_active(sh, &handler_bool_iterate_active,
                                           NULL) < 0);

    CU_ASSERT(semanage_bool_iterate_active(sh, NULL, NULL) < 0);

    cleanup_handle(SH_HANDLE);

    // connect
    setup_handle(SH_CONNECT);

    counter_bool_iterate_active = 0;
    CU_ASSERT(semanage_bool_iterate_active(sh, &handler_bool_iterate_active,
                                           NULL) >= 0);
#if VERS_CHECK(2, 5)
    CU_ASSERT(counter_bool_iterate_active == BOOLS_COUNT);
#else
    CU_ASSERT(counter_bool_iterate_active > 0);
#endif

    CU_ASSERT_SIGNAL(semanage_bool_iterate_active(sh, NULL, NULL), SIGSEGV);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    counter_bool_iterate_active = 0;
    CU_ASSERT(semanage_bool_iterate_active(sh, &handler_bool_iterate_active,
                                           NULL) >= 0);
#if VERS_CHECK(2, 5)
    CU_ASSERT(counter_bool_iterate_active == BOOLS_COUNT);
#else
    CU_ASSERT(counter_bool_iterate_active > 0);
#endif

    CU_ASSERT_SIGNAL(semanage_bool_iterate_active(sh, NULL, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function semanage_bool_list_active

void helper_bool_list_active_invalid(void) {
    semanage_bool_t **records;
    unsigned int count;

    CU_ASSERT_SIGNAL(semanage_bool_list_active(NULL, &records, &count),
                     SIGSEGV);

    setup_handle(SH_HANDLE);
    
    CU_ASSERT(semanage_bool_list_active(sh, &records, &count) < 0);

    CU_ASSERT(semanage_bool_list_active(sh, NULL, &count) < 0);
    CU_ASSERT(semanage_bool_list_active(sh, &records, NULL) < 0);

    cleanup_handle(SH_HANDLE);
}

void helper_bool_list_active(level_t level) {
    semanage_bool_t **records;
    unsigned int count;

    setup_handle(level);
    
    CU_ASSERT(semanage_bool_list_active(sh, &records, &count) >= 0);
#if VERS_CHECK(2, 5)
    CU_ASSERT(count == BOOLS_COUNT);
#else
    CU_ASSERT(count > 0);
#endif

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    for (unsigned int i = 0; i < count; i++) {
        semanage_bool_free(records[i]);
    }

    CU_ASSERT_SIGNAL(semanage_bool_list_active(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_bool_list_active(sh, &records, NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_bool_list_active(void) {
    helper_bool_list_active_invalid();
    helper_bool_list_active(SH_CONNECT);
    helper_bool_list_active(SH_TRANS);
}
