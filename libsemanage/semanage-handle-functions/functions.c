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
        exit(1);
    }

    return sh;
}

int test_connect(semanage_handle_t *sh) {
    int result = semanage_connect(sh);
    printf("semanage_connect(%p): %d\n", (void *) sh, result);

    if (result != 0) {
        perror("semanage_connect");
        exit(1);
    }

    return result;
}

int test_disconnect(semanage_handle_t *sh) {
    int result = semanage_disconnect(sh);
    printf("semanage_disconnect(%p): %d\n", (void *) sh, result);

    if (result != 0) {
        perror("semanage_disconnect");
        exit(1);
    }

    return result;
}

int test_begin_transaction(semanage_handle_t *sh) {
    int result = semanage_begin_transaction(sh);
    printf("semanage_begin_transaction(%p): %d\n", (void *) sh, result);

    if (result != 0) {
        perror("semanage_begin_transaction");
        exit(1);
    }

    return result;
}

int test_commit(semanage_handle_t *sh) {
    int result = semanage_commit(sh);
    printf("semanage_commit(%p): %d\n", (void *) sh, result);

    if (result != 0) {
        perror("semanage_commit");
        exit(1);
    }

    return result;
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
