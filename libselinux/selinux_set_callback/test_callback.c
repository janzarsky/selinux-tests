#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <selinux/selinux.h>
#include <selinux/avc.h>
#include <selinux/label.h>

int validate_counter = 0;

int my_log(int type, const char *fmt, ...) {
    printf("function my_log, type: %d, fmt: %s\n", type, fmt);
    return 0;
}

int my_audit(void *auditdata, security_class_t cls, char *msgbuf, size_t msgbufsize) {
    printf("function my_audit, auditdata: %p, cls: %u, msgbuf: %s, msgbufsize: %lu\n", auditdata, cls, msgbuf, msgbufsize);
    return 0;
}

int my_validate(char **ctx) {
    if (validate_counter++ == 0)
        printf("function my_validate, ctx: %p\n", (void *) ctx);

    return 0;
}

int my_setenforce(int enforcing) {
    printf("function my_setenforce, enforcing: %d\n", enforcing);
    return 0;
}

int my_policyload(int seqno) {
    printf("function my_policyload, seqno: %d\n", seqno);
    return 0;
}

int main (int argc, char **argv) {
    int exit_code = 0;

    // LOG
    printf("setting LOG callback\n");
    selinux_set_callback(SELINUX_CB_LOG, (union selinux_callback) my_log);
    
    if (selinux_get_callback(SELINUX_CB_LOG).func_log != my_log) {
        printf("ERROR: selinux_get_callback() does not match\n");
        exit_code = 1;
    }

    // AUDIT
    printf("setting AUDIT callback\n");
    selinux_set_callback(SELINUX_CB_AUDIT, (union selinux_callback) my_audit);
    
    if (selinux_get_callback(SELINUX_CB_AUDIT).func_audit != my_audit) {
        printf("ERROR: selinux_get_callback() does not match\n");
        exit_code = 1;
    }

    printf("calling avc_audit to call audit and log functions\n");
    
    avc_init("", NULL, NULL, NULL, NULL);
    
    struct security_id ssid = { "asdf", 5 };
    struct security_id tsid = { "asdf", 5 };
    struct av_decision avd = { 1, 0, 1, 0, 0, 0 };

    avc_audit(&ssid, &tsid, 0, 1, &avd, 0, NULL);

    // VALIDATE
    printf("setting VALIDATE callback\n");
    selinux_set_callback(SELINUX_CB_VALIDATE, (union selinux_callback) my_validate);
    
    if (selinux_get_callback(SELINUX_CB_VALIDATE).func_validate != my_validate) {
        printf("ERROR: selinux_get_callback() does not match\n");
        exit_code = 1;
    }

    struct selabel_handle *hnd = NULL;
    struct selinux_opt selabel_option [] = {
        { SELABEL_OPT_VALIDATE, (char *) 1 }
    };

    hnd = selabel_open(SELABEL_CTX_FILE, selabel_option, 1);
    selabel_close(hnd);
    
    // SETENFORCE
    printf("setting SETENFORCE callback\n");
    selinux_set_callback(SELINUX_CB_SETENFORCE, (union selinux_callback) my_setenforce);
    
    if (selinux_get_callback(SELINUX_CB_SETENFORCE).func_setenforce != my_setenforce) {
        printf("ERROR: selinux_get_callback() does not match\n");
        exit_code = 1;
    }

    int enforcing = security_getenforce();

    printf("calling security_setenforce to call setenforce function\n");

    if (enforcing == 1) {
        security_setenforce(0);
        security_setenforce(1);
    }
    else {
        security_setenforce(1);
        security_setenforce(0);
    }

    // triggers callbacks
    avc_has_perm_noaudit(&ssid, &tsid, 0, 1, NULL, &avd);
    
    // POLICYLOAD
    printf("setting POLICYLOAD callback\n");
    selinux_set_callback(SELINUX_CB_POLICYLOAD, (union selinux_callback) my_policyload);
    
    if (selinux_get_callback(SELINUX_CB_POLICYLOAD).func_policyload != my_policyload) {
        printf("ERROR: selinux_get_callback() does not match\n");
        exit_code = 1;
    }

    selinux_mkload_policy(1);

    // triggers callbacks
    avc_has_perm_noaudit(&ssid, &tsid, 0, 1, NULL, &avd);

    return exit_code;
}
