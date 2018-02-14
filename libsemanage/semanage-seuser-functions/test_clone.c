#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t *seuser;
    semanage_seuser_t *seuser_clone;
    int result;
    const char *str;
    const char *str_clone;
    
    if (argc < 3)
        exit(2);
    
    sh = get_handle(argv[1]);

    seuser = test_get_seuser(sh, argv[2]);

    result = semanage_seuser_clone(sh, seuser, &seuser_clone);
    printf("semanage_seuser_clone(%p, %p): %d\n",
           (void *) seuser, (void *) seuser_clone, result);

    if (result < 0) {
        perror("semanage_seuser_clone");
        exit(1);
    }

    str = semanage_seuser_get_name(seuser);
    str_clone = semanage_seuser_get_name(seuser_clone);

    if (strcmp(str, str_clone) != 0) {
        fprintf(stderr, "Different in get_name\n");
        exit(1);
    }

    str = semanage_seuser_get_sename(seuser);
    str_clone = semanage_seuser_get_sename(seuser_clone);

    if (strcmp(str, str_clone) != 0) {
        fprintf(stderr, "Different in get_sename\n");
        exit(1);
    }

    str = semanage_seuser_get_mlsrange(seuser);
    str_clone = semanage_seuser_get_mlsrange(seuser_clone);

    if (strcmp(str, str_clone) != 0) {
        fprintf(stderr, "Different in get_mlsrange\n");
        exit(1);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
