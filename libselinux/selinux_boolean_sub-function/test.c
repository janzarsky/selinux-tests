#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <selinux/selinux.h>

int main (int argc, char **argv) {
    if (argc < 2) {
        printf("Invalid number of arguments");
        return -1;
    }

    char *boolean_name;

    if (strcmp(argv[1], "NULL") == 0) {
        boolean_name = NULL;
    }
    else {
        boolean_name = argv[1];
    }

    printf("Executing: selinux_boolean_sub(%s)\n", boolean_name);

    char *result = selinux_boolean_sub(boolean_name);

    printf("selinux_boolean_sub: %s\n", result);

    free(result);
    return 0;
}
