#ifndef __TEST_MODULE_H__
#define __TEST_MODULE_H__

#include <CUnit/Basic.h>

#include "functions.h"

int module_test_init(void);
int module_test_cleanup(void);
int module_add_tests(CU_pSuite suite);

// modules.h

void test_module_list(void);

#if VERS_CHECK(2, 5)
void test_module_install(void);
void test_module_install_file(void);
void test_module_install_info(void);
void test_module_remove(void);
void test_module_remove_key(void);

void test_module_extract(void);
void test_module_list_all(void);

void test_module_get_name(void);
void test_module_get_module_info(void);

void test_module_info_create(void);
void test_module_info_get_set_priority(void);
void test_module_info_get_set_name(void);
void test_module_info_get_set_lang_ext(void);
void test_module_info_get_set_enabled(void);

void test_module_key_create(void);
void test_module_key_get_set_name(void);
void test_module_key_get_set_priority(void);
void test_module_get_set_enabled(void);
#endif

#endif

