#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t *seuser;
    semanage_seuser_t *seuser2;
    int result;
    int first = -1;
    int second = -1;
    
    if (argc < 4)
        exit(2);
    
    sh = get_handle(argv[1]);

    if (strcmp(argv[2], "NULL") == 0) {
        seuser = NULL;
    }
    else {
        first = strtol(argv[2], NULL, 10);
        seuser = test_get_seuser_nth(sh, first);
    }

    if (strcmp(argv[3], "NULL") == 0) {
        seuser2 = NULL;
    }
    else {
        second = strtol(argv[3], NULL, 10);
        seuser2 = test_get_seuser_nth(sh, second);
    }

    result = semanage_seuser_compare2(seuser, seuser2);
    printf("semanage_seuser_compare(%p, %p): %d\n",
           (void *) seuser, (void *) seuser2, result);

    if (first == second && result != 0) {
        fprintf(stderr, "Expected same but got different\n");
        exit(1);
    }
    else if (first != second && result == 0) {
        fprintf(stderr, "Expected different but got same\n");
        exit(1);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
