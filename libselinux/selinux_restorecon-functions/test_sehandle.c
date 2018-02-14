#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <selinux/selinux.h>
#include <selinux/context.h>
#include <selinux/label.h>
#include <selinux/restorecon.h>

int main(int argc, char **argv) {
    struct selabel_handle *hndl = NULL;
    char *path = NULL;
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

    // set sehandle
    if (strcmp(argv[1], "DEFAULT") == 0) {
        hndl = selinux_restorecon_default_handle();

        if (hndl == NULL) {
            return 1;
        }
    }
    else if (strcmp(argv[1], "INVALID") == 0) {
        hndl = (struct selabel_handle *) 1;
    }
    else if (strcmp(argv[1], "NULL") == 0) {
        hndl = NULL;
    }
    else if (strcmp(argv[1], "CUSTOM") == 0) {
        struct selinux_opt options[] = {
            { SELABEL_OPT_DIGEST, (char *)1 },
            { SELABEL_OPT_BASEONLY, (char *)1 }
        };

        hndl = selabel_open(SELABEL_CTX_FILE, options, 2);
    }

    printf("Running selinux_restorecon_set_sehandle();\n");

    selinux_restorecon_set_sehandle(hndl);

    printf("Running selinux_restorecon(\"%s\", %#08x);\n", path, flags);

    return selinux_restorecon(path, flags);
}
