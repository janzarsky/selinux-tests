#ifndef __TEST_BOOL_H__
#define __TEST_BOOL_H__

#include <CUnit/Basic.h>

int bool_test_init(void);
int bool_test_cleanup(void);
int bool_add_tests(CU_pSuite suite);

// boolean_record.h

void test_bool_key_create(void);
void test_bool_key_extract(void);
void test_bool_compare(void);
void test_bool_compare2(void);
void test_bool_get_set_name(void);
void test_bool_get_set_value(void);
void test_bool_create(void);
void test_bool_clone(void);

// booleans_policy.h

void test_bool_query(void);
void test_bool_exists(void);
void test_bool_count(void);
void test_bool_iterate(void);
void test_bool_list(void);

// booleans_local.h

void test_bool_modify_del_local(void);
void test_bool_query_local(void);
void test_bool_exists_local(void);
void test_bool_count_local(void);
void test_bool_iterate_local(void);
void test_bool_list_local(void);

// booleans_active.h

void test_bool_set_active(void);
void test_bool_query_active(void);
void test_bool_exists_active(void);
void test_bool_count_active(void);
void test_bool_iterate_active(void);
void test_bool_list_active(void);

#endif

