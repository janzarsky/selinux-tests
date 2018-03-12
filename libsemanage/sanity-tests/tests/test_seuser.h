#ifndef __TEST_SEUSER_H__
#define __TEST_SEUSER_H__

#include <CUnit/Basic.h>

int seuser_test_init(void);
int seuser_test_cleanup(void);
int seuser_add_tests(CU_pSuite suite);

// seuser_record.h

void test_seuser_key_create(void);
void test_seuser_key_extract(void);
void test_seuser_compare(void);
void test_seuser_compare2(void);
void test_seuser_get_set_name(void);
void test_seuser_get_set_sename(void);
void test_seuser_get_set_mlsrange(void);
void test_seuser_clone(void);
void test_seuser_create(void);

// seusers_policy.h

void test_seuser_query(void);
void test_seuser_exists(void);
void test_seuser_count(void);
void test_seuser_iterate(void);
void test_seuser_list(void);

// seusers_local.h

void test_seuser_modify_del_local(void);
void test_seuser_query_local(void);
void test_seuser_exists_local(void);
void test_seuser_count_local(void);
void test_seuser_iterate_local(void);
void test_seuser_list_local(void);

#endif

