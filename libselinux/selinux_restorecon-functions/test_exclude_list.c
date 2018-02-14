#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <selinux/selinux.h>
#include <selinux/context.h>
#include <selinux/label.h>
#include <selinux/restorecon.h>

int main(int argc, char **argv) {
    char *path = NULL;
    const char **list = NULL;
    unsigned int flags = 0;

    if (argc < 3) {
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }
    
    // set restorecon path
    if (strcmp(argv[2], "EMPTY") == 0) {
        path = "";
    }
    else if (strcmp(argv[2], "NULL") == 0) {
        path = NULL;
    }
    else {
        path = argv[2];
    }

    // set restorecon flags
    flags |= SELINUX_RESTORECON_RECURSE;
    flags |= SELINUX_RESTORECON_IGNORE_DIGEST;

    // set exclude list
    if (strcmp(argv[1], "NULL") == 0) {
        list = NULL;
    }
    else if (strcmp(argv[1], "EMPTY") == 0) {
        list = malloc(sizeof(char*));
        list[0] = NULL;
    }
    else {
        list = malloc(2*sizeof(char*));
        list[0] = argv[1];
        list[1] = NULL;
    }

    printf("Running selinux_restorecon_set_exclude_list();\n");

    selinux_restorecon_set_exclude_list(list);

    printf("Running selinux_restorecon(\"%s\", %#08x);\n", path, flags);

    return selinux_restorecon(path, flags);
}
