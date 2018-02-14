#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;

    if (argc < 2)
        exit(1);
    
    sh = get_handle(argv[1]);

    int result = semanage_access_check(sh);
    printf("semanage_access_check(%p): %d\n", (void *) sh, result);
    
    if (result < 0 || (result != 0 && result != SEMANAGE_CAN_READ
                       && result != SEMANAGE_CAN_WRITE)) {
        perror("semanage_access_check");
        exit(1);
    }

    if (argc >= 3)
        check_result_int(argv[2], result);

    destroy_handle(sh, argv[1]);

    exit(0);
}
