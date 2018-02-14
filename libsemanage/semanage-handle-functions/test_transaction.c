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
    
    if (argc >= 3 && strcmp(argv[2], "reversed") == 0) {
        test_commit(sh);
        test_begin_transaction(sh);
    }
    else if (argc >= 3 && strcmp(argv[2], "twice") == 0) {
        test_begin_transaction(sh);
        test_begin_transaction(sh);
        test_commit(sh);
    }
    else {
        test_begin_transaction(sh);
        test_commit(sh);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
