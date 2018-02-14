#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <selinux/selinux.h>
#include <linux/limits.h>

int main (int argc, char **argv) {
    if (argc < 2) {
        printf("Invalid number of arguments\n");
        return -1;
    }

    char *name;

    if (strcmp(argv[1], "NULL") == 0) {
        name = NULL;
    }
    else {
        name = argv[1];
    }

    char *resolved_path;

    if (argc == 3 && (strcmp(argv[1], "NULL") == 0)) {
        resolved_path = NULL;
    }
    else {
        resolved_path = malloc(PATH_MAX);

        if (resolved_path == NULL) {
            printf("Error while allocating memory\n");
        }
    }

    printf("Executing: realpath_not_final(%s, resolved_path)\n", name);

    int result = realpath_not_final(name, resolved_path);

    printf("realpath_not_final: %s\n", resolved_path);

    free(resolved_path);
    return result;
}
