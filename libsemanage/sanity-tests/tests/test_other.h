#ifndef __TEST_OTHER_H__
#define __TEST_OTHER_H__

#include <CUnit/Basic.h>

#include "functions.h"

int other_test_init(void);
int other_test_cleanup(void);
int other_add_tests(CU_pSuite suite);

// context_record.h

void test_semanage_context(void);

// debug.h

#if VERS_CHECK(2, 5)

void test_debug(void);

#endif

#endif

