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
    int mode;
    
    if (strcmp(argv[7], "X_PROP") == 0)
        mode = SELABEL_X_PROP;
    else if (strcmp(argv[7], "X_SELN") == 0)
        mode = SELABEL_X_SELN;
    else if (strcmp(argv[7], "X_EXT") == 0)
        mode = SELABEL_X_EXT;
    else if (strcmp(argv[7], "X_EVENT") == 0)
        mode = SELABEL_X_EVENT;
    else if (strcmp(argv[7], "X_CLIENT") == 0)
        mode = SELABEL_X_CLIENT;
    else if (strcmp(argv[7], "X_POLYPROP") == 0)
        mode = SELABEL_X_POLYPROP;
    else if (strcmp(argv[7], "X_POLYSELN") == 0)
        mode = SELABEL_X_POLYSELN;
    else if (strcmp(argv[7], "DB_DATABASE") == 0)
        mode = SELABEL_DB_DATABASE;
    else if (strcmp(argv[7], "DB_SCHEMA") == 0)
        mode = SELABEL_DB_SCHEMA;
    else if (strcmp(argv[7], "DB_VIEW") == 0)
        mode = SELABEL_DB_VIEW;
    else if (strcmp(argv[7], "DB_TABLE") == 0)
        mode = SELABEL_DB_TABLE;
    else if (strcmp(argv[7], "DB_COLUMN") == 0)
        mode = SELABEL_DB_COLUMN;
    else if (strcmp(argv[7], "DB_TUPLE") == 0)
        mode = SELABEL_DB_TUPLE;
    else if (strcmp(argv[7], "DB_PROCEDURE") == 0)
        mode = SELABEL_DB_PROCEDURE;
    else if (strcmp(argv[7], "DB_SEQUENCE") == 0)
        mode = SELABEL_DB_SEQUENCE;
    else if (strcmp(argv[7], "DB_BLOB") == 0)
        mode = SELABEL_DB_BLOB;
#ifndef RHEL6
    else if (strcmp(argv[7], "DB_LANGUAGE") == 0)
        mode = SELABEL_DB_LANGUAGE;
    else if (strcmp(argv[7], "DB_EXCEPTION") == 0)
        mode = SELABEL_DB_EXCEPTION;
    else if (strcmp(argv[7], "DB_DATATYPE") == 0)
        mode = SELABEL_DB_DATATYPE;
#endif
    else
        // notice the base 8
        mode = strtol(argv[7], NULL, 8);
    
    int e1 = 0, e2 = 0;

    if (strcmp(argv[6], "NULL") == 0) {
        path = NULL;
    }
    else if (strcmp(argv[6], "'*'") == 0) {
        path = "*";
    }
    else {
        path = argv[6];
    }

    printf("Executing: selabel_lookup(hnd, &selabel_context, %s, %d)\n", path, mode);

    errno = 0;
    
    if ((result = selabel_lookup(hnd, &selabel_context, path, mode)) == -1) {
        e1 = errno;
        perror("selabel_lookup - ERROR");
    }
    else {
        printf("selabel_lookup context: %s\n", selabel_context);
        freecon(selabel_context);
    }

    printf("Executing: selabel_lookup_raw(hnd, &selabel_context, %s, %d)\n", path, mode);

    errno = 0;
    
    if ((result = selabel_lookup_raw(hnd, &selabel_context, path, mode)) == -1) {
        e2 = errno;
        perror("selabel_lookup_raw - ERROR");
    }
    else {
        printf("selabel_lookup_raw context: %s\n", selabel_context);
        freecon(selabel_context);
    }

    if (hnd != NULL)
        selabel_close(hnd);

    if (e1 == e2)
        return e1;
    else
        return 255;
}
