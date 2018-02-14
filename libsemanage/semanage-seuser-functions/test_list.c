#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t **records;
    unsigned int count;
    int result;
    
    if (argc < 2)
        exit(2);
    
    sh = get_handle(argv[1]);

    result = semanage_seuser_list(sh, &records, &count);
    printf("semanage_seuser_list(%p, %p, %p): %d",
           (void *) sh, (void *) &records, (void *) &count, result);
    
    if (result < 0) {
        perror("semanage_seuser_list");
        exit(1);
    }

    printf(", count: %u, records: ", count);

    const char *name;

    for (unsigned int i = 0; i < count; i++) {
        name = semanage_seuser_get_name(records[i]);
        printf("%p (%s), ", (void *) records[i], name);
    }

    printf("\n");

    if (argc >= 3) {
        unsigned int exp_count = strtoul(argv[2], NULL, 10);
        
        if (count != exp_count) {
            printf("Expected %u but got %u\n", exp_count, count);
            exit(1);
        }

        const char *name;

        for (unsigned int i = 0; i < count; i++) {
            name = semanage_seuser_get_name(records[i]);

            if (strcmp(name, argv[3 + i]) != 0) {
                printf("Expected %s but got %s\n", name, argv[3 + i]);
                exit(1);
            }
        }
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
