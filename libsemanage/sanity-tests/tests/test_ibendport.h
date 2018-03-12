#ifndef __TEST_IBENDPORT_H__
#define __TEST_IBENDPORT_H__

#include <CUnit/Basic.h>

#include "functions.h"

int ibendport_test_init(void);
int ibendport_test_cleanup(void);
int ibendport_add_tests(CU_pSuite suite);

#if VERS_CHECK(2, 5)

// ibendports_policy.h

void test_ibendport_query(void);
void test_ibendport_exists(void);
void test_ibendport_count(void);
void test_ibendport_iterate(void);
void test_ibendport_list(void);

// ibendports_local.h

void test_ibendport_modify_del_query_local(void);
void test_ibendport_exists_local(void);
void test_ibendport_count_local(void);
void test_ibendport_iterate_local(void);
void test_ibendport_list_local(void);

#endif

#endif
