#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t *seuser;
    int result;
    const char *str;
    
    if (argc < 2)
        exit(2);
    
    sh = get_handle(argv[1]);

    result = semanage_seuser_create(sh, &seuser);
    printf("semanage_seuser_create(%p, %p): %d\n",
           (void *) sh, (void *) seuser, result);

    if (result < 0) {
        perror("semanage_seuser_create");
        exit(1);
    }

    str = semanage_seuser_get_name(seuser);

    if (str != NULL) {
        fprintf(stderr, "Expected name == NULL, got %s\n", str);
        exit(1);
    }

    str = semanage_seuser_get_sename(seuser);

    if (str != NULL) {
        fprintf(stderr, "Expected sename == NULL, got %s\n", str);
        exit(1);
    }

    str = semanage_seuser_get_mlsrange(seuser);

    if (str != NULL) {
        fprintf(stderr, "Expected mlsrange == NULL, got %s\n", str);
        exit(1);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
