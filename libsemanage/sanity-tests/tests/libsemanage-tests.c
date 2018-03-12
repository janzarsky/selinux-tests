/* Authors: Christopher Ashworth <cashworth@tresys.com>
 *          Caleb Case <ccase@tresys.com>
 *          Chad Sellers <csellers@tresys.com>
 *
 * Copyright (C) 2006 Tresys Technology, LLC
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <stdio.h>

#include "test_handle.h"
#include "test_seuser.h"
#include "test_bool.h"
#include "test_fcontext.h"
#include "test_module.h"
#include "test_port.h"
#include "test_iface.h"
#include "test_node.h"
#include "test_other.h"
#include "test_user.h"
#include "test_ibendport.h"

#include <CUnit/Basic.h>
#include <CUnit/Console.h>
#include <CUnit/TestDB.h>

#define DECLARE_SUITE(name) \
    suites[suite_index] = CU_add_suite(#name, name##_test_init, name##_test_cleanup); \
    if (NULL == suites[suite_index]) { \
        CU_cleanup_registry(); \
        return CU_get_error(); } \
    if (name##_add_tests(suites[suite_index])) { \
        CU_cleanup_registry(); \
        return CU_get_error(); } \
    suites_names[suite_index] = #name; \
    suite_index++;

#define SUITE_NUM 11

CU_pSuite suites[SUITE_NUM] = { NULL };
const char *suites_names[SUITE_NUM] = { NULL };

int suite_index = 0;

int main(int argc, char **argv)
{
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    // when adding new suite, update SUITE_NUM and include header file
    DECLARE_SUITE(handle);
    DECLARE_SUITE(seuser);
    DECLARE_SUITE(fcontext);
    DECLARE_SUITE(module);
    DECLARE_SUITE(bool);
    DECLARE_SUITE(port);
    DECLARE_SUITE(iface);
    DECLARE_SUITE(node);
    DECLARE_SUITE(other);
    DECLARE_SUITE(user);
    DECLARE_SUITE(ibendport);

    CU_basic_set_mode(CU_BRM_VERBOSE);

    if (argc == 1) {
        CU_basic_run_tests();
    }
    else if (argc == 2 && strcmp(argv[1], "list") == 0) {
        for (int i = 0; i < suite_index; i++)
            printf("%s ", suites_names[i]);
    }
    else if (argc == 3 && strcmp(argv[1], "suite") == 0) {
        int i;

        for (i = 0; i < suite_index; i++) {
            if (strcmp(argv[2], suites_names[i]) == 0) {
                CU_basic_run_suite(suites[i]);
                break;
            }
        }
        
        if (i == suite_index) {
            fprintf(stderr, "Invalid suite name\n");
            CU_cleanup_registry();
            return 1;
        }
    }
    else {
        fprintf(stderr, "Invalid arguments\n");
        CU_cleanup_registry();
        return 1;
    }

    CU_cleanup_registry();

    return 0;
}
