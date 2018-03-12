#ifndef __TEST_USER_H__
#define __TEST_USER_H__

#include <CUnit/Basic.h>

int user_test_init(void);
int user_test_cleanup(void);
int user_add_tests(CU_pSuite suite);

// user_record.h

void test_user_compare(void);
void test_user_compare2(void);
void test_user_key_create(void);
void test_user_key_extract(void);
void test_user_get_set_name(void);
void test_user_get_set_prefix(void);
void test_user_get_set_mlslevel(void);
void test_user_get_set_mlsrange(void);
void test_user_roles(void);
void test_user_create(void);
void test_user_clone(void);

// users_policy.h

void test_user_query(void);
void test_user_exists(void);
void test_user_count(void);
void test_user_iterate(void);
void test_user_list(void);

// users_local.h

void test_user_modify_del_query_local(void);
void test_user_exists_local(void);
void test_user_count_local(void);
void test_user_iterate_local(void);
void test_user_list_local(void);

#endif

