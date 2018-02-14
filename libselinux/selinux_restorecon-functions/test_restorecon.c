#include <stdio.h>
#include <string.h>
#include <selinux/selinux.h>
#include <selinux/context.h>
#include <selinux/label.h>
#include <selinux/restorecon.h>

int main(int argc, char **argv) {
    unsigned int flags = 0;
    char *path = NULL;

    if (argc < 2) {
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }

    // set path
    if (strcmp(argv[1], "EMPTY") == 0) {
        path = "";
    }
    else if (strcmp(argv[1], "NULL") == 0) {
        path = NULL;
    }
    else {
        path = argv[1];
    }
    
    // set flags (if any)
    if (argc >= 3) {
        for (int i = 2; i < argc; i++)
            if (strcmp(argv[i], "IGNORE_DIGEST") == 0)
                flags |= SELINUX_RESTORECON_IGNORE_DIGEST;
            else if (strcmp(argv[i], "NOCHANGE") == 0)
                flags |= SELINUX_RESTORECON_NOCHANGE;
            else if (strcmp(argv[i], "VERBOSE") == 0)
                flags |= SELINUX_RESTORECON_VERBOSE;
            else if (strcmp(argv[i], "PROGRESS") == 0)
                flags |= SELINUX_RESTORECON_PROGRESS;
            else if (strcmp(argv[i], "RECURSE") == 0)
                flags |= SELINUX_RESTORECON_RECURSE;
            else if (strcmp(argv[i], "SET_SPECFILE_CTX") == 0)
                flags |= SELINUX_RESTORECON_SET_SPECFILE_CTX;
            else if (strcmp(argv[i], "REALPATH") == 0)
                flags |= SELINUX_RESTORECON_REALPATH;
            else if (strcmp(argv[i], "XDEV") == 0)
                flags |= SELINUX_RESTORECON_XDEV;
            else {
                fprintf(stderr, "Invalid flag %s\n", argv[i]);
                return 1;
            }

    }

    printf("Running selinux_restorecon(\"%s\", %#08x);\n", path, flags);

    return selinux_restorecon(path, flags);
}
