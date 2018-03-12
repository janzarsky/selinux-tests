#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <semanage/semanage.h>
#include <semanage/handle.h>
#include <selinux/selinux.h>
#include <CUnit/Basic.h>

#include "test_seuser.h"
#include "functions.h"

// TODO: test function semanage_seuser_compare2_qsort
// TODO: seuser_print: error gotos
// TODO: seuser_parse: error gotos
// TODO: seuser_file_dbase_init: make dbase_file_init return error
// TODO: semanage_seuser_audit: lines 88-90, 93-94
// TODO: semanage_seuser_audit: make audit_open return error
// TODO: validate_handler: error gotos

extern semanage_handle_t *sh;

#define SEUSER_NONEXISTENT "nonuser"
#define SEUSER_DEFAULT "__default__"

char *SEUSER;
char *SEUSER_SENAME;
char *SEUSER_MLSRANGE;
char *SEUSER2;
char *SEUSER2_SENAME;
char *SEUSER2_MLSRANGE;
unsigned int SEUSERS_COUNT;
char *SEUSERS;

int prepare_paths(char **p1, char **p2, char **p3) {
#define POLICY_STORE1 "/var/lib/selinux/"
#define POLICY_STORE2 "/etc/selinux/"
#define PATH1 "/active/seusers"
#define PATH2 "/seusers"

    char *policy_type;

    selinux_getpolicytype(&policy_type);

    *p1 = malloc(strlen(POLICY_STORE1) + strlen(policy_type)
                           + strlen(PATH1) + 1);

    if (*p1 == NULL)
        return -1;

    *p2 = malloc(strlen(POLICY_STORE2) + strlen(policy_type)
                           + strlen(PATH1) + 1);
    
    if (*p2 == NULL) {
        free(*p1);
        return -1;
    }

    *p3 = malloc(strlen(POLICY_STORE2) + strlen(policy_type)
                           + strlen(PATH2) + 1);
    
    if (*p3 == NULL) {
        free(*p1);
        free(*p2);
        return -1;
    }

    strcpy(*p1, POLICY_STORE1);
    strcat(*p1, policy_type);
    strcat(*p1, PATH1);

    strcpy(*p2, POLICY_STORE2);
    strcat(*p2, policy_type);
    strcat(*p2, PATH1);

    strcpy(*p3, POLICY_STORE2);
    strcat(*p3, policy_type);
    strcat(*p3, PATH2);

    free(policy_type);

    return 0;

#undef POLICY_STORE1
#undef POLICY_STORE2
#undef PATH1
#undef PATH2
}

int seuser_test_init(void)
{
    char *seusers_path1;
    char *seusers_path2;
    char *seusers_path3;
    FILE *f;

    if (prepare_paths(&seusers_path1, &seusers_path2, &seusers_path3) < 0) {
        return 1;
    }

    f = fopen(seusers_path1, "r");

    if (f == NULL) {
        f = fopen(seusers_path2, "r");

        if (f == NULL) {
            f = fopen(seusers_path3, "r");
            
            if (f == NULL) {
                fprintf(stderr, "Unable to open seusers file\n");
                return 1;
            }
        }
    }

    char line[512];
    unsigned int line_counter = 0;
    char *p;
    char seusers_tmp[512];

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n')
            continue;

        p = strtok(line, ":");
        strcat(seusers_tmp, " ");
        strcat(seusers_tmp, p);

        if (line_counter == 0) {
            SEUSER = strdup(p);

            p = strtok(NULL, ":");
            SEUSER_SENAME = strdup(p);

            p = strtok(NULL, ":");
            SEUSER_MLSRANGE = strdup(p);
        }
        else if (line_counter == 1) {
            SEUSER2 = strdup(p);

            p = strtok(NULL, ":");
            SEUSER2_SENAME = strdup(p);

            p = strtok(NULL, ":");
            SEUSER2_MLSRANGE = strdup(p);
        }

        line_counter++;
    }

    SEUSERS = strdup(seusers_tmp);
    
    SEUSERS_COUNT = line_counter;

    if (SEUSERS_COUNT == 0) {
        fprintf(stderr, "SEUSERS_COUNT is zero\n");
        return 1;
    }

    // delete local seusers (written because of RHEL 6)
    semanage_seuser_key_t *key = NULL;
    semanage_seuser_t **records = NULL;
    unsigned int count = 0;
    
    if ((sh = semanage_handle_create()) == NULL) {
        fprintf(stderr, "Error deleting local seusers\n");
        return 1;
    }

    if (semanage_connect(sh) < 0) {
        fprintf(stderr, "Error deleting local seusers\n");
        return 1;
    }

    if (semanage_begin_transaction(sh) < 0) {
        fprintf(stderr, "Error deleting local seusers\n");
        return 1;
    }

    if (semanage_seuser_list_local(sh, &records, &count) < 0) {
        fprintf(stderr, "Error deleting local seusers\n");
        return 1;
    }

    for (unsigned int i = 0; i < count; i++) {
        if (semanage_seuser_key_extract(sh, records[i], &key) < 0) {
            fprintf(stderr, "Error deleting local seusers\n");
            return 1;
        }

        if (semanage_seuser_del_local(sh, key) < 0) {
            fprintf(stderr, "Error deleting local seusers\n");
            return 1;
        }
    }

    if (semanage_seuser_count_local(sh, &count) < 0) {
        fprintf(stderr, "Error deleting local seusers\n");
        return 1;
    }
    
    if (count != 0) {
        fprintf(stderr, "Error deleting local seusers\n");
        return 1;
    }
    
    if (semanage_commit(sh) < 0) {
        fprintf(stderr, "Error deleting local seusers\n");
        return 1;
    }

    if (semanage_disconnect(sh) < 0) {
        fprintf(stderr, "Error deleting local seusers\n");
        return 1;
    }

    semanage_handle_destroy(sh);
    free(seusers_path1);
    free(seusers_path2);
    free(seusers_path3);

	return 0;
}

int seuser_test_cleanup(void)
{
	return 0;
}

int seuser_add_tests(CU_pSuite suite)
{
    CU_add_test(suite, "seuser_key_create",         test_seuser_key_create);
    CU_add_test(suite, "seuser_key_extract",        test_seuser_key_extract);
    CU_add_test(suite, "seuser_compare",            test_seuser_compare);
    CU_add_test(suite, "seuser_compare2",           test_seuser_compare2);
    CU_add_test(suite, "seuser_get_set_name",       test_seuser_get_set_name);
    CU_add_test(suite, "seuser_get_set_sename",     test_seuser_get_set_sename);
    CU_add_test(suite, "seuser_get_set_mlsrange", test_seuser_get_set_mlsrange);
    CU_add_test(suite, "seuser_clone",              test_seuser_clone);
    CU_add_test(suite, "seuser_create",             test_seuser_create);

    CU_add_test(suite, "seuser_query",              test_seuser_query);
    CU_add_test(suite, "seuser_exists",             test_seuser_exists);
    CU_add_test(suite, "seuser_count",              test_seuser_count);
    CU_add_test(suite, "seuser_iterate",            test_seuser_iterate);
    CU_add_test(suite, "seuser_list",               test_seuser_list);

    CU_add_test(suite, "seuser_modify_del_local", test_seuser_modify_del_local);
    CU_add_test(suite, "seuser_query_local",        test_seuser_query_local);
    CU_add_test(suite, "seuser_exists_local",       test_seuser_exists_local);
    CU_add_test(suite, "seuser_count_local",        test_seuser_count_local);
    CU_add_test(suite, "seuser_iterate_local",      test_seuser_iterate_local);
    CU_add_test(suite, "seuser_list_local",         test_seuser_list_local);
     
	return 0;
}

// Function seuser_key_create

void helper_seuser_key_create(level_t level) {
    semanage_seuser_key_t *key = NULL;

    setup_handle(level);

    // name == ""
    key = NULL;

    CU_ASSERT(semanage_seuser_key_create(sh, "", &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_seuser_key_free(key);

    // name == "testuser"
    key = NULL;

    CU_ASSERT(semanage_seuser_key_create(sh, "testuser", &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    semanage_seuser_key_free(key);

    // &key == NULL
    CU_ASSERT_SIGNAL(semanage_seuser_key_create(sh, "testuser", NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_seuser_key_create(void) {
    helper_seuser_key_create(SH_CONNECT);
    helper_seuser_key_create(SH_TRANS);
}

// Function seuser_key_extract
// TODO: make semanage_seuser_key_create return error

#define SK_NULL 1
#define SK_NEW 2
#define SK_INDEX 3
#define SK_KEY_NULL 4
void helper_seuser_key_extract(level_t level, int mode) {
    semanage_seuser_t *seuser;
    semanage_seuser_key_t *key;
    int result;
    
    setup_handle(level);

    switch (mode) {
        case SK_NULL:
            seuser = NULL;
            break;
        case SK_NEW:
            seuser = get_seuser_new();
            break;
        case SK_INDEX:
            seuser = get_seuser_nth(0);
            break;
        case SK_KEY_NULL:
            seuser = get_seuser_nth(0);
            break;
        default:
            CU_FAIL_FATAL("Invalid mode\n");
    }

    if (mode == SK_KEY_NULL)
        result = semanage_seuser_key_extract(sh, seuser, NULL);
    else
        result = semanage_seuser_key_extract(sh, seuser, &key);

    CU_ASSERT(result >= 0);

    result = semanage_seuser_compare(seuser, key);

    CU_ASSERT(result == 0);

    semanage_seuser_key_free(key);
    semanage_seuser_free(seuser);

    cleanup_handle(level);
}

void test_seuser_key_extract(void) {
    CU_ASSERT_SIGNAL(helper_seuser_key_extract(SH_CONNECT, SK_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_key_extract(SH_CONNECT, SK_KEY_NULL),
                     SIGSEGV);

    // FIXME
    //helper_seuser_key_extract(SH_CONNECT, SK_NEW);
    CU_ASSERT_SIGNAL(helper_seuser_key_extract(SH_CONNECT, SK_NEW), SIGSEGV);

    helper_seuser_key_extract(SH_CONNECT, SK_INDEX);

    CU_ASSERT_SIGNAL(helper_seuser_key_extract(SH_TRANS, SK_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_key_extract(SH_TRANS, SK_KEY_NULL), SIGSEGV);

    // FIXME
    //helper_seuser_key_extract(SH_TRANS, SK_NEW);
    CU_ASSERT_SIGNAL(helper_seuser_key_extract(SH_TRANS, SK_NEW), SIGSEGV);

    helper_seuser_key_extract(SH_TRANS, SK_INDEX);
}
#undef SK_NULL
#undef SK_NEW
#undef SK_INDEX
#undef SK_KEY_NULL

// Function seuser_compare

void helper_seuser_compare(level_t level, int seuser_index1,
                           int seuser_index2) {
    semanage_seuser_t *seuser;
    semanage_seuser_key_t *key;
    int result;

    setup_handle(level);

    seuser = get_seuser_nth(seuser_index1);

    key = get_seuser_key_nth(seuser_index2);

    result = semanage_seuser_compare(seuser, key);

    if (seuser_index1 == seuser_index2) {
        CU_ASSERT(result == 0);
    }
    else {
        CU_ASSERT(result != 0);
    }

    semanage_seuser_free(seuser);
    semanage_seuser_key_free(key);
    cleanup_handle(level);
}

void test_seuser_compare(void) {
    helper_seuser_compare(SH_CONNECT, I_FIRST,  I_FIRST);
    helper_seuser_compare(SH_CONNECT, I_FIRST,  I_SECOND);
    helper_seuser_compare(SH_CONNECT, I_SECOND, I_FIRST);
    helper_seuser_compare(SH_CONNECT, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_seuser_compare(SH_CONNECT, I_NULL,  I_FIRST),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_compare(SH_CONNECT, I_FIRST, I_NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_compare(SH_CONNECT, I_NULL,  I_NULL),
                     SIGSEGV);

    helper_seuser_compare(SH_TRANS, I_FIRST,  I_FIRST);
    helper_seuser_compare(SH_TRANS, I_FIRST,  I_SECOND);
    helper_seuser_compare(SH_TRANS, I_SECOND, I_FIRST);
    helper_seuser_compare(SH_TRANS, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_seuser_compare(SH_TRANS, I_NULL, I_FIRST), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_compare(SH_TRANS, I_FIRST, I_NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_compare(SH_TRANS, I_NULL, I_NULL), SIGSEGV);
}

// Function seuser_compare2

void helper_seuser_compare2(level_t level, int seuser_index1,
                            int seuser_index2) {
    semanage_seuser_t *seuser1;
    semanage_seuser_t *seuser2;
    int result;

    setup_handle(level);

    seuser1 = get_seuser_nth(seuser_index1);
    seuser2 = get_seuser_nth(seuser_index2);

    result = semanage_seuser_compare2(seuser1, seuser2);

    if (seuser_index1 == seuser_index2) {
        CU_ASSERT(result == 0);
    }
    else {
        CU_ASSERT(result != 0);
    }

    semanage_seuser_free(seuser1);
    semanage_seuser_free(seuser2);

    cleanup_handle(level);
}

void test_seuser_compare2(void) {
    helper_seuser_compare2(SH_CONNECT, I_FIRST,  I_FIRST);
    helper_seuser_compare2(SH_CONNECT, I_FIRST,  I_SECOND);
    helper_seuser_compare2(SH_CONNECT, I_SECOND, I_FIRST);
    helper_seuser_compare2(SH_CONNECT, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_seuser_compare2(SH_CONNECT, I_NULL,  I_FIRST),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_compare2(SH_CONNECT, I_FIRST, I_NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_compare2(SH_CONNECT, I_NULL,  I_NULL),
                     SIGSEGV);

    helper_seuser_compare2(SH_TRANS, I_FIRST,  I_FIRST);
    helper_seuser_compare2(SH_TRANS, I_FIRST,  I_SECOND);
    helper_seuser_compare2(SH_TRANS, I_SECOND, I_FIRST);
    helper_seuser_compare2(SH_TRANS, I_SECOND, I_SECOND);

    CU_ASSERT_SIGNAL(helper_seuser_compare2(SH_TRANS, I_NULL, I_FIRST),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_compare2(SH_TRANS, I_FIRST, I_NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_compare2(SH_TRANS, I_NULL, I_NULL),
                     SIGSEGV);
}

// Function seuser_get_name, seuser_set_name

void helper_seuser_get_set_name(level_t level, int seuser_index,
                                const char *name) {
    semanage_seuser_t *user;
    const char *new_name = NULL;
    
    setup_handle(level);

    user = get_seuser_nth(seuser_index);

    CU_ASSERT(semanage_seuser_set_name(sh, user, name) >= 0);

    new_name = semanage_seuser_get_name(user);

    CU_ASSERT_PTR_NOT_NULL(new_name);
    CU_ASSERT_STRING_EQUAL(new_name, name);

    semanage_seuser_free(user);
    cleanup_handle(level);
}

void test_seuser_get_set_name(void) {
    CU_ASSERT_SIGNAL(helper_seuser_get_set_name(SH_CONNECT, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_get_set_name(SH_CONNECT, I_NULL, "testuser"),
                     SIGSEGV);

    helper_seuser_get_set_name(SH_CONNECT, I_FIRST, "testseuser");
    helper_seuser_get_set_name(SH_CONNECT, I_FIRST, "");
    helper_seuser_get_set_name(SH_CONNECT, I_SECOND, "testseuser");
    helper_seuser_get_set_name(SH_CONNECT, I_SECOND, "");

    CU_ASSERT_SIGNAL(helper_seuser_get_set_name(SH_TRANS, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_get_set_name(SH_TRANS, I_NULL, "testuser"),
                     SIGSEGV);

    helper_seuser_get_set_name(SH_TRANS, I_FIRST, "testseuser");
    helper_seuser_get_set_name(SH_TRANS, I_FIRST, "");
    helper_seuser_get_set_name(SH_TRANS, I_SECOND, "testseuser");
    helper_seuser_get_set_name(SH_TRANS, I_SECOND, "");
}

// Function seuser_get_sename, seuser_set_sename

void helper_seuser_get_set_sename(level_t level, int seuser_index,
                                  const char *sename) {
    semanage_seuser_t *user;
    const char *old_sename = NULL;
    const char *new_sename = NULL;
    
    setup_handle(level);

    user = get_seuser_nth(seuser_index);

    old_sename = semanage_seuser_get_sename(user);

    CU_ASSERT_PTR_NOT_NULL(old_sename);

    CU_ASSERT(semanage_seuser_set_sename(sh, user, sename) >= 0);

    new_sename = semanage_seuser_get_sename(user);

    CU_ASSERT_PTR_NOT_NULL(new_sename);
    CU_ASSERT_STRING_EQUAL(new_sename, sename);

    semanage_seuser_free(user);
    cleanup_handle(level);
}

void test_seuser_get_set_sename(void) {
    CU_ASSERT_SIGNAL(helper_seuser_get_set_sename(SH_CONNECT, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_get_set_sename(SH_CONNECT, I_NULL,
                                                  "testseuser"), SIGSEGV);

    helper_seuser_get_set_sename(SH_CONNECT, I_FIRST,  "testseuser");
    helper_seuser_get_set_sename(SH_CONNECT, I_FIRST,  "");
    helper_seuser_get_set_sename(SH_CONNECT, I_SECOND, "testseuser");
    helper_seuser_get_set_sename(SH_CONNECT, I_SECOND, "");

    CU_ASSERT_SIGNAL(helper_seuser_get_set_sename(SH_TRANS, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_get_set_sename(SH_TRANS, I_NULL,
                                                  "testseuser"), SIGSEGV);

    helper_seuser_get_set_sename(SH_TRANS, I_FIRST,  "testseuser");
    helper_seuser_get_set_sename(SH_TRANS, I_FIRST,  "");
    helper_seuser_get_set_sename(SH_TRANS, I_SECOND, "testseuser");
    helper_seuser_get_set_sename(SH_TRANS, I_SECOND, "");
}

// Function seuser_get_mlsrange, seuser_set_mlsrange

void helper_seuser_get_set_mlsrange(level_t level, int seuser_index,
                                    const char *mlsrange) {
    semanage_seuser_t *user;
    const char *old_mlsrange = NULL;
    const char *new_mlsrange = NULL;
    
    setup_handle(level);

    user = get_seuser_nth(seuser_index);

    old_mlsrange = semanage_seuser_get_mlsrange(user);

    CU_ASSERT_PTR_NOT_NULL(old_mlsrange);

    CU_ASSERT(semanage_seuser_set_mlsrange(sh, user, mlsrange) >= 0);

    new_mlsrange = semanage_seuser_get_mlsrange(user);

    CU_ASSERT_PTR_NOT_NULL(new_mlsrange);
    CU_ASSERT_STRING_EQUAL(new_mlsrange, mlsrange);

    semanage_seuser_free(user);
    cleanup_handle(level);
}

void test_seuser_get_set_mlsrange(void) {
    CU_ASSERT_SIGNAL(helper_seuser_get_set_mlsrange(SH_CONNECT, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_get_set_mlsrange(SH_CONNECT, I_NULL,
                                                    "testmlsrange"), SIGSEGV);

    helper_seuser_get_set_mlsrange(SH_CONNECT, I_FIRST,  "testmlsrange");
    helper_seuser_get_set_mlsrange(SH_CONNECT, I_FIRST,  "");
    helper_seuser_get_set_mlsrange(SH_CONNECT, I_SECOND, "testmlsrange");
    helper_seuser_get_set_mlsrange(SH_CONNECT, I_SECOND, "");

    CU_ASSERT_SIGNAL(helper_seuser_get_set_mlsrange(SH_TRANS, I_FIRST, NULL),
                     SIGSEGV);
    CU_ASSERT_SIGNAL(helper_seuser_get_set_mlsrange(SH_TRANS, I_NULL,
                                                    "testmlsrange"), SIGSEGV);

    helper_seuser_get_set_mlsrange(SH_TRANS, I_FIRST,  "testmlsrange");
    helper_seuser_get_set_mlsrange(SH_TRANS, I_FIRST,  "");
    helper_seuser_get_set_mlsrange(SH_TRANS, I_SECOND, "testmlsrange");
    helper_seuser_get_set_mlsrange(SH_TRANS, I_SECOND, "");
}

// Function seuser_create

void helper_seuser_create(level_t level) {
    semanage_seuser_t *seuser;
    
    setup_handle(level);

    CU_ASSERT(semanage_seuser_create(sh, &seuser) >= 0);

    CU_ASSERT_PTR_NULL(semanage_seuser_get_name(seuser));
    CU_ASSERT_PTR_NULL(semanage_seuser_get_sename(seuser));
    CU_ASSERT_PTR_NULL(semanage_seuser_get_mlsrange(seuser));

    semanage_seuser_free(seuser);
    cleanup_handle(level);
}

void test_seuser_create(void) {
    // FIXME
    //CU_ASSERT_SIGNAL(helper_seuser_create(SH_NULL), SIGSEGV);
    helper_seuser_create(SH_NULL);

    // FIXME
    //CU_ASSERT_SIGNAL(helper_seuser_create(SH_HANDLE), SIGSEGV);
    helper_seuser_create(SH_HANDLE);

    helper_seuser_create(SH_CONNECT);
    helper_seuser_create(SH_TRANS);
}

// Function seuser_clone
// TODO: error gotos

void helper_seuser_clone(level_t level, int seuser_index) {
    semanage_seuser_t *seuser;
    semanage_seuser_t *seuser_clone;
    const char *str;
    const char *str_clone;
    
    setup_handle(level);

    seuser = get_seuser_nth(seuser_index);

    CU_ASSERT(semanage_seuser_clone(sh, seuser, &seuser_clone) >= 0);

    str = semanage_seuser_get_name(seuser);
    str_clone = semanage_seuser_get_name(seuser_clone);

    CU_ASSERT_STRING_EQUAL(str, str_clone);

    str = semanage_seuser_get_sename(seuser);
    str_clone = semanage_seuser_get_sename(seuser_clone);

    CU_ASSERT_STRING_EQUAL(str, str_clone);

    str = semanage_seuser_get_mlsrange(seuser);
    str_clone = semanage_seuser_get_mlsrange(seuser_clone);

    CU_ASSERT_STRING_EQUAL(str, str_clone);

    semanage_seuser_free(seuser);
    semanage_seuser_free(seuser_clone);
    cleanup_handle(level);
}

void test_seuser_clone(void) {
    CU_ASSERT_SIGNAL(helper_seuser_clone(SH_CONNECT, I_NULL), SIGSEGV);
    helper_seuser_clone(SH_CONNECT, I_FIRST);
    helper_seuser_clone(SH_CONNECT, I_SECOND);

    CU_ASSERT_SIGNAL(helper_seuser_clone(SH_TRANS, I_NULL), SIGSEGV);
    helper_seuser_clone(SH_TRANS, I_FIRST);
    helper_seuser_clone(SH_TRANS, I_SECOND);
}

// Function seuser_query

void helper_seuser_query(level_t level, const char *seuser_str,
                         int exp_result) {
    semanage_seuser_key_t *key;
    semanage_seuser_t *response;
    const char *name;

    // setup
    setup_handle(level);

    key = get_seuser_key_from_str(seuser_str);

    // test
    if (exp_result < 0) {
        CU_ASSERT(semanage_seuser_query(sh, key, &response) < 0);
    }
    else {
        CU_ASSERT(semanage_seuser_query(sh, key, &response) >= 0);

        name = semanage_seuser_get_name(response);

        CU_ASSERT_STRING_EQUAL(name, seuser_str);
    }

    // cleanup
    semanage_seuser_key_free(key);
    cleanup_handle(level);
}

void test_seuser_query(void) {
    CU_ASSERT_SIGNAL(helper_seuser_query(SH_CONNECT, NULL, -1), SIGSEGV);

    helper_seuser_query(SH_CONNECT, SEUSER,  1);
    helper_seuser_query(SH_CONNECT, SEUSER2, 1);
    helper_seuser_query(SH_CONNECT, SEUSER_DEFAULT,      1);
    helper_seuser_query(SH_CONNECT, SEUSER_NONEXISTENT, -1);

    CU_ASSERT_SIGNAL(helper_seuser_query(SH_TRANS, NULL, -1), SIGSEGV);

    helper_seuser_query(SH_TRANS, SEUSER,  1);
    helper_seuser_query(SH_TRANS, SEUSER2, 1);
    helper_seuser_query(SH_TRANS, SEUSER_DEFAULT,      1);
    helper_seuser_query(SH_TRANS, SEUSER_NONEXISTENT, -1);
}

// Function seuser_exists

void helper_seuser_exists(level_t level, char * seuser_str, int exp_response) {
    semanage_seuser_key_t *key;
    int response;

    setup_handle(level);

    key = get_seuser_key_from_str(seuser_str);

    CU_ASSERT(semanage_seuser_exists(sh, key, &response) >= 0);
    CU_ASSERT(response == exp_response);

    CU_ASSERT_SIGNAL(semanage_seuser_exists(sh, key, NULL), SIGSEGV);

    semanage_seuser_key_free(key);

    cleanup_handle(level);
}

void test_seuser_exists(void) {
    CU_ASSERT_SIGNAL(helper_seuser_exists(SH_CONNECT, NULL, 0), SIGSEGV);

    helper_seuser_exists(SH_CONNECT, SEUSER,  1);
    helper_seuser_exists(SH_CONNECT, SEUSER2, 1);
    helper_seuser_exists(SH_CONNECT, SEUSER_DEFAULT,     1);
    helper_seuser_exists(SH_CONNECT, SEUSER_NONEXISTENT, 0);

    CU_ASSERT_SIGNAL(helper_seuser_exists(SH_TRANS, NULL, 0), SIGSEGV);

    helper_seuser_exists(SH_TRANS, SEUSER,  1);
    helper_seuser_exists(SH_TRANS, SEUSER2, 1);
    helper_seuser_exists(SH_TRANS, SEUSER_DEFAULT,     1);
    helper_seuser_exists(SH_TRANS, SEUSER_NONEXISTENT, 0);
}

// Function seuser_count

void test_seuser_count(void) {
    unsigned int response;

    // test without handle
    CU_ASSERT_SIGNAL(semanage_seuser_count(NULL, &response), SIGSEGV);

    // test with handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_seuser_count(sh, &response) < 0);

    CU_ASSERT(semanage_seuser_count(sh, NULL) < 0);
    
    // test connected
    connect();

    CU_ASSERT(semanage_seuser_count(sh, &response) >= 0);
    CU_ASSERT(response == SEUSERS_COUNT);

    CU_ASSERT_SIGNAL(semanage_seuser_count(sh, NULL), SIGSEGV);

    // test in transaction
    begin_transaction();

    CU_ASSERT(semanage_seuser_count(sh, &response) >= 0);
    CU_ASSERT(response == SEUSERS_COUNT);

    CU_ASSERT_SIGNAL(semanage_seuser_count(sh, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function seuser_iterate
// TODO: compare with entries in file
// TODO: test argument passing

unsigned int counter_seuser_iterate = 0;

int handler_seuser_iterate(const semanage_seuser_t *record, void *varg) {
    counter_seuser_iterate++;
    return 0;
}

void helper_seuser_iterate_invalid(void) {
    CU_ASSERT_SIGNAL(semanage_seuser_iterate(NULL, &handler_seuser_iterate,
                                             NULL), SIGSEGV);

    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_seuser_iterate(sh, &handler_seuser_iterate, NULL) < 0);

    CU_ASSERT(semanage_seuser_iterate(sh, NULL, NULL) < 0);

    cleanup_handle(SH_HANDLE);
}

void helper_seuser_iterate(level_t level) {
    setup_handle(level);

    counter_seuser_iterate = 0;

    CU_ASSERT(semanage_seuser_iterate(sh, &handler_seuser_iterate, NULL) >= 0);
    CU_ASSERT(counter_seuser_iterate == SEUSERS_COUNT);

    CU_ASSERT_SIGNAL(semanage_seuser_iterate(sh, NULL, NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_seuser_iterate(void) {
    helper_seuser_iterate_invalid();
    helper_seuser_iterate(SH_CONNECT);
    helper_seuser_iterate(SH_TRANS);
}

// Function seuser_list
// TODO: compare with entries in file

void helper_seuser_list_invalid(void) {
    semanage_seuser_t **records;
    unsigned int count;

    CU_ASSERT_SIGNAL(semanage_seuser_list(NULL, &records, &count), SIGSEGV);

    setup_handle(SH_HANDLE);
    
    CU_ASSERT(semanage_seuser_list(sh, &records, &count) < 0);

    CU_ASSERT(semanage_seuser_list(sh, NULL, &count) < 0);
    CU_ASSERT(semanage_seuser_list(sh, &records, NULL) < 0);

    cleanup_handle(SH_HANDLE);
}

void helper_seuser_list(level_t level) {
    semanage_seuser_t **records;
    unsigned int count;

    setup_handle(level);
    
    CU_ASSERT(semanage_seuser_list(sh, &records, &count) >= 0);
    CU_ASSERT(count == SEUSERS_COUNT);

    for (unsigned int i = 0; i < count; i++) {
        CU_ASSERT_PTR_NOT_NULL(records[i]);
    }

    for (unsigned int i = 0; i < count; i++) {
        semanage_seuser_free(records[i]);
    }

    free(records);

    CU_ASSERT_SIGNAL(semanage_seuser_list(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_seuser_list(sh, &records, NULL), SIGSEGV);

    cleanup_handle(level);
}

void test_seuser_list(void) {
    helper_seuser_list_invalid();
    helper_seuser_list(SH_CONNECT);
    helper_seuser_list(SH_TRANS);
}

// Function seuser_modify_local, seuser_del_local
// TODO: make semanage_seuser_get_sename return NULL
// TODO: make semanage_seuser_clone return error
// TODO: mls enabled
// TODO: make semanage_seuser_audit return error

void helper_seuser_modify_del_local(level_t level, int seuser_index,
                                    int exp_result) {
    semanage_seuser_t *seuser;
    semanage_seuser_t *seuser_local;
    semanage_seuser_key_t *key = NULL;
    int result;

    setup_handle(level);

    seuser = get_seuser_nth(seuser_index);

    CU_ASSERT(semanage_seuser_key_extract(sh, seuser, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL(key);

    result = semanage_seuser_modify_local(sh, key, seuser);

    if (exp_result < 0) {
        CU_ASSERT(result < 0);
    }
    else {
        CU_ASSERT(result >= 0);

        // write changes to file
        if (level == SH_TRANS) {
            commit();
            begin_transaction();
        }

        CU_ASSERT(semanage_seuser_query_local(sh, key, &seuser_local) >= 0);
        CU_ASSERT(semanage_seuser_compare2(seuser_local, seuser) == 0);

        CU_ASSERT(semanage_seuser_del_local(sh, key) >= 0);

        CU_ASSERT(semanage_seuser_query_local(sh, key, &seuser_local) < 0);
    }

#if VERS_CHECK(2, 5)
    CU_ASSERT_SIGNAL(semanage_seuser_modify_local(sh, NULL, seuser), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_seuser_modify_local(sh, key, NULL), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_seuser_del_local(sh, NULL), SIGSEGV);
#endif

    semanage_seuser_key_free(key);
    semanage_seuser_free(seuser);

    cleanup_handle(level);
}

void test_seuser_modify_del_local(void) {
    helper_seuser_modify_del_local(SH_CONNECT, I_FIRST,  -1);
    helper_seuser_modify_del_local(SH_CONNECT, I_SECOND, -1);

    helper_seuser_modify_del_local(SH_TRANS, I_FIRST,  1);
    helper_seuser_modify_del_local(SH_TRANS, I_SECOND, 1);
}

// Function seuser_query_local

void test_seuser_query_local(void) {
    semanage_seuser_key_t *key = NULL;
    semanage_seuser_t *response = NULL;

    // connect
    setup_handle(SH_CONNECT);

    key = get_seuser_key_nth(I_FIRST);

    CU_ASSERT(semanage_seuser_query_local(sh, key, &response) < 0);
    CU_ASSERT_PTR_NULL(response);

    semanage_seuser_key_free(key);
    cleanup_handle(SH_CONNECT);

    // transaction
    setup_handle(SH_TRANS);

    key = get_seuser_key_nth(I_FIRST);

    CU_ASSERT(semanage_seuser_query_local(sh, key, &response) < 0);
    CU_ASSERT_PTR_NULL(response);

    add_local_seuser(I_FIRST);

    CU_ASSERT(semanage_seuser_query_local(sh, key, &response) >= 0);
    CU_ASSERT_PTR_NOT_NULL(response);

    semanage_seuser_key_free(key);
    key = get_seuser_key_nth(I_SECOND);

    add_local_seuser(I_SECOND);

    CU_ASSERT(semanage_seuser_query_local(sh, key, &response) >= 0);
    CU_ASSERT_PTR_NOT_NULL(response);

    semanage_seuser_key_free(key);
    delete_local_seuser(I_FIRST);
    delete_local_seuser(I_SECOND);
    cleanup_handle(SH_TRANS);
}

// Function seuser_exists_local

void test_seuser_exists_local(void) {
    int response = -1;
    semanage_seuser_key_t *key;

    // setup
    setup_handle(SH_TRANS);

    // test
    key = get_seuser_key_nth(I_FIRST);

    CU_ASSERT(semanage_seuser_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 0);

    add_local_seuser(I_FIRST);
    response = -1;

    CU_ASSERT(semanage_seuser_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 1);

    delete_local_seuser(I_FIRST);
    response = -1;

    CU_ASSERT(semanage_seuser_exists_local(sh, key, &response) >= 0);
    CU_ASSERT(response == 0);

    response = -1;

    CU_ASSERT(semanage_seuser_exists_local(sh, NULL, &response) >= 0);
    CU_ASSERT(response == 0);

    CU_ASSERT_SIGNAL(semanage_seuser_exists_local(sh, key, NULL), SIGSEGV);

    // cleanup
    semanage_seuser_key_free(key);
    cleanup_handle(SH_TRANS);
}

// Function seuser_count_local

void test_seuser_count_local(void) {
    unsigned int response;

    // test without handle
    CU_ASSERT_SIGNAL(semanage_seuser_count_local(NULL, &response), SIGSEGV);

    // test with handle
    setup_handle(SH_HANDLE);

    CU_ASSERT(semanage_seuser_count_local(sh, &response) < 0);

    // test connected
    connect();

    CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 0);

    CU_ASSERT_SIGNAL(semanage_seuser_count_local(sh, NULL), SIGSEGV);

    // test in transaction
    begin_transaction();
    
    CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 0);

    add_local_seuser(I_FIRST);

    CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 1);

    add_local_seuser(I_SECOND);

    CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 2);

    delete_local_seuser(I_SECOND);

    CU_ASSERT(semanage_seuser_count_local(sh, &response) >= 0);
    CU_ASSERT(response == 1);

    delete_local_seuser(I_FIRST);

    CU_ASSERT_SIGNAL(semanage_seuser_count_local(sh, NULL), SIGSEGV);

    cleanup_handle(SH_TRANS);
}

// Function seuser_iterate_local

unsigned int counter_seuser_iterate_local = 0;

int handler_seuser_iterate_local(const semanage_seuser_t *record, void *varg) {
    counter_seuser_iterate_local++;
    return 0;
}

void test_seuser_iterate_local(void) {
    // test without handle
    CU_ASSERT_SIGNAL(semanage_seuser_iterate_local(NULL,
                                 &handler_seuser_iterate_local, NULL), SIGSEGV);

    // test with handle
    handle_create();

    CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
                                            NULL) < 0);

    CU_ASSERT(semanage_seuser_iterate_local(sh, NULL, NULL) < 0);

    // test connected
    connect();

    counter_seuser_iterate_local = 0;
    CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
                                            NULL) >= 0);
    CU_ASSERT(counter_seuser_iterate_local == 0);

    // FIXME
    CU_ASSERT(semanage_seuser_iterate_local(sh, NULL, NULL) >= 0);

    // test in transaction
    begin_transaction();

    counter_seuser_iterate_local = 0;
    CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
                                            NULL) >= 0);
    CU_ASSERT(counter_seuser_iterate_local == 0);

    add_local_seuser(I_FIRST);

    counter_seuser_iterate_local = 0;
    CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
                                            NULL) >= 0);
    CU_ASSERT(counter_seuser_iterate_local == 1);

    add_local_seuser(I_SECOND);

    counter_seuser_iterate_local = 0;
    CU_ASSERT(semanage_seuser_iterate_local(sh, &handler_seuser_iterate_local,
                                            NULL) >= 0);
    CU_ASSERT(counter_seuser_iterate_local == 2);

    CU_ASSERT_SIGNAL(semanage_seuser_iterate_local(sh, NULL, NULL), SIGSEGV);

    delete_local_seuser(I_FIRST);
    delete_local_seuser(I_SECOND);
    cleanup_handle(SH_TRANS);
}

// Function seuser_list_local

void test_seuser_list_local(void) {
    semanage_seuser_t **records;
    unsigned int count;

    // test without handle
    CU_ASSERT_SIGNAL(semanage_seuser_list_local(NULL, &records, &count),
                     SIGSEGV);
    
    // test with handle
    handle_create();

    CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) < 0);

    CU_ASSERT(semanage_seuser_list_local(sh, NULL, &count) < 0);
    CU_ASSERT(semanage_seuser_list_local(sh, &records, NULL) < 0);

    // test connected
    connect();
    
    CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 0);

    CU_ASSERT_SIGNAL(semanage_seuser_list_local(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_seuser_list_local(sh, &records, NULL), SIGSEGV);

    // test in transaction
    begin_transaction();

    CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 0);

    add_local_seuser(I_FIRST);
    
    CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 1);
    CU_ASSERT_PTR_NOT_NULL(records[0]);

    add_local_seuser(I_SECOND);
    
    CU_ASSERT(semanage_seuser_list_local(sh, &records, &count) >= 0);
    CU_ASSERT(count == 2);
    CU_ASSERT_PTR_NOT_NULL(records[0]);
    CU_ASSERT_PTR_NOT_NULL(records[1]);

    CU_ASSERT_SIGNAL(semanage_seuser_list_local(sh, NULL, &count), SIGSEGV);
    CU_ASSERT_SIGNAL(semanage_seuser_list_local(sh, &records, NULL), SIGSEGV);

    delete_local_seuser(I_FIRST);
    delete_local_seuser(I_SECOND);
    cleanup_handle(SH_TRANS);
}
