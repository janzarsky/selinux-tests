#ifndef __TEST_HANDLE_H__
#define __TEST_HANDLE_H__

#include <CUnit/Basic.h>

#include "functions.h"

int handle_test_init(void);
int handle_test_cleanup(void);
int handle_add_tests(CU_pSuite suite);

void test_handle_create(void);
void test_connect(void);
void test_disconnect(void);
void test_transaction(void);
void test_commit(void);
void test_is_connected(void);
void test_access_check(void);
void test_is_managed(void);
void test_mls_enabled(void);
void test_msg_set_callback(void);

#if VERS_CHECK(2, 5)
void test_root(void);
#endif

void test_select_store(void);

#endif

