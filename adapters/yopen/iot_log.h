#ifndef IOT_LOG_YOPEN_H
#define IOT_LOG_YOPEN_H

#include "../../iot_types.h"
#include "yopen_debug.h"

#define iot_log_debug(fmt, ...) \
    yopen_trace("[D] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define iot_log_info(fmt, ...) \
    yopen_trace("[I] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define iot_log_warn(fmt, ...) \
    yopen_trace("[W] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define iot_log_error(fmt, ...) \
    yopen_trace("[E] %s:%d " fmt "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define iot_log_set_level(level) \
    do { \
        yopen_debug_log_owner_level _lvl = (level == IOT_LOG_LEVEL_DEBUG) ? YOPEN_DEV_LOG_LEVEL_DEBUG : \
                                           (level == IOT_LOG_LEVEL_INFO) ? YOPEN_DEV_LOG_LEVEL_INFO : \
                                           (level == IOT_LOG_LEVEL_WARN) ? YOPEN_DEV_LOG_LEVEL_WARNING : \
                                           YOPEN_DEV_LOG_LEVEL_ERROR; \
        yopen_debug_set_log_level(YOPEN_DEV_LOG_ID_APP, _lvl); \
    } while(0)

#define iot_wdt_feed() \
    yopen_debug_feed_wdt()

#define iot_wdt_set_timeout(timeout) \
    yopen_debug_cfg_wdt(timeout)

#define iot_assert(condition) \
    YOPEN_ASSERT(condition)

#endif
