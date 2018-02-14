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
    int nopt = 0;
    unsigned int backend = 0;

    struct selinux_opt selabel_option [] = {
        { SELABEL_OPT_PATH, NULL },
        { SELABEL_OPT_SUBSET, NULL },
        { SELABEL_OPT_VALIDATE, (char *) 1 },
        { SELABEL_OPT_BASEONLY, (char *) 1 }
    };

    if (argc < 6) {
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

    // set file contexts path
    if (strcmp(argv[2], "NULL") == 0) {
        selabel_option[0].value = NULL;
	}
	else {
        selabel_option[0].value = argv[2];
	}

    // set subset
    if (strcmp(argv[3], "NULL") == 0) {
        selabel_option[1].value = NULL;
	}
	else {
        selabel_option[1].value = argv[3];
	}

    // set validate
    if (strcmp(argv[4], "0") == 0) {
        selabel_option[2].value = NULL;
	}
	else {
        selabel_option[2].value = (char *) 1;
	}

    // set baseonly
    if (strcmp(argv[5], "0") == 0) {
        selabel_option[3].value = NULL;
	}
	else {
        selabel_option[3].value = (char *) 1;
	}

    if (argc == 7) {
        nopt = strtol(argv[6], NULL, 10);
    }
    else {
        nopt = 4;
    }

    printf("selabel_options: "); 
    printf("SELABEL_OPT_PATH = %s, ", selabel_option[0].value);
    printf("SELABEL_OPT_SUBSET = %s, ", selabel_option[1].value);
    printf("SELABEL_OPT_VALIDATE = %ld, ", (long int)(intptr_t) selabel_option[2].value);
    printf("SELABEL_OPT_BASEONLY = %ld\n", (long int)(intptr_t) selabel_option[3].value);

    printf("Executing: selabel_open(SELABEL_%s, &selabel_option, %d)\n\n", argv[1], nopt);

    errno = 0;

    if ((hnd = selabel_open(backend, selabel_option, nopt)) == NULL) {
        int e = errno;
        perror("selabel_open - ERROR");
        return e;
    }

    selabel_close(hnd);
    return 0;
}
