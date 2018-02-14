#include <stdio.h>
#include <errno.h>
#include <sepol/sepol.h>

int main (int argc, char *argv[]) {
    FILE *policyfile;

    if (argc < 3) {
        fprintf(stderr, "%s <binary-policy-path> <context>\n", argv[0]);
        return 1;
    }

    policyfile = fopen(argv[1], "r");
    if (policyfile == NULL) {
        perror("fopen");
        return 1;
    }

    if (sepol_set_policydb_from_file(policyfile) < 0) {
        perror("sepol_set_policydb_from_file");
        return 1;
    }

    if (sepol_check_context(argv[2]) < 0) {
        perror("sepol_check_context");
        return 1;
    }

    if (fclose(policyfile) != 0) {
        perror("fclose");
    }

    return 0;
}

