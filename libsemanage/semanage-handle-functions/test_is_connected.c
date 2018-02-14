#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    int result;
    
    if (argc < 2)
        exit(1);
    
    sh = get_handle(argv[1]);

    result = semanage_is_connected(sh);
    printf("semanage_is_connected(%p): %d\n", (void *) sh, result);

    if (result != 0 && result != 1) {
        perror("semanage_is_connected");
        exit(1);
    }

    if (argc >= 3)
        check_result_int(argv[2], result);

    destroy_handle(sh, argv[1]);

    exit(0);
}
