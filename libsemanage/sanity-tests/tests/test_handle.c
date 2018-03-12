#include <sys/wait.h>

#include <semanage/semanage.h>
#include <semanage/handle.h>

#include <CUnit/Basic.h>

#include "test_handle.h"
#include "functions.h"

extern semanage_handle_t *sh;

int handle_test_init(void)
{
	return 0;
}

int handle_test_cleanup(void)
{
	return 0;
}

int handle_add_tests(CU_pSuite suite)
{
    CU_add_test(suite, "test_handle_create",    test_handle_create);
    CU_add_test(suite, "test_connect",          test_connect);
    CU_add_test(suite, "test_disconnect",       test_disconnect);
    CU_add_test(suite, "test_transaction",      test_transaction);
    CU_add_test(suite, "test_commit",           test_commit);
    CU_add_test(suite, "test_is_connected",     test_is_connected);
    CU_add_test(suite, "test_access_check",     test_access_check);
    CU_add_test(suite, "test_is_managed",       test_is_managed);
    CU_add_test(suite, "test_mls_enabled",      test_mls_enabled);
    CU_add_test(suite, "msg_set_callback",      test_msg_set_callback);

#if VERS_CHECK(2, 5)
    CU_add_test(suite, "test_root",         test_root);
#endif

    CU_add_test(suite, "test_select_store", test_select_store);

	return 0;
}

// Function semanage_handle_create

void test_handle_create(void) {
    sh = semanage_handle_create();

    CU_ASSERT_PTR_NOT_NULL(sh);

    semanage_handle_destroy(sh);
}

// Function semanage_connect
// TODO: make semanage_direct_connect(sh) return error
// TODO: make semanage_check_init return error
// TODO: make semanage_create_store return error
// TODO: make other functions return error
// TODO: try all values of disable dontaudit (add file)

void test_connect(void) {
    // test without handle
    setup_handle(SH_NULL);

    CU_ASSERT_SIGNAL(semanage_connect(sh), SIGABRT);

    // test handle created
    helper_handle_create();

    CU_ASSERT(semanage_connect(sh) >= 0);
    CU_ASSERT(semanage_disconnect(sh) >= 0);

    cleanup_handle(SH_HANDLE);

    // test invalid store
    setup_handle_invalid_store(SH_HANDLE);

    CU_ASSERT(semanage_connect(sh) < 0);

    cleanup_handle(SH_HANDLE);

    // test normal use
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_connect(sh) >= 0);

    // test connecting twice
    // TODO: connecting twice creates memory leaks
    //CU_ASSERT(semanage_connect(sh) < 0);
    CU_ASSERT(semanage_connect(sh) >= 0);

    CU_ASSERT(semanage_disconnect(sh) >= 0);

    cleanup_handle(SH_HANDLE);
}

// Function semanage_disconnect
// TODO: disconnect when disconnect function not set
// TODO: make semanage_remove_directory return error (twice)

void test_disconnect(void) {
    setup_handle(SH_NULL);

    CU_ASSERT_SIGNAL(semanage_disconnect(sh), SIGABRT);

    helper_handle_create();

    CU_ASSERT_SIGNAL(semanage_disconnect(sh), SIGABRT);

    helper_connect();

    CU_ASSERT(semanage_disconnect(sh) >= 0);

    cleanup_handle(SH_HANDLE);
}

// Function semanage_begin_transaction
// TODO: make semanage_get_trans_lock return error
// TODO: make semanage_make_sandbow return error
// TODO: make semanage_make_final return error
// TODO: there is a memory leak in semanage_begin_transaction()
/*
==24810== 101 (80 direct, 21 indirect) bytes in 1 blocks are definitely lost in loss record 26 of 31
==24810==    at 0x4C30A1E: calloc (vg_replace_malloc.c:711)
==24810==    by 0x5062AF8: semanage_genhomedircon (genhomedircon.c:1351)
==24810==    by 0x505FAF4: semanage_direct_commit (direct_api.c:1567)
==24810==    by 0x5064C6C: semanage_commit (handle.c:426)
==24810==    by 0x41032E: test_transaction (test_handle.c:146)
==24810==    by 0x4E3F106: ??? (in /usr/lib64/libcunit.so.1.0.1)
==24810==    by 0x4E3F33C: ??? (in /usr/lib64/libcunit.so.1.0.1)
==24810==    by 0x4E3F855: CU_run_suite (in /usr/lib64/libcunit.so.1.0.1)
==24810==    by 0x42085A: main (libsemanage-tests.c:90)
*/

void test_transaction(void) {
    // test without handle
    setup_handle(SH_NULL);

    CU_ASSERT_SIGNAL(semanage_begin_transaction(sh), SIGABRT);

    // test with handle
    helper_handle_create();

    CU_ASSERT_SIGNAL(semanage_begin_transaction(sh), SIGABRT);

    // test disconnected
    helper_connect();
    helper_disconnect();

    CU_ASSERT(semanage_begin_transaction(sh) < 0);

    cleanup_handle(SH_HANDLE);

    // test normal use
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_begin_transaction(sh) >= 0);
    CU_ASSERT(semanage_commit(sh) >= 0);

    // test beginning transaction twice
    // FIXME
    //CU_ASSERT(semanage_begin_transaction(sh) >= 0);
    //CU_ASSERT(semanage_begin_transaction(sh) >= 0);
    //CU_ASSERT(semanage_commit(sh) >= 0);

    cleanup_handle(SH_CONNECT);
}

// Function semanage_commit
// TODO: try other values of disable dontaudit
// TODO: try other values of preserve tunables
// TODO: make users->dtable->is_modified return true
// TODO: try missing paths (lines 1236-1272)
// TODO: error gotos (lines 1299-1514)
// TODO: make sh->conf->disable_genhomedircon return true
// TODO: make semanage_genhomedircon return error

void test_commit(void) {
    setup_handle(SH_NULL);

    CU_ASSERT_SIGNAL(semanage_commit(sh), SIGABRT);

    helper_handle_create();

    CU_ASSERT_SIGNAL(semanage_commit(sh), SIGABRT);

    helper_connect();

    CU_ASSERT(semanage_commit(sh) < 0);

    helper_begin_transaction();

    // FIXME: memory leaks
    CU_ASSERT(semanage_commit(sh) >= 0);

    cleanup_handle(SH_CONNECT);
}

// Function semanage_is_connected

void test_is_connected(void) {
    setup_handle(SH_NULL);

    CU_ASSERT_SIGNAL(semanage_is_connected(sh), SIGABRT);

    helper_handle_create();

    CU_ASSERT(semanage_is_connected(sh) == 0);

    helper_connect();

    CU_ASSERT(semanage_is_connected(sh) == 1);

    helper_begin_transaction();

    CU_ASSERT(semanage_is_connected(sh) == 1);

    // FIXME: memory leaks (semanage_commit)
    cleanup_handle(SH_TRANS);
}

// Function semanage_access_check

void test_access_check(void) {
    int result = 0;

    // test without handle
    setup_handle(SH_NULL);

    CU_ASSERT_SIGNAL(semanage_access_check(sh), SIGABRT);

    // test with handle
    helper_handle_create();

    result = semanage_access_check(sh);

    CU_ASSERT(result == 0 || result == SEMANAGE_CAN_READ
              || result == SEMANAGE_CAN_WRITE);

    cleanup_handle(SH_HANDLE);
    
    // test with invalid store
    setup_handle_invalid_store(SH_HANDLE);

    CU_ASSERT(semanage_access_check(sh) < 0);

    cleanup_handle(SH_HANDLE);
    
    // test normal use
    setup_handle(SH_CONNECT);

    result = semanage_access_check(sh);

    CU_ASSERT(result == 0 || result == SEMANAGE_CAN_READ
              || result == SEMANAGE_CAN_WRITE);

    cleanup_handle(SH_CONNECT);
}

// Function semanage_is_managed
// TODO: make semanage_check_init return error
// TODO: make semanage_access_check return error

void test_is_managed(void) {
    int result = 0;
    
    // test without handle
    setup_handle(SH_NULL);

    CU_ASSERT_SIGNAL(semanage_is_managed(sh), SIGABRT);

    // test with handle
    helper_handle_create();

    result = semanage_is_managed(sh);

    CU_ASSERT(result == 0 || result == 1);

    // test connected
    helper_connect();

    result = semanage_is_managed(sh);

    CU_ASSERT(result < 0);

    cleanup_handle(SH_CONNECT);
}

// Function semanage_mls_enabled

void test_mls_enabled(void) {
    int result = 0;

    // test without handle
    setup_handle(SH_NULL);

    CU_ASSERT_SIGNAL(semanage_mls_enabled(sh), SIGABRT);

    // test with handle
    helper_handle_create();

    result = semanage_mls_enabled(sh);

    CU_ASSERT(result == 0 || result == 1);

    cleanup_handle(SH_HANDLE);
    
    // test with invalid store
    setup_handle_invalid_store(SH_HANDLE);

    CU_ASSERT(semanage_mls_enabled(sh) < 0);

    cleanup_handle(SH_HANDLE);

    // test connected
    setup_handle(SH_CONNECT);

    result = semanage_mls_enabled(sh);

    CU_ASSERT(result == 0 || result == 1);

    cleanup_handle(SH_CONNECT);
}

// Function semanage_set_callback

int msg_set_callback_count = 0;

void helper_msg_set_callback(void *varg, semanage_handle_t *sh,
                             const char *fmt, ...) {
    msg_set_callback_count++;
}

void test_msg_set_callback(void) {
    setup_handle(SH_CONNECT);

    semanage_msg_set_callback(sh, helper_msg_set_callback, NULL);

    // produce error message
    semanage_commit(sh);

    CU_ASSERT(msg_set_callback_count == 1);

    semanage_msg_set_callback(sh, NULL, NULL);

    // produce error message
    semanage_commit(sh);

    CU_ASSERT(msg_set_callback_count == 1);

    cleanup_handle(SH_CONNECT);
}

// Function semanage_root, semanage_set_root

#if VERS_CHECK(2, 5)

void helper_root(void) {
    const char *root = semanage_root();

    CU_ASSERT_STRING_EQUAL(root, "");

    CU_ASSERT(semanage_set_root("asdf") >= 0);

    root = semanage_root();

    CU_ASSERT_STRING_EQUAL(root, "asdf");

    CU_ASSERT(semanage_set_root("") >= 0);

    root = semanage_root();

    CU_ASSERT_STRING_EQUAL(root, "");
}

void test_root(void) {
    // test without handle
    setup_handle(SH_NULL);

    helper_root();

    // test with handle
    helper_handle_create();

    helper_root();

    // test connected
    helper_connect();

    helper_root();

    cleanup_handle(SH_CONNECT);
}

#endif

// Function semanage_select_store

void helper_select_store(char *name, enum semanage_connect_type type,
                         int exp_result) {
    setup_handle(SH_HANDLE);

    semanage_select_store(sh, name, type);

    int result = semanage_connect(sh);

    if (exp_result < 0) {
        CU_ASSERT(result < 0);
    }
    else {
        CU_ASSERT(result >= 0);
    }

    if (result >= 0)
        cleanup_handle(SH_CONNECT);
    else
        cleanup_handle(SH_HANDLE);
}

void test_select_store(void) {
    helper_select_store("asdf", SEMANAGE_CON_INVALID - 1, -1);
    helper_select_store("asdf", SEMANAGE_CON_POLSERV_REMOTE + 1, -1);
    helper_select_store("", SEMANAGE_CON_DIRECT, 0);
    CU_ASSERT_SIGNAL(helper_select_store(NULL, SEMANAGE_CON_DIRECT, -1),
                     SIGSEGV);

    helper_select_store("asdf", SEMANAGE_CON_INVALID, -1);
    helper_select_store("asdf", SEMANAGE_CON_DIRECT, 0);
    helper_select_store("asdf", SEMANAGE_CON_POLSERV_LOCAL, -1);
    helper_select_store("asdf", SEMANAGE_CON_POLSERV_REMOTE, -1);
}
