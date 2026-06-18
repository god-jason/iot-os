#ifndef IOT_LOG_ML307N_H
#define IOT_LOG_ML307N_H

#include "../../iot_types.h"
#include "os_debug.h"

#define iot_log_debug(fmt, ...) \
    os_kprintf("[D] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define iot_log_info(fmt, ...) \
    os_kprintf("[I] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define iot_log_warn(fmt, ...) \
    os_kprintf("[W] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define iot_log_error(fmt, ...) \
    os_kprintf("[E] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define iot_log_set_level(level) \
    do {} while(0)

#endif
