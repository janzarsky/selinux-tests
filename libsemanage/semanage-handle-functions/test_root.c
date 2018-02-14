#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    const char *root;
    int result;

    if (argc < 2)
        exit(1);
    
    sh = get_handle(argv[1]);

    root = semanage_root();
    printf("semanage_root(): %s\n", root);

    if (root == NULL) {
        perror("semanage_root");
        exit(1);
    }

    if (argc >= 3) {
        result = semanage_set_root(argv[2]);
        printf("semanage_set_root(\"%s\"): %d\n", argv[2], result);

        if (root == NULL) {
            perror("semanage_set_root");
            exit(1);
        }

        root = semanage_root();
        printf("semanage_root(): %s\n", root);

        if (result != 0) {
            perror("semanage_root");
            exit(1);
        }

        if (strcmp(root, argv[2]) != 0) {
            fprintf(stderr, "Expected \"%s\" but got \"%s\"\n", argv[2], root);
            exit(1);
        }
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
