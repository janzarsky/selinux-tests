#include <stdio.h>
#include <string.h>
#include <semanage/semanage.h>
#include <semanage/handle.h>
#include <selinux/selinux.h>
#include <CUnit/Basic.h>

#include "test_fcontext.h"
#include "functions.h"

/* TODO: test function type_str
 * TODO: test function fcontext_print
 * TODO: function fcontext_parse, error gotos
 * TODO: test function validate_handler
 */

extern semanage_handle_t *sh;

char *FCONTEXT_EXPR;
int FCONTEXT_TYPE;
char *FCONTEXT_CON;

char *FCONTEXT2_EXPR;
int FCONTEXT2_TYPE;
char *FCONTEXT2_CON;

char *FCONTEXT_NONEXISTENT_EXPR;
int FCONTEXT_NONEXISTENT_TYPE;

unsigned int FCONTEXTS_COUNT;

int get_type(char *t) {
    if (strcmp(t, "--") == 0)
        return SEMANAGE_FCONTEXT_ALL;
    else if (strcmp(t, "-f") == 0)
        return SEMANAGE_FCONTEXT_REG;
    else if (strcmp(t, "-d") == 0)
        return SEMANAGE_FCONTEXT_DIR;
    else if (strcmp(t, "-c") == 0)
        return SEMANAGE_FCONTEXT_CHAR;
    else if (strcmp(t, "-b") == 0)
        return SEMANAGE_FCONTEXT_BLOCK;
    else if (strcmp(t, "-s") == 0)
        return SEMANAGE_FCONTEXT_SOCK;
    else if (strcmp(t, "-l") == 0)
        return SEMANAGE_FCONTEXT_LINK;
    else if (strcmp(t, "-p") == 0)
        return SEMANAGE_FCONTEXT_PIPE;
    else
        return -1;
}

int fcontext_test_init(void) {
    char *policy_type;
    char *fcontexts_path;

    selinux_getpolicytype(&policy_type);

    fcontexts_path = malloc(strlen("/etc/selinux/") + strlen(policy_type)
                     + strlen("/contexts/files/file_contexts") + 1);

    strcpy(fcontexts_path, "/etc/selinux/");
    strcat(fcontexts_path, policy_type);
    strcat(fcontexts_path, "/contexts/files/file_contexts");

    FILE *f = fopen(fcontexts_path, "r");

    if (f == NULL) {
        fprintf(stderr, "Error opening file %s\n", fcontexts_path);
        return 1;
    }

    char line[512];
    unsigned int line_counter = 0;
    char *p1;
    char *p2;
    char *p3;
    //char fcontexts_tmp[512];

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n')
            continue;

        p1 = strtok(line, "\t");
        p2 = strtok(NULL, "\t");
        p3 = strtok(NULL, "\t");
        //strcat(seusers_tmp, " ");
        //strcat(seusers_tmp, p);

        if (line_counter == 0) {
            FCONTEXT_EXPR = strdup(p1);

            if (p3 == NULL) {
                FCONTEXT_TYPE = SEMANAGE_FCONTEXT_ALL;
                FCONTEXT_CON = strdup(p2);
            }
            else {
                FCONTEXT_TYPE = get_type(p2);
                FCONTEXT_CON = strdup(p3);
            }
        }
        // chose entry with type != ALL
        else if (p3 != NULL && strcmp(p2, "--") != 0) {
            FCONTEXT2_EXPR = strdup(p1);
            FCONTEXT2_TYPE = get_type(p2);
            FCONTEXT2_CON = strdup(p3);
        }

        line_counter++;
    }

    //FCONTEXTS = strdup(seusers_tmp);
    
    FCONTEXT_NONEXISTENT_EXPR = "/non/existent/path";
    FCONTEXT_NONEXISTENT_TYPE = SEMANAGE_FCONTEXT_ALL;

    FCONTEXTS_COUNT = line_counter;

    if (FCONTEXTS_COUNT == 0) {
        fprintf(stderr, "FCONTEXTS_COUNT is zero\n");
        return 1;
    }

    free(fcontexts_path);

	return 0;
}

int fcontext_test_cleanup(void) {
	return 0;
}

int fcontext_add_tests(CU_pSuite suite) {
    CU_add_test(suite, "test_fcontext_compare",     test_fcontext_compare);
    CU_add_test(suite, "test_fcontext_compare2",    test_fcontext_compare2);
    CU_add_test(suite, "test_fcontext_key_create",  test_fcontext_key_create);
    CU_add_test(suite, "test_fcontext_key_extract", test_fcontext_key_extract);
    CU_add_test(suite, "test_fcontext_get_set_expr",test_fcontext_get_set_expr);
    CU_add_test(suite, "test_fcontext_get_set_type",test_fcontext_get_set_type);
    CU_add_test(suite, "test_fcontext_get_type_str",test_fcontext_get_type_str);
    CU_add_test(suite, "test_fcontext_get_set_con", test_fcontext_get_set_con);
    CU_add_test(suite, "test_fcontext_create",      test_fcontext_create);
    CU_add_test(suite, "test_fcontext_clone",       test_fcontext_clone);

    CU_add_test(suite, "test_fcontext_query",       test_fcontext_query);
    CU_add_test(suite, "test_fcontext_exists",      test_fcontext_exists);
    CU_add_test(suite, "test_fcontext_count",       test_fcontext_count);
    CU_add_test(suite, "test_fcontext_iterate",     test_fcontext_iterate);
    CU_add_test(suite, "test_fcontext_list",        test_fcontext_list);
    
    CU_add_test(suite, "test_fcontext_modify_del_local",
                test_fcontext_modify_del_local);
    CU_add_test(suite, "test_fcontext_query_local", test_fcontext_query_local);
    CU_add_test(suite, "test_fcontext_exists_local",test_fcontext_exists_local);
    CU_add_test(suite, "test_fcontext_count_local", test_fcontext_count_local);
    CU_add_test(suite, "test_fcontext_iterate_local",
                test_fcontext_iterate_local);
    CU_add_test(suite, "test_fcontext_list_local",  test_fcontext_list_local);

	return 0;
}

// Function semanage_fcontext_compare

void helper_fcontext_compare(level_t level, int fcontext_index1,
                             int fcontext_index2) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_key_t *key;
    int result;

    setup_handle(level);

    fcontext = get_fcontext_nth(fcontext_index1);
    key = get_fcontext_key_nth(fcontext_index2);

    result = semanage_fcontext_compare(fcontext, key);

    if (fcontext_index1 == fcontext_index2) {
        CU_ASSERT(result == 0);
    }
    else {
        CU_ASSERT(result != 0);
    }

    semanage_fcontext_free(fcontext);
    semanage_fcontext_key_free(key);
    cleanup_handle(level);
}

void helper_fcontext_compare_type(level_t level, int index, int increase) {
    semanage_fcontext_t *fcontext = NULL;
    semanage_fcontext_t *fcontext2 = NULL;
    semanage_fcontext_key_t *key = NULL;
    int type = 42;

    // setup
    setup_handle(level);

    fcontext = get_fcontext_nth(index);
    fcontext2 = get_fcontext_nth(index);

    type = semanage_fcontext_get_type(fcontext2);

    if (increase) {
        type += 1;

        if (type > SEMANAGE_FCONTEXT_PIPE)
            type = SEMANAGE_FCONTEXT_ALL;
    }
    else {
        type -= 1;

        if (type < SEMANAGE_FCONTEXT_ALL)
            type = SEMANAGE_FCONTEXT_PIPE;
    }

    semanage_fcontext_set_type(fcontext2, type);

    CU_ASSERT(semanage_fcontext_key_extract(sh, fcontext2, &key) >= 0);

    // test
    CU_ASSERT(semanage_fcontext_compare(fcontext, key) != 0);

    // cleanup
    semanage_fcontext_free(fcontext);
    semanage_fcontext_key_free(key);
    cleanup_handle(level);
}

void test_fcontext_compare(void) {
    helper_fcontext_compare(SH_CONNECT, I_FIRST,  I_FIRST);
    helper_fcontext_compare(SH_CONNECT, I_FIRST,  I_SECOND);
    helper_fcontext_compare(SH_CONNECT, I_SECOND, I_FIRST);
    helper_fcontext_compare(SH_CONNECT, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_fcontext_compare(SH_CONNECT, I_NULL,  I_FIRST),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_compare(SH_CONNECT, I_FIRST, I_NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_compare(SH_CONNECT, I_NULL,  I_NULL),
                     SIGSEGV);

    helper_fcontext_compare_type(SH_CONNECT, I_FIRST, -1);
    helper_fcontext_compare_type(SH_CONNECT, I_FIRST, 1);

    helper_fcontext_compare(SH_TRANS, I_FIRST,  I_FIRST);
    helper_fcontext_compare(SH_TRANS, I_FIRST,  I_SECOND);
    helper_fcontext_compare(SH_TRANS, I_SECOND, I_FIRST);
    helper_fcontext_compare(SH_TRANS, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_fcontext_compare(SH_TRANS, I_NULL,  I_FIRST),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_compare(SH_TRANS, I_FIRST, I_NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_compare(SH_TRANS, I_NULL,  I_NULL),
                     SIGSEGV);

    helper_fcontext_compare_type(SH_TRANS, I_FIRST, -1);
    helper_fcontext_compare_type(SH_TRANS, I_FIRST, 1);
}

// Function semanage_fcontext_compare2
// TODO: test same expr, different type
// TODO: test function semanage_fcontext_compare2_qsort

void helper_fcontext_compare2(level_t level, int fcontext_index1,
                              int fcontext_index2) {
    semanage_fcontext_t *fcontext1;
    semanage_fcontext_t *fcontext2;
    int result;

    setup_handle(level);

    fcontext1 = get_fcontext_nth(fcontext_index1);
    fcontext2 = get_fcontext_nth(fcontext_index2);

    result = semanage_fcontext_compare2(fcontext1, fcontext2);

    if (fcontext_index1 == fcontext_index2) {
        CU_ASSERT(result == 0);
    }
    else {
        CU_ASSERT(result != 0);
    }

    semanage_fcontext_free(fcontext1);
    semanage_fcontext_free(fcontext2); 
    cleanup_handle(level);
}

void test_fcontext_compare2(void) {
    helper_fcontext_compare2(SH_CONNECT, I_FIRST,  I_FIRST);
    helper_fcontext_compare2(SH_CONNECT, I_FIRST,  I_SECOND);
    helper_fcontext_compare2(SH_CONNECT, I_SECOND, I_FIRST);
    helper_fcontext_compare2(SH_CONNECT, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_fcontext_compare2(SH_CONNECT, I_NULL,  I_FIRST),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_compare2(SH_CONNECT, I_FIRST, I_NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_compare2(SH_CONNECT, I_NULL,  I_NULL),
                     SIGSEGV);

    helper_fcontext_compare2(SH_TRANS, I_FIRST,  I_FIRST);
    helper_fcontext_compare2(SH_TRANS, I_FIRST,  I_SECOND);
    helper_fcontext_compare2(SH_TRANS, I_SECOND, I_FIRST);
    helper_fcontext_compare2(SH_TRANS, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_fcontext_compare2(SH_TRANS, I_NULL,  I_FIRST),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_compare2(SH_TRANS, I_FIRST, I_NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_compare2(SH_TRANS, I_NULL,  I_NULL),
                     SIGSEGV);
}

// Function semanage_fcontext_key_create

void helper_fcontext_key_create(level_t level) {
    semanage_fcontext_key_t *key = NULL;

    setup_handle(level);

    CU_ASSERT(semanage_fcontext_key_create(sh, "", SEMANAGE_FCONTEXT_ALL,
                                           &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_fcontext_key_free(key);

    key = NULL;

    CU_ASSERT(semanage_fcontext_key_create(sh, "testfcontext",
                                           SEMANAGE_FCONTEXT_ALL, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_fcontext_key_free(key);

    CU_ASSERT_SIGNAL(semanage_fcontext_key_create(sh, "testfcontext",
                                         SEMANAGE_FCONTEXT_ALL, NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_fcontext_key_create(void) {
    helper_fcontext_key_create(SH_NULL);
    helper_fcontext_key_create(SH_HANDLE);
    helper_fcontext_key_create(SH_CONNECT);
    helper_fcontext_key_create(SH_TRANS);
}

// Function semanage_fcontext_key_extract
// TODO: make semanage_fcontext_key_create return error

#define SK_NULL 1
#define SK_NEW 2
#define SK_INDEX 3
#define SK_KEY_NULL 4
void helper_fcontext_key_extract(level_t level, int mode) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_key_t *key;
    int result;
    
    setup_handle(level);

    switch (mode) {
        case SK_NULL:
            fcontext = NULL;
            break;
        case SK_NEW:
            fcontext = get_fcontext_new();
            break;
        case SK_INDEX:
            fcontext = get_fcontext_nth(0);
            break;
        case SK_KEY_NULL:
            fcontext = get_fcontext_nth(0);
            break;
        default:
            CU_FAIL_FATAL("Invalid mode\n");
    }

    if (mode == SK_KEY_NULL)
        result = semanage_fcontext_key_extract(sh, fcontext, NULL);
    else
        result = semanage_fcontext_key_extract(sh, fcontext, &key);

    CU_ASSERT(result >= 0);

    result = semanage_fcontext_compare(fcontext, key);

    CU_ASSERT(result == 0);

    semanage_fcontext_key_free(key);
    semanage_fcontext_free(fcontext);

    cleanup_handle(level);
}

void test_fcontext_key_extract(void) {
    CU_ASSERT_SIGNAL(helper_fcontext_key_extract(SH_CONNECT, SK_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_key_extract(SH_CONNECT, SK_KEY_NULL),
                     SIGSEGV);

    // FIXME
    //helper_fcontext_key_extract(SH_CONNECT, SK_NEW);
    CU_ASSERT_SIGNAL(helper_fcontext_key_extract(SH_CONNECT, SK_NEW), SIGSEGV);

    helper_fcontext_key_extract(SH_CONNECT, SK_INDEX);

    CU_ASSERT_SIGNAL(helper_fcontext_key_extract(SH_TRANS, SK_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_key_extract(SH_TRANS, SK_KEY_NULL),
                     SIGSEGV);

    // FIXME
    //helper_fcontext_key_extract(SH_TRANS, SK_NEW);
    CU_ASSERT_SIGNAL(helper_fcontext_key_extract(SH_TRANS, SK_NEW), SIGSEGV);

    helper_fcontext_key_extract(SH_TRANS, SK_INDEX);
}
#undef SK_NULL
#undef SK_NEW
#undef SK_INDEX
#undef SK_KEY_NULL

// Function semanage_fcontext_get_expr, semanage_fcontext_set_expr

void helper_fcontext_get_set_expr(level_t level, int fcontext_index,
                                  const char *name) {
    semanage_fcontext_t *fcontext;
    const char *new_name = NULL;
    
    setup_handle(level);

    fcontext = get_fcontext_nth(fcontext_index);

    CU_ASSERT(semanage_fcontext_set_expr(sh, fcontext, name) >= 0);

    new_name = semanage_fcontext_get_expr(fcontext);

    CU_ASSERT_PTR_NOT_NULL(new_name);
    CU_ASSERT_STRING_EQUAL(new_name, name);

    semanage_fcontext_free(fcontext);
    cleanup_handle(level);
}

void test_fcontext_get_set_expr(void) {
    CU_ASSERT_SIGNAL(helper_fcontext_get_set_expr(SH_CONNECT, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_get_set_expr(SH_CONNECT, I_NULL,
                                                  "testfcontext"), SIGSEGV);

    helper_fcontext_get_set_expr(SH_CONNECT, I_FIRST, "testfcontext");
    helper_fcontext_get_set_expr(SH_CONNECT, I_FIRST, "");
    helper_fcontext_get_set_expr(SH_CONNECT, I_SECOND, "testfcontext");
    helper_fcontext_get_set_expr(SH_CONNECT, I_SECOND, "");

    CU_ASSERT_SIGNAL(helper_fcontext_get_set_expr(SH_TRANS, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_fcontext_get_set_expr(SH_TRANS, I_NULL,
                                                  "testfcontext"), SIGSEGV);

    helper_fcontext_get_set_expr(SH_TRANS, I_FIRST, "testfcontext");
    helper_fcontext_get_set_expr(SH_TRANS, I_FIRST, "");
    helper_fcontext_get_set_expr(SH_TRANS, I_SECOND, "testfcontext");
    helper_fcontext_get_set_expr(SH_TRANS, I_SECOND, "");
}

// Function semanage_fcontext_get_type, semanage_fcontext_set_type

void helper_fcontext_get_set_type(int fcontext_index, int type) {
    semanage_fcontext_t *fcontext;
    int new_type = -1;
    
    setup_handle(SH_CONNECT);

    fcontext = get_fcontext_nth(fcontext_index);

    semanage_fcontext_set_type(fcontext, type);

    new_type = semanage_fcontext_get_type(fcontext);

    CU_ASSERT(new_type == type);

    semanage_fcontext_free(fcontext);
    cleanup_handle(SH_CONNECT);
}

void test_fcontext_get_set_type(void) {
    helper_fcontext_get_set_type(I_FIRST, SEMANAGE_FCONTEXT_ALL - 1);
    helper_fcontext_get_set_type(I_FIRST, SEMANAGE_FCONTEXT_ALL);
    helper_fcontext_get_set_type(I_FIRST, SEMANAGE_FCONTEXT_DIR);
    helper_fcontext_get_set_type(I_FIRST, SEMANAGE_FCONTEXT_PIPE);
    helper_fcontext_get_set_type(I_FIRST, SEMANAGE_FCONTEXT_PIPE + 1);

    helper_fcontext_get_set_type(I_SECOND, SEMANAGE_FCONTEXT_ALL);
}

// Function semanage_fcontext_get_type_str

void helper_fcontext_get_type_str(int type, const char *exp_str) {
    CU_ASSERT_STRING_EQUAL(semanage_fcontext_get_type_str(type), exp_str);
}

void test_fcontext_get_type_str(void) {
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_ALL,   "all files");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_REG,   "regular file");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_DIR,   "directory");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_CHAR,  "character device");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_BLOCK, "block device");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_SOCK,  "socket");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_LINK,  "symbolic link");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_PIPE,  "named pipe");

    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_ALL - 1,  "????");
    helper_fcontext_get_type_str(SEMANAGE_FCONTEXT_PIPE + 1, "????");
}

// Function semanage_fcontext_get_con, semanage_fcontext_set_con

void helper_fcontext_get_set_con(level_t level, int fcontext_index,
                                 const char *con_str) {
    semanage_fcontext_t *fcontext;
    semanage_context_t *con = NULL;
    semanage_context_t *new_con = NULL;
    
    setup_handle(level);

    fcontext = get_fcontext_nth(fcontext_index);

    if (con_str != NULL) {
        CU_ASSERT(semanage_context_from_string(sh, con_str, &con) >= 0);
        CU_ASSERT_PTR_NOT_NULL(con);
    }
    else {
        con = NULL;
    }

    // set con
    CU_ASSERT(semanage_fcontext_set_con(sh, fcontext, con) >= 0);

    // get con
    new_con = semanage_fcontext_get_con(fcontext);

    if (con_str != NULL) {
        CU_ASSERT_CONTEXT_EQUAL(con, new_con);
    }
    else {
        CU_ASSERT_PTR_NULL(new_con);
    }

    semanage_fcontext_free(fcontext);
    cleanup_handle(level);
}

void test_fcontext_get_set_con(void) {
    CU_ASSERT_SIGNAL(helper_fcontext_get_set_con(SH_CONNECT, I_NULL,
                                           "user_u:role_r:type_t:s0"), SIGSEGV);
    helper_fcontext_get_set_con(SH_CONNECT, I_FIRST, NULL);

    helper_fcontext_get_set_con(SH_CONNECT, I_FIRST, "user_u:role_r:type_t:s0");
    helper_fcontext_get_set_con(SH_CONNECT, I_SECOND,
                                "user_u:role_r:type_t:s0");

    helper_fcontext_get_set_con(SH_TRANS, I_FIRST, NULL);
    CU_ASSERT_SIGNAL(helper_fcontext_get_set_con(SH_TRANS, I_NULL,
                                           "user_u:role_r:type_t:s0"), SIGSEGV);

    helper_fcontext_get_set_con(SH_TRANS, I_FIRST,  "user_u:role_r:type_t:s0");
    helper_fcontext_get_set_con(SH_TRANS, I_SECOND, "user_u:role_r:type_t:s0");
}

// Function semanage_fcontext_create

void helper_fcontext_create(level_t level) {
    semanage_fcontext_t *fcontext;
    
    setup_handle(level);

    CU_ASSERT(semanage_fcontext_create(sh, &fcontext) >= 0);

    CU_ASSERT_PTR_NULL(semanage_fcontext_get_expr(fcontext));
    CU_ASSERT(semanage_fcontext_get_type(fcontext) == SEMANAGE_FCONTEXT_ALL);
    CU_ASSERT_PTR_NULL(semanage_fcontext_get_con(fcontext));

    semanage_fcontext_free(fcontext);
    cleanup_handle(level);
}

void test_fcontext_create(void) {
    helper_fcontext_create(SH_NULL);
    helper_fcontext_create(SH_HANDLE);
    helper_fcontext_create(SH_CONNECT);
    helper_fcontext_create(SH_TRANS);
}

// Function semanage_fcontext_clone
// TODO: error gotos

void helper_fcontext_clone(level_t level, int fcontext_index) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_t *fcontext_clone;
    const char *expr;
    const char *expr_clone;
    int type;
    int type_clone;
    semanage_context_t *con;
    semanage_context_t *con_clone;
    
    setup_handle(level);

    fcontext = get_fcontext_nth(fcontext_index);

    // clone
    CU_ASSERT(semanage_fcontext_clone(sh, fcontext, &fcontext_clone) >= 0);

    // compare expr
    expr = semanage_fcontext_get_expr(fcontext);
    expr_clone = semanage_fcontext_get_expr(fcontext_clone);

    CU_ASSERT_STRING_EQUAL(expr, expr_clone);

    // compare type
    type = semanage_fcontext_get_type(fcontext);
    type_clone = semanage_fcontext_get_type(fcontext_clone);

    CU_ASSERT_EQUAL(type, type_clone);

    // compare context
    con = semanage_fcontext_get_con(fcontext);
    con_clone = semanage_fcontext_get_con(fcontext_clone);

    CU_ASSERT_CONTEXT_EQUAL(con, con_clone);

    semanage_fcontext_free(fcontext);
    semanage_fcontext_free(fcontext_clone);
    cleanup_handle(level);
}

void test_fcontext_clone(void) {
    CU_ASSERT_SIGNAL(helper_fcontext_clone(SH_CONNECT, I_NULL), SIGSEGV);
    helper_fcontext_clone(SH_CONNECT, I_FIRST);
    helper_fcontext_clone(SH_CONNECT, I_SECOND);

    CU_ASSERT_SIGNAL(helper_fcontext_clone(SH_TRANS, I_NULL), SIGSEGV);
    helper_fcontext_clone(SH_TRANS, I_FIRST);
    helper_fcontext_clone(SH_TRANS, I_SECOND);
}

// Function semanage_fcontext_query

void helper_fcontext_query(level_t level, const char *fcontext_expr,
                           int fcontext_type, int exp_result) {
    semanage_fcontext_key_t *key;
    // some non-null value
    semanage_fcontext_t *response = (void *) 42;
    const char *expr;
    int result;

    setup_handle(level);

    key = get_fcontext_key_from_str(fcontext_expr, fcontext_type);

    result = semanage_fcontext_query(sh, key, &response);

    if (exp_result >= 0) {
        CU_ASSERT(result >= 0);

        expr = semanage_fcontext_get_expr(response);

        CU_ASSERT_STRING_EQUAL(expr, fcontext_expr);
    }
    else {
        CU_ASSERT(result < 0);
        CU_ASSERT(response == (void *) 42);
    }

    cleanup_handle(level);
}

void test_fcontext_query(void) {
    CU_ASSERT_SIGNAL(helper_fcontext_query(SH_CONNECT, NULL, 0, -1), SIGSEGV);
    helper_fcontext_query(SH_CONNECT, FCONTEXT_NONEXISTENT_EXPR,
                          FCONTEXT_NONEXISTENT_TYPE, -1);
    // different type
    helper_fcontext_query(SH_CONNECT, FCONTEXT2_EXPR, FCONTEXT_TYPE, -1);

    helper_fcontext_query(SH_CONNECT, FCONTEXT_EXPR,  FCONTEXT_TYPE,  1);
    helper_fcontext_query(SH_CONNECT, FCONTEXT2_EXPR, FCONTEXT2_TYPE, 1);

    CU_ASSERT_SIGNAL(helper_fcontext_query(SH_TRANS, NULL, 0, -1), SIGSEGV);
    helper_fcontext_query(SH_TRANS, FCONTEXT_NONEXISTENT_EXPR,
                          FCONTEXT_NONEXISTENT_TYPE, -1);
    // different type
    helper_fcontext_query(SH_TRANS, FCONTEXT2_EXPR, FCONTEXT_TYPE, -1);

    helper_fcontext_query(SH_TRANS, FCONTEXT_EXPR,  FCONTEXT_TYPE,  1);
    helper_fcontext_query(SH_TRANS, FCONTEXT2_EXPR, FCONTEXT2_TYPE, 1);
}

// Function semanage_fcontext_exists

void helper_fcontext_exists(level_t level, char * fcontext_expr,
                            int fcontext_type, int exp_response) {
    semanage_fcontext_key_t *key;
    int response;

    setup_handle(level);

    key = get_fcontext_key_from_str(fcontext_expr, fcontext_type);

    CU_ASSERT(semanage_fcontext_exists(sh, key, &response) >= 0);
    CU_ASSERT(response == exp_response);

    CU_ASSERT_SIGNAL(semanage_fcontext_exists(sh, key, NULL), SIGSEGV);

    semanage_fcontext_key_free(key);

    cleanup_handle(level);
}

void test_fcontext_exists(void) {
    CU_ASSERT_SIGNAL(helper_fcontext_exists(SH_CONNECT, NULL, 0, 0), SIGSEGV);
    helper_fcontext_exists(SH_CONNECT, FCONTEXT_NONEXISTENT_EXPR,
                           FCONTEXT_NONEXISTENT_TYPE, 0);
    // different type
    helper_fcontext_exists(SH_CONNECT, FCONTEXT2_EXPR, FCONTEXT_TYPE, 0);

    helper_fcontext_exists(SH_CONNECT, FCONTEXT_EXPR,  FCONTEXT_TYPE,  1);
    helper_fcontext_exists(SH_CONNECT, FCONTEXT2_EXPR, FCONTEXT2_TYPE, 1);

    CU_ASSERT_SIGNAL(helper_fcontext_exists(SH_TRANS, NULL, 0, 0), SIGSEGV);
    helper_fcontext_exists(SH_TRANS, FCONTEXT_NONEXISTENT_EXPR,
                           FCONTEXT_NONEXISTENT_TYPE, 0);
    // different type
    helper_fcontext_exists(SH_TRANS, FCONTEXT2_EXPR, FCONTEXT_TYPE,  0);

    helper_fcontext_exists(SH_TRANS, FCONTEXT_EXPR,  FCONTEXT_TYPE,  1);
    helper_fcontext_exists(SH_TRANS, FCONTEXT2_EXPR, FCONTEXT2_TYPE, 1);
}

// Function semanage_fcontext_count

void test_fcontext_count(void) {
    unsigned int response;

    CU_ASSERT_SIGNAL(semanage_fcontext_count(NULL, &response), SIGSEGV);

    //handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_fcontext_count(sh, &response) < 0);

    CU_ASSERT(semanage_fcontext_count(sh, NULL) < 0);
    
    cleanup_handle(SH_HANDLE);
    
    //connect
    response = 0;
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_fcontext_count(sh, &response) >= 0);
    CU_ASSERT(response == FCONTEXTS_COUNT);

    CU_ASSERT_SIGNAL(semanage_fcontext_count(sh, NULL), SIGSEGV);

    cleanup_handle(SH_CONNECT);

    //trans
    response = 0;
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_fcontext_count(sh, &response) >= 0);
    CU_ASSERT(response == FCONTEXTS_COUNT);

    CU_ASSERT_SIGNAL(semanage_fcontext_count(sh, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_iterate

unsigned int counter_fcontext_iterate = 0;

int handler_fcontext_iterate(const semanage_fcontext_t *record, void *varg) {
    counter_fcontext_iterate++;
    return 0;
}

void helper_fcontext_iterate_invalid(void) {
    CU_ASSERT_SIGNAL(semanage_fcontext_iterate(NULL, &handler_fcontext_iterate,
                                               NULL), SIGSEGV);

    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_fcontext_iterate(sh, &handler_fcontext_iterate,
                                        NULL) < 0);

    CU_ASSERT(semanage_fcontext_iterate(sh, NULL, NULL) < 0);

    cleanup_handle(SH_HANDLE);
}

void helper_fcontext_iterate(level_t level) {
    setup_handle(level);

    counter_fcontext_iterate = 0;

    CU_ASSERT(semanage_fcontext_iterate(sh, &handler_fcontext_iterate,
                                        NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate == FCONTEXTS_COUNT);

    CU_ASSERT_SIGNAL(semanage_fcontext_iterate(sh, NULL, NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_fcontext_iterate(void) {
    helper_fcontext_iterate_invalid();
    helper_fcontext_iterate(SH_CONNECT);
    helper_fcontext_iterate(SH_TRANS);
}

// Function semanage_fcontext_list

void helper_fcontext_list_invalid(void) {
    semanage_fcontext_t **records;
    unsigned int count;

    CU_ASSERT_SIGNAL(semanage_fcontext_list(NULL, &records, &count), SIGSEGV);

    setup_handle(SH_HANDLE);
    
    CU_ASSERT(semanage_fcontext_list(sh, &records, &count) < 0);

    CU_ASSERT(semanage_fcontext_list(sh, NULL, &count) < 0);
    CU_ASSERT(semanage_fcontext_list(sh, &records, NULL) < 0);

    cleanup_handle(SH_HANDLE);
}

void helper_fcontext_list(level_t level) {
    semanage_fcontext_t **records;
    unsigned int count;

    setup_handle(level);
    
    CU_ASSERT(semanage_fcontext_list(sh, &records, &count) >= 0);
    CU_ASSERT(count == FCONTEXTS_COUNT);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    for (unsigned int i = 0; i < count; i++) {
        semanage_fcontext_free(records[i]);
    }

    CU_ASSERT_SIGNAL(semanage_fcontext_list(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_fcontext_list(sh, &records, NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_fcontext_list(void) {
    helper_fcontext_list_invalid();
    helper_fcontext_list(SH_CONNECT);
    helper_fcontext_list(SH_TRANS);
}

// Function semanage_fcontext_modify_local, semanage_fcontext_del_local

void helper_fcontext_modify_del_local(level_t level, int fcontext_index,
                                      const char *con_str, int exp_result) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_t *fcontext_local;
    semanage_fcontext_key_t *key = NULL;
    semanage_context_t *con = NULL;
    int result;

    setup_handle(level);

    fcontext = get_fcontext_nth(fcontext_index);

    CU_ASSERT(semanage_fcontext_key_extract(sh, fcontext, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    if (con_str != NULL) {
        CU_ASSERT(semanage_context_from_string(sh, con_str, &con) >= 0);
        CU_ASSERT_PTR_NOT_NULL(con);
    }
    else {
        con = NULL;
    }

    CU_ASSERT(semanage_fcontext_set_con(sh, fcontext, con) >= 0);

    result = semanage_fcontext_modify_local(sh, key, fcontext);

    if (exp_result < 0) {
        CU_ASSERT(result < 0);
    }
    else {
        CU_ASSERT(result >= 0);

        if (level == SH_TRANS) {
            commit();
            begin_transaction();
        }

        CU_ASSERT(semanage_fcontext_query_local(sh, key, &fcontext_local) >= 0);
        CU_ASSERT(semanage_fcontext_compare2(fcontext_local, fcontext) == 0);

        CU_ASSERT(semanage_fcontext_del_local(sh, key) >= 0);

        CU_ASSERT(semanage_fcontext_query_local(sh, key, &fcontext_local) < 0);
    }

    semanage_fcontext_key_free(key);
    semanage_fcontext_free(fcontext);

    cleanup_handle(level);
}

void test_fcontext_modify_del_local(void) {
    helper_fcontext_modify_del_local(SH_CONNECT, I_FIRST,
                                     "system_u:object_r:tmp_t:s0", -1);
    helper_fcontext_modify_del_local(SH_CONNECT, I_SECOND,
                                     "system_u:object_r:tmp_t:s0", -1);

    helper_fcontext_modify_del_local(SH_TRANS, I_FIRST,
                                     "system_u:object_r:tmp_t:s0", 1);
    helper_fcontext_modify_del_local(SH_TRANS, I_SECOND,
                                     "system_u:object_r:tmp_t:s0", 1);
}

// Function semanage_fcontext_query_local

void test_fcontext_query_local(void) {
    semanage_fcontext_key_t *key = NULL;
    semanage_fcontext_t *response = NULL;

    // connect
    setup_handle(SH_CONNECT);

    key = get_fcontext_key_nth(I_FIRST);

    CU_ASSERT(semanage_fcontext_query_local(sh, key, &response) < 0);
    CU_ASSERT_PTR_NULL(response);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    key = get_fcontext_key_nth(I_FIRST);

    CU_ASSERT(semanage_fcontext_query_local(sh, key, &response) < 0);
    CU_ASSERT_PTR_NULL(response);

    add_local_fcontext(I_FIRST);

    CU_ASSERT(semanage_fcontext_query_local(sh, key, &response) >= 0);
    CU_ASSERT_PTR_NOT_NULL(response);

    semanage_fcontext_key_free(key);
    key = get_fcontext_key_nth(I_SECOND);

    add_local_fcontext(I_SECOND);

    CU_ASSERT(semanage_fcontext_query_local(sh, key, &response) >= 0);
    CU_ASSERT_PTR_NOT_NULL(response);

    delete_local_fcontext(I_FIRST);
    delete_local_fcontext(I_SECOND);
    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_exists_local

void test_fcontext_exists_local(void) {
    int response = -1;
    semanage_fcontext_key_t *key;
    setup_handle(SH_TRANS);

    key = get_fcontext_key_nth(I_FIRST);

    CU_ASSERT(semanage_fcontext_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 0);

    add_local_fcontext(I_FIRST);
    response = -1;

    CU_ASSERT(semanage_fcontext_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 1);

    delete_local_fcontext(I_FIRST);
    response = -1;

    CU_ASSERT(semanage_fcontext_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 0);

    response = -1;

    CU_ASSERT(semanage_fcontext_exists_local(sh, NULL, &response) >= 0);
    CU_ASSERT(response == 0);

    CU_ASSERT_SIGNAL(semanage_fcontext_exists_local(sh, key, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_count_local

void test_fcontext_count_local(void) {
    unsigned int response;

    // null
    CU_ASSERT_SIGNAL(semanage_fcontext_count_local(NULL, &response), SIGSEGV);

    // handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) < 0);

    cleanup_handle(SH_HANDLE);
    
    // connect
    setup_handle(SH_CONNECT);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 0);

    CU_ASSERT_SIGNAL(semanage_fcontext_count_local(sh, NULL), SIGSEGV);

    cleanup_handle(SH_CONNECT);
    
    // transaction
    setup_handle(SH_TRANS);
    
    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 0);

    add_local_fcontext(I_FIRST);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 1);

    add_local_fcontext(I_SECOND);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 2);

    delete_local_fcontext(I_SECOND);

    CU_ASSERT(semanage_fcontext_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 1);

    delete_local_fcontext(I_FIRST);

    CU_ASSERT_SIGNAL(semanage_fcontext_count_local(sh, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_iterate_local

unsigned int counter_fcontext_iterate_local = 0;

int handler_fcontext_iterate_local(const semanage_fcontext_t *record,
                                   void *varg) {
    counter_fcontext_iterate_local++;
    return 0;
}

void test_fcontext_iterate_local(void) {
    // null
    CU_ASSERT_SIGNAL(semanage_fcontext_iterate_local(NULL,
                               &handler_fcontext_iterate_local, NULL), SIGSEGV);

    // handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                    &handler_fcontext_iterate_local, NULL) < 0);

    CU_ASSERT(semanage_fcontext_iterate_local(sh, NULL, NULL) < 0);

    cleanup_handle(SH_HANDLE);
    
    // connect
    setup_handle(SH_CONNECT);

    counter_fcontext_iterate_local = 0;
    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                   &handler_fcontext_iterate_local, NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate_local == 0);

    // FIXME
    CU_ASSERT(semanage_fcontext_iterate_local(sh, NULL, NULL) >= 0);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    counter_fcontext_iterate_local = 0;
    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                   &handler_fcontext_iterate_local, NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate_local == 0);

    add_local_fcontext(I_FIRST);

    counter_fcontext_iterate_local = 0;
    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                   &handler_fcontext_iterate_local, NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate_local == 1);

    add_local_fcontext(I_SECOND);

    counter_fcontext_iterate_local = 0;
    CU_ASSERT(semanage_fcontext_iterate_local(sh,
                                   &handler_fcontext_iterate_local, NULL) >= 0);
    CU_ASSERT(counter_fcontext_iterate_local == 2);

    CU_ASSERT_SIGNAL(semanage_fcontext_iterate_local(sh, NULL, NULL), SIGSEGV);

    delete_local_fcontext(I_FIRST);
    delete_local_fcontext(I_SECOND);
    cleanup_handle(SH_TRANS);
}

// Function semanage_fcontext_list_local

void test_fcontext_list_local(void) {
    semanage_fcontext_t **records;
    unsigned int count;

    // null
    CU_ASSERT_SIGNAL(semanage_fcontext_list_local(NULL, &records, &count),
                     SIGSEGV);

    // handle
    setup_handle(SH_HANDLE);
    
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) < 0);

    CU_ASSERT(semanage_fcontext_list_local(sh, NULL, &count) < 0);
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, NULL) < 0);

    cleanup_handle(SH_HANDLE);

    // connect
    setup_handle(SH_CONNECT);
    
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 0);

    CU_ASSERT_SIGNAL(semanage_fcontext_list_local(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_fcontext_list_local(sh, &records, NULL), SIGSEGV);

    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 0);

    add_local_fcontext(I_FIRST);
    
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 1);
    CU_ASSERT_PTR_NOT_NULL(records[0]);

    add_local_fcontext(I_SECOND);
    
    CU_ASSERT(semanage_fcontext_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 2);
    CU_ASSERT_PTR_NOT_NULL(records[0]);
    CU_ASSERT_PTR_NOT_NULL(records[1]);

    CU_ASSERT_SIGNAL(semanage_fcontext_list_local(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_fcontext_list_local(sh, &records, NULL), SIGSEGV);

    delete_local_fcontext(I_FIRST);
    delete_local_fcontext(I_SECOND);
    cleanup_handle(SH_TRANS);
}
