#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t *seuser;
    int result;
    unsigned int response;
    int num;
    
    if (argc < 2)
        exit(2);
    
    sh = get_handle(argv[1]);

    num = strtol(argv[2], NULL, 10);

    for (int i = 0; i < num; i++) {
        seuser = test_get_seuser_nth(sh, i);

        test_add_local_seuser(sh, seuser);
    }

    result = semanage_seuser_count_local(sh, &response);
    printf("semanage_seuser_count_local(%p, %p): %d, response: %d\n",
           (void *) sh, (void *) &response, result, response);

    if (result < 0) {
        perror("semanage_seuser_count_local");
        exit(1);
    }

    if (argc >= 3)
        check_result_int(argv[2], response);

    test_del_local_seuser(sh, seuser);

    destroy_handle(sh, argv[1]);

    exit(0);
}
