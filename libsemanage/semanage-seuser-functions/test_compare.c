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
    int result;
    
    if (argc < 3)
        exit(2);
    
    sh = get_handle(argv[1]);

    seuser = test_get_seuser(sh, "first");

    key = test_get_key(sh, argv[2]);

    result = semanage_seuser_compare(seuser, key);
    printf("semanage_seuser_compare(%p, %p): %d\n",
           (void *) seuser, (void *) key, result);

    if (argc >= 4) {
        if (strcmp(argv[3], "same") == 0 && result != 0) {
            fprintf(stderr, "Expected same but got different\n");
            exit(1);
        }
        else if (strcmp(argv[3], "different") == 0 && result == 0) {
            fprintf(stderr, "Expected different but got same\n");
            exit(1);
        }
    }

    semanage_seuser_key_free(key);

    destroy_handle(sh, argv[1]);

    exit(0);
}
