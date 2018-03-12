#ifndef __TEST_PORT_H__
#define __TEST_PORT_H__

#include <CUnit/Basic.h>

int port_test_init(void);
int port_test_cleanup(void);
int port_add_tests(CU_pSuite suite);

// port_record.h

void test_port_compare(void);
void test_port_compare2(void);
void test_port_key_create(void);
void test_port_key_extract(void);
void test_port_get_set_proto(void);
void test_port_get_proto_str(void);
void test_port_get_set_port(void);
void test_port_get_set_con(void);
void test_port_create(void);
void test_port_clone(void);

// ports_policy.h

void test_port_query(void);
void test_port_exists(void);
void test_port_count(void);
void test_port_iterate(void);
void test_port_list(void);

// ports_local.h

void test_port_modify_del_local(void);
void test_port_query_local(void);
void test_port_exists_local(void);
void test_port_count_local(void);
void test_port_iterate_local(void);
void test_port_list_local(void);
// internal
void test_port_validate_local(void);

#endif

