#define IBENDPORT_COUNT 2

#define IBENDPORT_MODULE_SOURCE \
    "(type test_ibendport_t)\n" \
    "(type test_ibendport_second_t)\n" \
    "(roletype object_r test_ibendport_t)\n" \
    "(roletype object_r test_ibendport_second_t)\n" \
    "(ibendportcon mlx4_0 1 (system_u object_r test_ibendport_t ((s0) (s0))))\n" \
    "(ibendportcon mlx4_1 2 (system_u object_r test_ibendport_second_t ((s0) (s0))))\n"

#define IBENDPORT_INV_NAME "mlx4_42"
#define IBENDPORT_INV_PORT 42

#define IBENDPORT_1_NAME "mlx4_0"
#define IBENDPORT_1_PORT 1
#define IBENDPORT_1_CON "system_u:object_r:test_ibendport_t:s0"

#define IBENDPORT_2_NAME "mlx4_1"
#define IBENDPORT_2_PORT 2
#define IBENDPORT_2_CON "system_u:object_r:test_ibendport_second_t:s0"

const char *test_ibendport_module = IBENDPORT_MODULE_SOURCE;
size_t test_ibendport_module_len = (sizeof IBENDPORT_MODULE_SOURCE) - 1;
