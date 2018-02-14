#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t *seuser;
    semanage_seuser_key_t *key;
    semanage_seuser_t **records;
    int result;
    unsigned int count;
    
    if (argc < 3)
        exit(2);
    
    sh = get_handle(argv[1]);

    seuser = test_get_seuser(sh, argv[2]);

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
        exit(1);
    }

    result = semanage_seuser_list_local(sh, &records, &count);
    printf("semanage_seuser_list_local(%p, %p, %p): %d\n",
           (void *) sh, (void *) &records, (void *) &count, result);
    
    if (result < 0) {
        perror("semanage_seuser_list_local");
        exit(2);
    }

    if (count != 1) {
        fprintf(stderr, "Number of local seusers is %u, expected 1!\n", count);
        exit(1);
    }

    if (semanage_seuser_compare(records[0], key) != 0) {
        fprintf(stderr, "Local seuser is different!\n");
        exit(1);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
