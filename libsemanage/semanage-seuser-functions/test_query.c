#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_key_t *key;
    semanage_seuser_t *response;
    int result;
    
    if (argc < 3)
        exit(2);
    
    sh = get_handle(argv[1]);

    result = semanage_seuser_key_create(sh, argv[2], &key);
    printf("semanage_seuser_key_create(%p, %s, %p): %d\n",
           (void *) sh, argv[2], (void *) &key, result);

    if (result < 0 || key == NULL) {
        perror("semanage_seuser_key_create");
        exit(2);
    }

    result = semanage_seuser_query(sh, key, &response);
    printf("semanage_seuser_query(%p, %p, %p): %d, response: %p\n",
           (void *) sh, (void *) key, (void *) &response, result, (void *) response);
    
    if (result < 0) {
        perror("semanage_seuser_query");
        exit(1);
    }

    const char *name = semanage_seuser_get_name(response);
    printf("semanage_seuser_get_name(%p): %s\n",
           (void *) response, name);

    if (strcmp(argv[2], name) != 0) {
        perror("semanage_seuser_get_name");
        exit(2);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
