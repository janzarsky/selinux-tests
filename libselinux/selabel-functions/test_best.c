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
        { SELABEL_OPT_SUBSET, NULL },
        { SELABEL_OPT_VALIDATE, (char *) 1 },
        { SELABEL_OPT_BASEONLY, (char *) 1 }
    };

    if (argc < 8) {
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

    if ((argc == 9) && (strcmp(argv[8], "nohandle") == 0)) {
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

        printf("selabel_options: "); 
        printf("SELABEL_OPT_PATH = %s, ", selabel_option[0].value);
        printf("SELABEL_OPT_SUBSET = %s, ", selabel_option[1].value);
        printf("SELABEL_OPT_VALIDATE = %ld, ", (long int)(intptr_t) selabel_option[2].value);
        printf("SELABEL_OPT_BASEONLY = %ld\n", (long int)(intptr_t) selabel_option[3].value);

        printf("Executing: selabel_open(SELABEL_%s, &selabel_option, 4)\n", argv[1]);

        errno = 0;

        if ((hnd = selabel_open(backend, selabel_option, 4)) == NULL) {
            perror("selabel_open - ERROR");
            return 255;
        }
    }

    int result;
    security_context_t selabel_context;
    char *path;

    if (strcmp(argv[6], "NULL") == 0) {
        path = NULL;
    }
    else {
        path = argv[6];
    }

    // notice the base 8
    int mode = strtol(argv[7], NULL, 8);

    int alias_cnt = argc-8;
    const char **aliases = malloc((alias_cnt + 1)*sizeof(const char *));

    if (aliases == NULL)
        return 255;

    printf("aliases:");

    for (int i = 0; i < alias_cnt; i++) {
        aliases[i] = argv[8 + i];
        printf(" %s", argv[8 + i]);
    }

    printf("\n");

    aliases[alias_cnt] = NULL;

    printf("Executing: selabel_lookup_best_match(hnd, &selabel_context, %s, aliases, %d)\n", path, mode);

    errno = 0;
    int e1 = 0, e2 = 0;

    if ((result = selabel_lookup_best_match(hnd, &selabel_context, path, aliases, mode)) == -1) {
        e1 = errno;
        perror("selabel_lookup_best_match - ERROR");
    }
    else {
        printf("selabel_lookup_best_match context: %s\n", selabel_context);
        freecon(selabel_context);
    }

    printf("Executing: selabel_lookup_best_match_raw(hnd, &selabel_context, %s, aliases, %d)\n", path, mode);

    errno = 0;

    if ((result = selabel_lookup_best_match_raw(hnd, &selabel_context, path, aliases, mode)) == -1) {
        e2 = errno;
        perror("selabel_lookup_best_match_raw - ERROR");
    }
    else {
        printf("selabel_lookup_best_match_raw context: %s\n", selabel_context);
        freecon(selabel_context);
    }

    if (hnd != NULL)
        selabel_close(hnd);

    if (e1 == e2)
        return e1;
    else
        return 255;
}
