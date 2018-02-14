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
        { SELABEL_OPT_VALIDATE, (char *) 1 },
        { SELABEL_OPT_DIGEST, (char *) 1 }
    };

    if (argc < 5) {
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


    if ((argc == 6) && (strcmp(argv[5], "nohandle") == 0)) {
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

        // set digest
        if (strcmp(argv[4], "0") == 0) {
            selabel_option[2].value = NULL;
        }
        else {
            selabel_option[2].value = (char *) 1;
        }

        printf("selabel_options: "); 
        printf("SELABEL_OPT_PATH = %s, ", selabel_option[0].value);
        printf("SELABEL_OPT_VALIDATE = %ld, ", (long int)(intptr_t) selabel_option[1].value);
        printf("SELABEL_OPT_DIGEST = %ld, ", (long int)(intptr_t) selabel_option[2].value);

        printf("Executing: selabel_open(SELABEL_%s, &selabel_option, 3)\n", argv[1]);

        errno = 0;

        if ((hnd = selabel_open(backend, selabel_option, 3)) == NULL) {
            perror("selabel_open - ERROR");
            return 255;
        }
    }

    unsigned char *digest;
    size_t digest_len;
    char **specfiles;
    size_t num_specfiles;
    int result, e = 0;

    printf("Executing: selabel_digest(hnd, digest, digest_len, specfiles, num_specfiles)\n");
    
    errno = 0;
    
    if ((result = selabel_digest(hnd, &digest, &digest_len, &specfiles, &num_specfiles)) == -1) {
        e = errno;
        perror("selabel_digest - ERROR");
    }
    else {
        printf("digest_len: %lu\n", digest_len);

        printf("digest: ");

        for (size_t i = 0; i < digest_len; i++)
            printf("%2x", digest[i]);

        printf("\n");

        printf("num_specfiles: %lu\n", num_specfiles);

        printf("specfiles:\n");

        for (size_t i = 0; i < num_specfiles; i++)
            printf("specfile: %s\n", specfiles[i]);
    }

    if (hnd != NULL)
        selabel_close(hnd);

    return e;
}
