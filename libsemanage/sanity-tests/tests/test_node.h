#ifndef __TEST_NODE_H__
#define __TEST_NODE_H__

#include <CUnit/Basic.h>

int node_test_init(void);
int node_test_cleanup(void);
int node_add_tests(CU_pSuite suite);

// node_record.h

void test_node_compare(void);
void test_node_compare2(void);
void test_node_key_create(void);
void test_node_key_extract(void);
void test_node_get_set_addr(void);
void test_node_get_set_addr_bytes(void);
void test_node_get_set_mask(void);
void test_node_get_set_mask_bytes(void);
void test_node_get_set_proto(void);
void test_node_get_proto_str(void);
void test_node_get_set_con(void);
void test_node_create(void);
void test_node_clone(void);

// nodes_policy.h

void test_node_query(void);
void test_node_exists(void);
void test_node_count(void);
void test_node_iterate(void);
void test_node_list(void);

// nodes_local.h

void test_node_modify_del_query_local(void);
void test_node_exists_local(void);
void test_node_count_local(void);
void test_node_iterate_local(void);
void test_node_list_local(void);

#endif

