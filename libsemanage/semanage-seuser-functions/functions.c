#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

void check_result_int(const char *expected, int real) {
    int exp = strtol(expected, NULL, 10);

    if (exp != real) {
        fprintf(stderr, "Expected %d but got %d\n", exp, real);
        exit(1);
    }
}

semanage_handle_t *test_handle_create() {
    semanage_handle_t *sh = NULL;

    sh = semanage_handle_create();
    printf("semanage_handle_create(): %p\n", (void *) sh);

    if (sh == NULL) {
        perror("semanage_handle_create");
        exit(2);
    }

    return sh;
}

int test_connect(semanage_handle_t *sh) {
    int result = semanage_connect(sh);
    printf("semanage_connect(%p): %d\n", (void *) sh, result);

    if (result != 0) {
        perror("semanage_connect");
        exit(2);
    }

    return result;
}

int test_disconnect(semanage_handle_t *sh) {
    int result = semanage_disconnect(sh);
    printf("semanage_disconnect(%p): %d\n", (void *) sh, result);

    if (result != 0) {
        perror("semanage_disconnect");
        exit(2);
    }

    return result;
}

int test_begin_transaction(semanage_handle_t *sh) {
    int result = semanage_begin_transaction(sh);
    printf("semanage_begin_transaction(%p): %d\n", (void *) sh, result);

    if (result != 0) {
        perror("semanage_begin_transaction");
        exit(2);
    }

    return result;
}

int test_commit(semanage_handle_t *sh) {
    int result = semanage_commit(sh);
    printf("semanage_commit(%p): %d\n", (void *) sh, result);

    if (result != 0) {
        perror("semanage_commit");
        exit(2);
    }

    return result;
}

semanage_seuser_key_t *test_get_key(semanage_handle_t *sh, const char *name) {
    semanage_seuser_key_t *key;
    int result = semanage_seuser_key_create(sh, name, &key);
    printf("semanage_seuser_key_create(%p, %s, %p): %d\n",
           (void *) sh, name, (void *) &key, result);

    if (key == NULL || result < 0) {
        perror("semanage_seuser_key_create");
        exit(2);
    }

    return key;
}

semanage_seuser_t *test_get_seuser_nth(semanage_handle_t *sh, unsigned int index) {
    int result;
    semanage_seuser_t **records;
    unsigned int count;

    result = semanage_seuser_list(sh, &records, &count);
    printf("semanage_seuser_list(%p, %p, %p): %d\n",
           (void *) sh, (void *) &records, (void *) &count, result);
    
    if (result < 0) {
        perror("semanage_seuser_list");
        exit(2);
    }

    if (count < index + 1)
        exit(2);

    return records[index];
}

semanage_seuser_t *test_get_seuser_new(semanage_handle_t *sh) {
    int result;
    semanage_seuser_t *seuser;

    result = semanage_seuser_create(sh, &seuser);
    printf("semanage_seuser_create(%p, %p): %d\n",
           (void *) sh, (void *) seuser, result);
    
    if (result < 0) {
        perror("semanage_seuser_create");
        exit(2);
    }

    return seuser;
}

semanage_seuser_t *test_get_seuser(semanage_handle_t *sh, const char *param) {
    if (strcmp(param, "new") == 0)
        return test_get_seuser_new(sh);
    
    if (strcmp(param, "first") == 0)
        return test_get_seuser_nth(sh, 0);

    if (strcmp(param, "second") == 0)
        return test_get_seuser_nth(sh, 1);

    fprintf(stderr, "Unknown seuser \"%s\" specified\n", param);
    exit(2);
}

void test_add_local_seuser(semanage_handle_t *sh, semanage_seuser_t *seuser) {
    int result;
    semanage_seuser_key_t *key;

    result = semanage_seuser_key_extract(sh, seuser, &key);
    printf("semanage_seuser_key_extract(%p, %p, %p): %d\n",
           (void *) sh, (void *) seuser, (void *) &key, result);

    if (result < 0) {
        perror("semanage_seuser_key_extract");
        exit(2);
    }

    result = semanage_seuser_modify_local(sh, key, seuser);
    printf("semanage_seuser_modify_local(%p, %p, %p): %d\n",
           (void *) seuser, (void *) key, (void *) seuser, result);

    if (result < 0) {
        perror("semanage_seuser_modify_local");
        exit(2);
    }
}

void test_del_local_seuser(semanage_handle_t *sh, semanage_seuser_t *seuser) {
    int result;
    semanage_seuser_key_t *key;

    result = semanage_seuser_key_extract(sh, seuser, &key);
    printf("semanage_seuser_key_extract(%p, %p, %p): %d\n",
           (void *) sh, (void *) seuser, (void *) &key, result);

    if (result < 0) {
        perror("semanage_seuser_key_extract");
        exit(2);
    }

    result = semanage_seuser_del_local(sh, key);
    printf("semanage_seuser_del_local(%p, %p): %d\n",
           (void *) seuser, (void *) key, result);

    if (result < 0) {
        perror("semanage_seuser_del_local");
        exit(2);
    }
}

#define STATE_INIT      1
#define STATE_HANDLE    2
#define STATE_CONN      3
#define STATE_TRANS     4

int get_state(const char *state_str) {
    if (strcmp(state_str, "init") == 0)
        return STATE_INIT;
    if (strcmp(state_str, "handle") == 0)
        return STATE_HANDLE;
    if (strcmp(state_str, "conn") == 0)
        return STATE_CONN;
    if (strcmp(state_str, "trans") == 0)
        return STATE_TRANS;

    return 0;
}

semanage_handle_t * get_handle(const char *state_str) {
    int state;
    semanage_handle_t *sh = NULL;

    state = get_state(state_str);

    if (state >= STATE_INIT)
        sh = NULL;

    if (state >= STATE_HANDLE)
        sh = test_handle_create();

    if (state >= STATE_CONN)
        test_connect(sh);

    if (state >= STATE_TRANS)
        test_begin_transaction(sh);

    return sh;
}

void destroy_handle(semanage_handle_t *sh, const char *state_str) {
    int state;

    state = get_state(state_str);

    if (state >= STATE_TRANS)
        test_commit(sh);

    if (state >= STATE_CONN)
        test_disconnect(sh);

    if (state >= STATE_HANDLE) {
        semanage_handle_destroy(sh);
        printf("semanage_handle_destroy(%p)\n", (void *) sh);
    }
}

int strcmp_null(const char *str1, const char *str2) {
    if (str1 == NULL && str2 == NULL)
        return 0;

    if (str1 == NULL) {
        if (strcmp(str2, "NULL") == 0)
            return 0;
        else
            return -1;
    }

    if (str2 == NULL) {
        if (strcmp(str1, "NULL") == 0)
            return 0;
        else
            return 1;
    }

    return strcmp(str1, str2);
}
