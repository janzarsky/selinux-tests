#ifndef __TEST_FCONTEXT_H__
#define __TEST_FCONTEXT_H__

#include <CUnit/Basic.h>

int fcontext_test_init(void);
int fcontext_test_cleanup(void);
int fcontext_add_tests(CU_pSuite suite);

// fcontext_record.h

void test_fcontext_compare(void);
void test_fcontext_compare2(void);
void test_fcontext_key_create(void);
void test_fcontext_key_extract(void);
void test_fcontext_get_set_expr(void);
void test_fcontext_get_set_type(void);
void test_fcontext_get_type_str(void);
void test_fcontext_get_set_con(void);
void test_fcontext_create(void);
void test_fcontext_clone(void);

// fcontext_policy.h

void test_fcontext_query(void);
void test_fcontext_exists(void);
void test_fcontext_count(void);
void test_fcontext_iterate(void);
void test_fcontext_list(void);

// fcontext_local.h

void test_fcontext_modify_del_local(void);
void test_fcontext_query_local(void);
void test_fcontext_exists_local(void);
void test_fcontext_count_local(void);
void test_fcontext_iterate_local(void);
void test_fcontext_list_local(void);

#endif

