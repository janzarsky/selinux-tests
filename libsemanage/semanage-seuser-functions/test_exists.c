#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_key_t *key;
    int result;
    int response;
    
    if (argc < 3)
        exit(2);
    
    sh = get_handle(argv[1]);

    key = test_get_key(sh, argv[2]);

    result = semanage_seuser_exists(sh, key, &response);
    printf("semanage_seuser_exists(%p, %p, %p): %d, response: %d\n",
           (void *) sh, (void *) key, (void *) &response, result, response);
    
    if (result < 0) {
        perror("semanage_seuser_exists");
        exit(1);
    }

    if (argc >= 4)
        check_result_int(argv[3], response);

    destroy_handle(sh, argv[1]);

    exit(0);
}
