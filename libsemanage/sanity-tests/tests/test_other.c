#include <stdio.h>
#include <semanage/semanage.h>
#include <semanage/handle.h>
#include <CUnit/Basic.h>

#include "test_other.h"
#include "functions.h"

extern semanage_handle_t *sh;

int other_test_init(void) {
	return 0;
}

int other_test_cleanup(void) {
	return 0;
}

int other_add_tests(CU_pSuite suite) {
    CU_add_test(suite, "semanage_context", test_semanage_context);

#if VERS_CHECK(2, 5)
    CU_add_test(suite, "debug", test_debug);
#endif

	return 0;
}

// Function semanage_context_get_user, semanage_context_set_user,
// semanage_context_get_role, semanage_context_set_role,
// semanage_context_get_type, semanage_context_set_type,
// semanage_context_get_mls, semanage_context_set_mls,
// semanage_context_create, semanage_context_clone,
// semanage_context_free, semanage_context_from_string
// semanage_context_to_string

void test_semanage_context(void) {
    semanage_context_t *con = NULL;
    semanage_context_t *con_clone = NULL;
    char *str = NULL;

    // setup
    setup_handle(SH_CONNECT);

    // test
    CU_ASSERT(semanage_context_create(sh, &con) >= 0);

    CU_ASSERT(semanage_context_set_user(sh, con, "user_u") >= 0);
    CU_ASSERT_STRING_EQUAL(semanage_context_get_user(con), "user_u");

    CU_ASSERT(semanage_context_set_role(sh, con, "role_r") >= 0);
    CU_ASSERT_STRING_EQUAL(semanage_context_get_role(con), "role_r");
    
    CU_ASSERT(semanage_context_set_type(sh, con, "type_t") >= 0);
    CU_ASSERT_STRING_EQUAL(semanage_context_get_type(con), "type_t");
    
    CU_ASSERT(semanage_context_set_mls(sh, con, "s0") >= 0);
    CU_ASSERT_STRING_EQUAL(semanage_context_get_mls(con), "s0");

    CU_ASSERT(semanage_context_to_string(sh, con, &str) >= 0);
    CU_ASSERT_PTR_NOT_NULL(str);
    CU_ASSERT_STRING_EQUAL(str, "user_u:role_r:type_t:s0");

    CU_ASSERT(semanage_context_from_string(sh, "my_u:my_r:my_t:s0",
                                           &con) >= 0);
    
    CU_ASSERT_STRING_EQUAL(semanage_context_get_user(con), "my_u");
    CU_ASSERT_STRING_EQUAL(semanage_context_get_role(con), "my_r");
    CU_ASSERT_STRING_EQUAL(semanage_context_get_type(con), "my_t");
    CU_ASSERT_STRING_EQUAL(semanage_context_get_mls(con), "s0");

    CU_ASSERT(semanage_context_clone(sh, con, &con_clone) >= 0);

    CU_ASSERT_STRING_EQUAL(semanage_context_get_user(con_clone), "my_u");
    CU_ASSERT_STRING_EQUAL(semanage_context_get_role(con_clone), "my_r");
    CU_ASSERT_STRING_EQUAL(semanage_context_get_type(con_clone), "my_t");
    CU_ASSERT_STRING_EQUAL(semanage_context_get_mls(con_clone), "s0");

    // cleanup
    semanage_context_free(con);
    semanage_context_free(con_clone);
    cleanup_handle(SH_CONNECT);
}

// Function semanage_msg_default_handler

#if VERS_CHECK(2, 5)

void test_debug(void) {
    semanage_module_info_t *modinfo = NULL;

    // setup
    sh = semanage_handle_create();
    CU_ASSERT_PTR_NOT_NULL(sh);

    CU_ASSERT(semanage_connect(sh) >= 0);

    CU_ASSERT(semanage_module_info_create(sh, &modinfo) >= 0);

    // test
    CU_ASSERT(semanage_module_info_set_priority(sh, modinfo, -42) < 0);

    // cleanup
    CU_ASSERT(semanage_disconnect(sh) >= 0);
    semanage_handle_destroy(sh);
}

#endif
