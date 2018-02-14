#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <selinux/selinux.h>
#include <selinux/label.h>

int main (int argc, char **argv)
{
    struct selabel_handle *hnd = NULL;
    unsigned int backend = 0;

    struct selinux_opt selabel_option [] = {
        { SELABEL_OPT_PATH, NULL },
        { SELABEL_OPT_VALIDATE, (char *) 1 }
    };

    if (argc < 4) {
        fprintf(stderr, "Invalid number of arguments\n");
        return 255;
    }

    // set backend
    if (strcmp(argv[1], "CTX_FILE") == 0)
        backend = SELABEL_CTX_FILE;
    else if (strcmp(argv[1], "CTX_MEDIA") == 0)
        backend = SELABEL_CTX_MEDIA;
    else if (strcmp(argv[1], "CTX_X") == 0)
        backend = SELABEL_CTX_X;
    else if (strcmp(argv[1], "CTX_DB") == 0)
        backend = SELABEL_CTX_DB;
#ifndef RHEL6
    else if (strcmp(argv[1], "CTX_ANDROID_PROP") == 0)
        backend = SELABEL_CTX_ANDROID_PROP;
#endif
    else
        backend = strtoul(argv[1], NULL, 10);


    if ((argc == 5) && (strcmp(argv[4], "nohandle") == 0)) {
        hnd = NULL;
    }
    else {
        // set file contexts path
        if (strcmp(argv[2], "NULL") == 0) {
            selabel_option[0].value = NULL;
        }
        else {
            selabel_option[0].value = argv[2];
        }

        // set validate
        if (strcmp(argv[3], "0") == 0) {
            selabel_option[1].value = NULL;
        }
        else {
            selabel_option[1].value = (char *) 1;
        }

        printf("selabel_options: "); 
        printf("SELABEL_OPT_PATH = %s, ", selabel_option[0].value);
        printf("SELABEL_OPT_VALIDATE = %ld, ", (long int)(intptr_t) selabel_option[1].value);

        printf("Executing: selabel_open(SELABEL_%s, &selabel_option, 2)\n", argv[1]);

        errno = 0;

        if ((hnd = selabel_open(backend, selabel_option, 2)) == NULL) {
            perror("selabel_open - ERROR");
            return 255;
        }
    }

    printf("Executing: selabel_stats(hnd)\n");
    
    selabel_stats(hnd);

    if (hnd != NULL)
        selabel_close(hnd);

    return 0;
}
