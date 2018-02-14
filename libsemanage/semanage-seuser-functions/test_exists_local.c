#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t *seuser;
    semanage_seuser_t *seuser_exists;
    semanage_seuser_key_t *key;
    int result;
    int response;
    int exp;
    
    if (argc < 4)
        exit(2);
    
    sh = get_handle(argv[1]);

    seuser = test_get_seuser(sh, argv[2]);
    seuser_exists = test_get_seuser(sh, argv[3]);

    test_add_local_seuser(sh, seuser);

    result = semanage_seuser_key_extract(sh, seuser_exists, &key);
    printf("semanage_seuser_key_extract(%p, %p, %p): %d\n",
           (void *) sh, (void *) seuser_exists, (void *) &key, result); 
    if (result < 0) {
        perror("semanage_seuser_key_extract");
        exit(2);
    }

    result = semanage_seuser_exists_local(sh, key, &response);
    printf("semanage_seuser_exists_local(%p, %p, %p): %d\n",
           (void *) sh, (void *) key, (void *) &response, result);

    if (result < 0) {
        perror("semanage_seuser_exists_local");
        exit(1);
    }

    if (argc >= 5) {
        exp = strtol(argv[4], NULL, 10);

        if (response != exp) {
            fprintf(stderr, "Expected %d but got %d\n", exp, response);
            exit(1);
        }
    }

    test_del_local_seuser(sh, seuser);

    destroy_handle(sh, argv[1]);

    exit(0);
}
