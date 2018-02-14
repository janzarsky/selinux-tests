#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t *user;
    int result;
    const char *mlsrange;
    
    if (argc < 3)
        exit(2);
    
    sh = get_handle(argv[1]);

    user = test_get_seuser(sh, "first");

    if (strcmp(argv[2], "NULL") == 0)
        mlsrange = NULL;
    else 
        mlsrange = argv[2];

    const char *old_mlsrange = semanage_seuser_get_mlsrange(user);
    printf("semanage_seuser_get_mlsrange(%p): %s\n",
           (void *) user, old_mlsrange);

    if (old_mlsrange == NULL) {
        perror("semanage_seuser_get_mlsrange");
        exit(2);
    }

    if (strcmp(old_mlsrange, mlsrange) == 0) {
        printf("New mlsrange is the same\n");
        exit(2);
    }

    result = semanage_seuser_set_mlsrange(sh, user, mlsrange);
    printf("semanage_seuser_set_mlsrange(%p, %p, %s): %d\n",
           (void *) sh, (void *) user, mlsrange, result);

    if (result < 0) {
        perror("semanage_seuser_set_mlsrange");
        exit(1);
    }

    const char *new_mlsrange = semanage_seuser_get_mlsrange(user);
    printf("semanage_seuser_get_mlsrange(%p): %s\n",
           (void *) user, new_mlsrange);

    if (strcmp(new_mlsrange, mlsrange) != 0) {
        perror("semanage_seuser_get_mlsrange");
        exit(1);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
