#include <stdio.h>
#include <selinux/selinux.h> 
#include <selinux/avc.h> 

int main(__attribute__((unused)) int argc, __attribute__((unused)) char **argv) {
    printf("(before open) selinux_status_getenforce %d\n",
        selinux_status_getenforce());

    printf("(before open) selinux_status_policyload %d\n",
        selinux_status_policyload());

    printf("(before open) selinux_status_deny_unknown %d\n",
        selinux_status_deny_unknown());

    printf("(before open) selinux_status_updated %d\n",
        selinux_status_updated());


    printf("selinux_status_open %d\n",
        selinux_status_open(1));

    printf("selinux_status_getenforce %d\n",
        selinux_status_getenforce());

    printf("selinux_status_policyload %d\n",
        selinux_status_policyload());

    printf("selinux_status_deny_unknown %d\n",
        selinux_status_deny_unknown());

    printf("selinux_status_updated %d\n",
        selinux_status_updated());

    printf("selinux_status_close void\n");
    selinux_status_close();

    return 0;
}
