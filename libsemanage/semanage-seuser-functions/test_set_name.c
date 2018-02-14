#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <semanage/semanage.h>

#include "functions.c"

int main (int argc, char **argv) {
    semanage_handle_t *sh;
    semanage_seuser_t *user;
    int result;
    const char *name;
    
    if (argc < 3)
        exit(2);
    
    sh = get_handle(argv[1]);

    user = test_get_seuser(sh, "first");

    if (strcmp(argv[2], "NULL") == 0)
        name = NULL;
    else 
        name = argv[2];

    const char *old_name = semanage_seuser_get_name(user);
    printf("semanage_seuser_get_name(%p): %s\n",
           (void *) user, old_name);

    if (old_name == NULL) {
        perror("semanage_seuser_get_name");
        exit(2);
    }

    if (strcmp(old_name, name) == 0) {
        printf("New name is the same\n");
        exit(2);
    }

    result = semanage_seuser_set_name(sh, user, name);
    printf("semanage_seuser_set_name(%p, %p, %s): %d\n",
           (void *) sh, (void *) user, name, result);

    if (result < 0) {
        perror("semanage_seuser_set_name");
        exit(1);
    }

    const char *new_name = semanage_seuser_get_name(user);
    printf("semanage_seuser_get_name(%p): %s\n",
           (void *) user, new_name);

    if (strcmp(new_name, name) != 0) {
        perror("semanage_seuser_get_name");
        exit(1);
    }

    destroy_handle(sh, argv[1]);

    exit(0);
}
