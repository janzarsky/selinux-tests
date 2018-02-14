#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int counter = 0;

int handler(const semanage_seuser_t *record, void *varg) {
    char **args = (char **) varg;
    
    const char *name = semanage_seuser_get_name(record);

    if (strcmp(name, args[2 + counter++]) != 0)
        return -1;

    return 0;
}

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    int result;
    
    if (argc < 2)
        exit(2);
    
    sh = get_handle(argv[1]);

    char **param = NULL;

    if (argc >= 3) {
        param = argv;
    }

    result = semanage_seuser_iterate(sh, &handler, (void *) param);
    printf("semanage_seuser_iterate(%p, %p, %p): %d\n",
           (void *) sh, (void *) &handler, (void *) param, result);
    
    if (result < 0) {
        perror("semanage_seuser_iterate");
        exit(1);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
