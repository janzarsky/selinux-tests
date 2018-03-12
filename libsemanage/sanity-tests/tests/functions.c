#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdarg.h>

#include <semanage/handle.h>
#include <semanage/semanage.h>

#include <CUnit/Basic.h>

#include "functions.h"

int result;

semanage_handle_t *sh = NULL;

void stderr_callback(void *varg, semanage_handle_t *sh, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);

    fprintf(stderr, "\n");

    va_end(args);
}

void handle_create(void) {
    sh = semanage_handle_create();
    
    CU_ASSERT_PTR_NOT_NULL(sh);

    semanage_msg_set_callback(sh, stderr_callback, NULL);
}

void handle_destroy(void) {
    semanage_handle_destroy(sh);
}

void connect(void) {
    CU_ASSERT(semanage_connect(sh) >= 0);
}

void disconnect(void) {
    CU_ASSERT(semanage_disconnect(sh) >= 0);
}

void begin_transaction(void) {
    CU_ASSERT(semanage_begin_transaction(sh) >= 0);
}

void commit(void) {
    CU_ASSERT(semanage_commit(sh) >= 0);
}

void setup_handle(level_t level) {
    if (level >= SH_NULL)
        sh = NULL;

    if (level >= SH_HANDLE)
        handle_create();

    if (level >= SH_CONNECT)
        connect();

    if (level >= SH_TRANS)
        begin_transaction();
}

void cleanup_handle(level_t level) {
    if (level >= SH_TRANS)
        commit();

    if (level >= SH_CONNECT)
        disconnect();

    if (level >= SH_HANDLE)
        handle_destroy();

    if (level >= SH_NULL)
        sh = NULL;
}

void setup_handle_invalid_store(level_t level) {
    CU_ASSERT(level >= SH_HANDLE);

    handle_create();
    
    semanage_select_store(sh, "", SEMANAGE_CON_INVALID);

    if (level >= SH_CONNECT)
        connect();

    if (level >= SH_TRANS)
        begin_transaction();
}

int context_compare(semanage_context_t *con, const char *str) {
    char *con_str = NULL;
    int rc;

    CU_ASSERT_PTR_NOT_NULL_FATAL(con);

    CU_ASSERT(semanage_context_to_string(sh, con, &con_str) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(con_str);

    rc = strcmp(con_str, str);

    free(con_str);
    return rc;
}

semanage_seuser_t *get_seuser_nth(int index) {
    int result;
    semanage_seuser_t **records;
    semanage_seuser_t *seuser;
    unsigned int count;

    if (index == I_NULL)
        return NULL;

    result = semanage_seuser_list(sh, &records, &count);
    
    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_FATAL(count >= index + 1);

    seuser = records[index];

    for (unsigned int i = 0; i < count; i++) {
        if (i != index)
            semanage_seuser_free(records[i]);
    }

    return seuser;
}

semanage_seuser_t *get_seuser_new() {
    int result;
    semanage_seuser_t *seuser;

    result = semanage_seuser_create(sh, &seuser);
    
    CU_ASSERT_FATAL(result >= 0);

    return seuser;
}

semanage_seuser_key_t *get_seuser_key_nth(int index) {
    semanage_seuser_key_t *key;
    semanage_seuser_t *seuser;
    int result;

    if (index == I_NULL)
        return NULL;

    seuser = get_seuser_nth(index);

    result = semanage_seuser_key_extract(sh, seuser, &key);

    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

semanage_seuser_key_t *get_seuser_key_from_str(const char *str) {
    semanage_seuser_key_t *key;
    int result;

    if (str == NULL)
        return NULL;

    result = semanage_seuser_key_create(sh, str, &key);

    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

void add_local_seuser(int seuser_index) {
    semanage_seuser_t *seuser;
    semanage_seuser_key_t *key = NULL;

    CU_ASSERT_FATAL(seuser_index != I_NULL);

    seuser = get_seuser_nth(seuser_index);

    CU_ASSERT_FATAL(semanage_seuser_key_extract(sh, seuser, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    CU_ASSERT_FATAL(semanage_seuser_modify_local(sh, key, seuser) >= 0);

    semanage_seuser_key_free(key);
    semanage_seuser_free(seuser);
}

void delete_local_seuser(int seuser_index) {
    semanage_seuser_key_t *key = NULL;

    CU_ASSERT_FATAL(seuser_index != I_NULL);

    key = get_seuser_key_nth(seuser_index);

    CU_ASSERT_FATAL(semanage_seuser_del_local(sh, key) >= 0);

    semanage_seuser_key_free(key);
}

semanage_bool_t *get_bool_nth(int index) {
    int result;
    semanage_bool_t **records;
    semanage_bool_t *boolean;
    unsigned int count;

    if (index == I_NULL)
        return NULL;

    result = semanage_bool_list(sh, &records, &count);
    
    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_FATAL(count >= index + 1);

    boolean = records[index];

    for (unsigned int i = 0; i < count; i++) {
        if (i != index)
            semanage_bool_free(records[i]);
    }

    return boolean;
}

semanage_bool_t *get_bool_new() {
    int result;
    semanage_bool_t *boolean;

    result = semanage_bool_create(sh, &boolean);
    
    CU_ASSERT_FATAL(result >= 0);

    return boolean;
}

semanage_bool_key_t *get_bool_key_nth(int index) {
    semanage_bool_key_t *key;
    semanage_bool_t *boolean;
    int result;

    if (index == I_NULL)
        return NULL;

    boolean = get_bool_nth(index);

    result = semanage_bool_key_extract(sh, boolean, &key);

    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

semanage_bool_key_t *get_bool_key_from_str(const char *str) {
    semanage_bool_key_t *key;
    int result;

    if (str == NULL)
        return NULL;

    result = semanage_bool_key_create(sh, str, &key);

    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

void add_local_bool(int bool_index) {
    semanage_bool_t *boolean;
    semanage_bool_key_t *key = NULL;

    CU_ASSERT_FATAL(bool_index != I_NULL);

    boolean = get_bool_nth(bool_index);

    CU_ASSERT_FATAL(semanage_bool_key_extract(sh, boolean, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    CU_ASSERT_FATAL(semanage_bool_modify_local(sh, key, boolean) >= 0);
}

void delete_local_bool(int bool_index) {
    semanage_bool_key_t *key = NULL;

    CU_ASSERT_FATAL(bool_index != I_NULL);

    key = get_bool_key_nth(bool_index);

    CU_ASSERT_FATAL(semanage_bool_del_local(sh, key) >= 0);
}

semanage_fcontext_t *get_fcontext_new() {
    int result;
    semanage_fcontext_t *fcontext;

    result = semanage_fcontext_create(sh, &fcontext);
    
    CU_ASSERT_FATAL(result >= 0);

    return fcontext;
}

semanage_fcontext_t *get_fcontext_nth(int index) {
    int result;
    semanage_fcontext_t **records;
    semanage_fcontext_t *fcontext;
    unsigned int count;

    if (index == I_NULL)
        return NULL;

    result = semanage_fcontext_list(sh, &records, &count);
    
    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_FATAL(count >= index + 1);

    fcontext = records[index];

    for (unsigned int i = 0; i < count; i++) {
        if (i != index)
            semanage_fcontext_free(records[i]);
    }

    return fcontext;
}

semanage_fcontext_key_t *get_fcontext_key_nth(int index) {
    semanage_fcontext_key_t *key;
    semanage_fcontext_t *fcontext;
    int result;

    if (index == I_NULL)
        return NULL;

    fcontext = get_fcontext_nth(index);

    result = semanage_fcontext_key_extract(sh, fcontext, &key);

    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

semanage_fcontext_key_t *get_fcontext_key_from_str(const char *str, int type) {
    semanage_fcontext_key_t *key;
    int result;

    if (str == NULL)
        return NULL;

    result = semanage_fcontext_key_create(sh, str, type, &key);

    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

void add_local_fcontext(int fcontext_index) {
    semanage_fcontext_t *fcontext;
    semanage_fcontext_key_t *key = NULL;

    CU_ASSERT_FATAL(fcontext_index != I_NULL);

    fcontext = get_fcontext_nth(fcontext_index);

    CU_ASSERT_FATAL(semanage_fcontext_key_extract(sh, fcontext, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    CU_ASSERT_FATAL(semanage_fcontext_modify_local(sh, key, fcontext) >= 0);
}

void delete_local_fcontext(int fcontext_index) {
    semanage_fcontext_key_t *key = NULL;

    CU_ASSERT_FATAL(fcontext_index != I_NULL);

    key = get_fcontext_key_nth(fcontext_index);

    CU_ASSERT_FATAL(semanage_fcontext_del_local(sh, key) >= 0);
}

#if VERS_CHECK(2, 5)

semanage_module_info_t *get_module_info_nth(int index, semanage_module_info_t **modinfo_free) {
    int result;
    semanage_module_info_t *records = NULL;
    semanage_module_info_t *modinfo;
    int count;

    if (index == I_NULL)
        return NULL;

    result = semanage_module_list_all(sh, &records, &count);
    
    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_FATAL(records != NULL);
    CU_ASSERT_FATAL(count >= index + 1);

    modinfo = semanage_module_list_nth(records, index);

    CU_ASSERT_FATAL(modinfo != NULL);

    for (unsigned int i = 0; i < count; i++) {
        if (i != index) {
            semanage_module_info_t *tmp = semanage_module_list_nth(records, i);
            semanage_module_info_destroy(sh, tmp);
        }
    }

    *modinfo_free = records;

    return modinfo;
}

semanage_module_key_t *get_module_key_nth(int index) {
    semanage_module_key_t *modkey = NULL;
    semanage_module_info_t *modinfo = NULL;
    semanage_module_info_t *modinfo_free = NULL;
    const char *name = NULL;
    uint16_t priority = 0;

    CU_ASSERT(semanage_module_key_create(sh, &modkey) >= 0);

    modinfo = get_module_info_nth(index, &modinfo_free);

    CU_ASSERT(semanage_module_info_get_name(sh, modinfo, &name) >= 0);
    CU_ASSERT(semanage_module_key_set_name(sh, modkey, name) >= 0);

    CU_ASSERT(semanage_module_info_get_priority(sh, modinfo, &priority) >= 0);
    CU_ASSERT(semanage_module_key_set_priority(sh, modkey, priority) >= 0);

    free(modinfo_free);

    return modkey;
}

int module_exists(const char *name) {
    const char *bools_path = "/sys/fs/selinux/booleans/";
    char *path = NULL;
    struct stat buf;

    path = malloc(strlen(bools_path) + strlen(name) + 1);

    CU_ASSERT_PTR_NOT_NULL(path);

    strcpy(path, bools_path);
    strcat(path, name);

    if (stat(path, &buf) != 0) {
        CU_ASSERT(errno == ENOENT);

        return 0;
    }
    else {
        return 1;
    }
}

#endif

semanage_port_t *get_port_nth(int index) {
    int result;
    semanage_port_t **records;
    semanage_port_t *port;
    unsigned int count;

    if (index == I_NULL)
        return NULL;

    result = semanage_port_list(sh, &records, &count);
    
    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_FATAL(count >= index + 1);

    port = records[index];

    for (unsigned int i = 0; i < count; i++) {
        if (i != index)
            semanage_port_free(records[i]);
    }

    return port;
}

semanage_port_key_t *get_port_key_nth(int index) {
    semanage_port_key_t *key;
    semanage_port_t *port;
    int result;

    if (index == I_NULL)
        return NULL;

    port = get_port_nth(index);

    result = semanage_port_key_extract(sh, port, &key);

    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

void add_local_port(int port_index) {
    semanage_port_t *port;
    semanage_port_key_t *key = NULL;

    CU_ASSERT_FATAL(port_index != I_NULL);

    port = get_port_nth(port_index);

    CU_ASSERT_FATAL(semanage_port_key_extract(sh, port, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    CU_ASSERT_FATAL(semanage_port_modify_local(sh, key, port) >= 0);
}

void delete_local_port(int port_index) {
    semanage_port_key_t *key = NULL;

    CU_ASSERT_FATAL(port_index != I_NULL);

    key = get_port_key_nth(port_index);

    CU_ASSERT_FATAL(semanage_port_del_local(sh, key) >= 0);
}

semanage_iface_t *get_iface() {
    semanage_iface_t *iface = NULL;
    semanage_context_t *ifcon = NULL;
    semanage_context_t *msgcon = NULL;

    CU_ASSERT_FATAL(semanage_iface_create(sh, &iface) >= 0);

    CU_ASSERT_FATAL(semanage_iface_set_name(sh, iface, "asdf") >= 0);

    CU_ASSERT_FATAL(semanage_context_from_string(sh, "system_u:object_r:tmp_t:s0", &ifcon) >= 0);
    CU_ASSERT_FATAL(semanage_iface_set_ifcon(sh, iface, ifcon) >= 0);

    CU_ASSERT_FATAL(semanage_context_from_string(sh, "system_u:object_r:tmp_t:s0", &msgcon) >= 0);
    CU_ASSERT_FATAL(semanage_iface_set_msgcon(sh, iface, msgcon) >= 0);

    return iface;
}

semanage_iface_key_t *get_iface_key() {
    semanage_iface_key_t *key;

    CU_ASSERT_FATAL(semanage_iface_key_create(sh, "asdf", &key) >= 0);

    return key;
}

void add_local_iface(int index) {
    semanage_iface_t *iface;
    semanage_iface_key_t *key = NULL;
    char name[50] = {};

    snprintf(name, 50, "asdf%d", index);

    iface = get_iface();

    CU_ASSERT_FATAL(semanage_iface_set_name(sh, iface, name) >= 0);

    CU_ASSERT_FATAL(semanage_iface_key_extract(sh, iface, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    CU_ASSERT_FATAL(semanage_iface_modify_local(sh, key, iface) >= 0);
}

void delete_local_iface(int index) {
    semanage_iface_key_t *key = NULL;
    char name[50] = {};

    snprintf(name, 50, "asdf%d", index);

    CU_ASSERT_FATAL(semanage_iface_key_create(sh, name, &key) >= 0);

    CU_ASSERT_FATAL(semanage_iface_del_local(sh, key) >= 0);
}

semanage_node_t *get_node() {
    semanage_node_t *node = NULL;
    semanage_context_t *con = NULL;

    CU_ASSERT_FATAL(semanage_node_create(sh, &node) >= 0);

    CU_ASSERT_FATAL(semanage_node_set_addr(sh, node, SEMANAGE_PROTO_IP4,
                                           "127.0.0.1") >= 0);

    CU_ASSERT_FATAL(semanage_node_set_mask(sh, node, SEMANAGE_PROTO_IP4,
                                           "255.255.255.0") >= 0);

    semanage_node_set_proto(node, SEMANAGE_PROTO_IP4);

    CU_ASSERT_FATAL(semanage_context_from_string(sh,
                                      "system_u:object_r:tmp_t:s0", &con) >= 0);
    CU_ASSERT_FATAL(semanage_node_set_con(sh, node, con) >= 0);

    return node;
}

semanage_node_key_t *get_node_key() {
    semanage_node_key_t *key;

    CU_ASSERT_FATAL(semanage_node_key_create(sh, "127.0.0.1", "255.255.255.0",
                                             SEMANAGE_PROTO_IP4, &key) >= 0);

    return key;
}

void add_local_node(int index) {
    semanage_node_t *node;
    semanage_node_key_t *key = NULL;
    char addr[50] = {};

    CU_ASSERT(index >= 0 && index <= 255);

    snprintf(addr, 50, "192.168.0.%d", index);

    node = get_node();

    CU_ASSERT_FATAL(semanage_node_set_addr(sh, node, SEMANAGE_PROTO_IP4,
                                           addr) >= 0);
    CU_ASSERT_FATAL(semanage_node_set_mask(sh, node, SEMANAGE_PROTO_IP4,
                                           "255.255.0.0") >= 0);

    CU_ASSERT_FATAL(semanage_node_key_extract(sh, node, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    CU_ASSERT_FATAL(semanage_node_modify_local(sh, key, node) >= 0);
}

void delete_local_node(int index) {
    semanage_node_key_t *key = NULL;
    char addr[50] = {};

    CU_ASSERT(index >= 0 && index <= 255);

    snprintf(addr, 50, "192.168.0.%d", index);

    CU_ASSERT_FATAL(semanage_node_key_create(sh, addr, "255.255.0.0",
                                             SEMANAGE_PROTO_IP4, &key) >= 0);

    CU_ASSERT_FATAL(semanage_node_del_local(sh, key) >= 0);
}

semanage_user_t *get_user_nth(int index) {
    int result;
    semanage_user_t **records;
    semanage_user_t *user;
    unsigned int count;

    if (index == I_NULL)
        return NULL;

    result = semanage_user_list(sh, &records, &count);
    
    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_FATAL(count >= index + 1);

    user = records[index];

    for (unsigned int i = 0; i < count; i++) {
        if (i != index)
            semanage_user_free(records[i]);
    }

    return user;
}

semanage_user_key_t *get_user_key_nth(int index) {
    semanage_user_key_t *key;
    semanage_user_t *user;
    int result;

    if (index == I_NULL)
        return NULL;

    user = get_user_nth(index);

    result = semanage_user_key_extract(sh, user, &key);

    CU_ASSERT_FATAL(result >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    return key;
}

void add_local_user(int user_index) {
    semanage_user_t *user;
    semanage_user_key_t *key = NULL;

    CU_ASSERT_FATAL(user_index != I_NULL);

    user = get_user_nth(user_index);

    CU_ASSERT_FATAL(semanage_user_key_extract(sh, user, &key) >= 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key);

    CU_ASSERT_FATAL(semanage_user_modify_local(sh, key, user) >= 0);
}

void delete_local_user(int user_index) {
    semanage_user_key_t *key = NULL;

    CU_ASSERT_FATAL(user_index != I_NULL);

    key = get_user_key_nth(user_index);

    CU_ASSERT_FATAL(semanage_user_del_local(sh, key) >= 0);
}

#ifdef FEATURE_INFINIBAND

void add_local_ibendport(int index) {
    semanage_ibendport_t *ibendport;
    semanage_ibendport_key_t *key = NULL;
    semanage_context_t *con = NULL;
    char name[50];

    CU_ASSERT(index >= 0 && index <= 255);

    snprintf(name, 50, "ibendport%d", index);

    CU_ASSERT_FATAL(semanage_ibendport_key_create(sh, name, 0, &key) >= 0);

    CU_ASSERT_FATAL(semanage_ibendport_create(sh, &ibendport) >= 0);
    CU_ASSERT_FATAL(semanage_ibendport_set_ibdev_name(sh, ibendport,
                                                      name) >= 0);
    semanage_ibendport_set_port(ibendport, 0);
    CU_ASSERT_FATAL(semanage_context_from_string(sh,
                                      "system_u:object_r:bin_t:s0", &con) >= 0);
    CU_ASSERT_FATAL(semanage_ibendport_set_con(sh, ibendport, con) >= 0);

    CU_ASSERT_FATAL(semanage_ibendport_modify_local(sh, key, ibendport) >= 0);

    semanage_context_free(con);
    semanage_ibendport_key_free(key);
    semanage_ibendport_free(ibendport);
}

void delete_local_ibendport(int index) {
    semanage_ibendport_key_t *key = NULL;
    char name[50];

    CU_ASSERT(index >= 0 && index <= 255);

    snprintf(name, 50, "ibendport%d", index);

    CU_ASSERT_FATAL(semanage_ibendport_key_create(sh, name, 0, &key) >= 0);

    CU_ASSERT_FATAL(semanage_ibendport_del_local(sh, key) >= 0);

    semanage_ibendport_key_free(key);
}

#endif
