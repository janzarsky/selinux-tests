#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    unsigned int response;
    int result;
    
    if (argc < 2)
        exit(2);
    
    sh = get_handle(argv[1]);

    result = semanage_seuser_count(sh, &response);
    printf("semanage_seuser_count(%p, %p): %d, response: %u\n",
           (void *) sh, (void *) &response, result, response);
    
    if (result < 0) {
        perror("semanage_seuser_count");
        exit(1);
    }

    if (argc >= 3)
        check_result_int(argv[2], response);

    destroy_handle(sh, argv[1]);

    exit(0);
}
