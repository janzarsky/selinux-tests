#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t *seuser;
    
    if (argc < 4)
        exit(2);
    
    sh = get_handle(argv[1]);

    seuser = test_get_seuser(sh, argv[2]);

    const char *name = semanage_seuser_get_mlsrange(seuser);
    printf("semanage_seuser_get_mlsrange(%p): %s\n",
           (void *) seuser, name);

    if (strcmp_null(argv[3], name) != 0) {
        fprintf(stderr, "Expected %s but got %s\n", argv[2], name);
        exit(1);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
