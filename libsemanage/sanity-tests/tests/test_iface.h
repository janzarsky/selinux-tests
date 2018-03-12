#ifndef __TEST_IFACE_H__
#define __TEST_IFACE_H__

#include <CUnit/Basic.h>

int iface_test_init(void);
int iface_test_cleanup(void);
int iface_add_tests(CU_pSuite suite);

// iface_record.h

void test_iface_compare(void);
void test_iface_compare2(void);
void test_iface_key_create(void);
void test_iface_key_extract(void);
void test_iface_get_set_name(void);
void test_iface_get_set_ifcon(void);
void test_iface_get_set_msgcon(void);
void test_iface_create(void);
void test_iface_clone(void);

// iterfaces_policy.h

void test_iface_query(void);
void test_iface_exists(void);
void test_iface_count(void);
void test_iface_iterate(void);
void test_iface_list(void);

// interfaces_local.h

void test_iface_modify_del_query_local(void);
void test_iface_exists_local(void);
void test_iface_count_local(void);
void test_iface_iterate_local(void);
void test_iface_list_local(void);

#endif

