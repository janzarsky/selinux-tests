#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <semanage/handle.h>
#include <semanage/semanage.h>

#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#if defined(VERS_MAJOR) && defined(VERS_MINOR)

#define VERS_CHECK(major, minor) \
    ((VERS_MAJOR > major) || ((VERS_MAJOR == major) && (VERS_MINOR >= minor)))

#else

#define VERS_CHECK(major, minor) 1

#endif

#if defined(VERS_MAJOR) && defined(VERS_MINOR) && defined(VERS_RELEASE)

#define VERS_CHECK_REL(major, minor, release) \
    ((VERS_MAJOR == major) && (VERS_MINOR == minor) && (VERS_RELEASE >= release))

#else

#define VERS_CHECK_REL(major, minor, release) 0

#endif

#if VERS_CHECK(2, 7) || VERS_CHECK_REL(2, 5, 9)
#define FEATURE_INFINIBAND
#endif

typedef enum { SH_NULL, SH_HANDLE, SH_CONNECT, SH_TRANS } level_t;

void setup_handle(level_t level);
void cleanup_handle(level_t level);
void setup_handle_invalid_store(level_t level);

void helper_handle_create(void);
void helper_handle_destroy(void);
void helper_connect(void);
void helper_disconnect(void);
void helper_begin_transaction(void);
void helper_commit(void);

int context_compare(semanage_context_t *con, const char *str);

// forks, runs code and checks signal returned by child process
#define CU_ASSERT_SIGNAL(CODE,SIGNAL) \
    do { \
        pid_t pid = fork(); \
        if (pid == 0) { \
            CODE; \
            exit(0); \
        } else { \
            int stat_val; \
            wait(&stat_val); \
            CU_ASSERT_FALSE(WIFEXITED(stat_val)); \
            CU_ASSERT_EQUAL(WTERMSIG(stat_val), SIGNAL); \
        } \
    } while(0)

#define CU_ASSERT_CONTEXT(CON1,CON2,EQUAL) \
    do { \
        char *str; \
        char *str2; \
        CU_ASSERT_PTR_NOT_NULL(CON1); \
        CU_ASSERT_PTR_NOT_NULL(CON2); \
        CU_ASSERT(semanage_context_to_string(sh, CON1, &str) >= 0); \
        CU_ASSERT(semanage_context_to_string(sh, CON2, &str2) >= 0); \
        if (EQUAL) {\
            CU_ASSERT_STRING_EQUAL(str, str2); \
        } else {\
            CU_ASSERT_STRING_NOT_EQUAL(str, str2); \
        } \
    } while(0)

#define CU_ASSERT_CONTEXT_EQUAL(CON1,CON2) CU_ASSERT_CONTEXT(CON1,CON2,1)
#define CU_ASSERT_CONTEXT_NOT_EQUAL(CON1,CON2) CU_ASSERT_CONTEXT(CON1,CON2,0)

#define I_NULL  -1
#define I_FIRST  0
#define I_SECOND 1
#define I_THIRD  2

// seusers
semanage_seuser_t *get_seuser_nth(int index);
semanage_seuser_t *get_seuser_new();

semanage_seuser_key_t *get_seuser_key_nth(int index);
semanage_seuser_key_t *get_seuser_key_from_str(const char *str);

void add_local_seuser(int seuser_index);
void delete_local_seuser(int seuser_index);

// booleans
semanage_bool_t *get_bool_nth(int index);
semanage_bool_t *get_bool_new();

semanage_bool_key_t *get_bool_key_nth(int index);
semanage_bool_key_t *get_bool_key_from_str(const char *str);

void add_local_bool(int bool_index);
void delete_local_bool(int bool_index);

// fcontext
semanage_fcontext_t *get_fcontext_nth(int index);
semanage_fcontext_t *get_fcontext_new();

semanage_fcontext_key_t *get_fcontext_key_nth(int index);
semanage_fcontext_key_t *get_fcontext_key_from_str(const char *str, int type);

void add_local_fcontext(int fcontext_index);
void delete_local_fcontext(int fcontext_index);

// module
#if VERS_CHECK(2, 5)
semanage_module_info_t *get_module_info_nth(int index, semanage_module_info_t **modinfo_free);
semanage_module_key_t *get_module_key_nth(int index);
int module_exists(const char *name);
#endif

// port
semanage_port_t *get_port_nth(int index);
semanage_port_key_t *get_port_key_nth(int index);

void add_local_port(int port_index);
void delete_local_port(int port_index);

// iface
semanage_iface_t *get_iface();
semanage_iface_key_t *get_iface_key();

void add_local_iface(int index);
void delete_local_iface(int index);

// node

semanage_node_t *get_node();
semanage_node_key_t *get_node_key();

void add_local_node(int index);
void delete_local_node(int index);

// user

semanage_user_t *get_user_nth(int index);
semanage_user_key_t *get_user_key_nth(int index);

void add_local_user(int index);
void delete_local_user(int index);

// ibendport
#ifdef FEATURE_INFINIBAND
void add_local_ibendport(int index);
void delete_local_ibendport(int index);
#endif

#endif
