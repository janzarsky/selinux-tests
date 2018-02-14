#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_key_t *key;
    const char *name;
    int result;
    
    if (argc < 3)
        exit(2);
    
    sh = get_handle(argv[1]);

    if (strcmp(argv[2], "NULL") == 0)
        name = NULL;
    else
        name = argv[2];

    result = semanage_seuser_key_create(sh, name, &key);
    printf("semanage_seuser_key_create(%p, %s, %p): %d\n",
           (void *) sh, name, (void *) &key, result);

    if (result < 0 || key == NULL) {
        perror("semanage_seuser_key_create");
        exit(1);
    }

    semanage_seuser_key_free(key);

    destroy_handle(sh, argv[1]);

    exit(0);
}
