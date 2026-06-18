#ifndef IOT_LOG_H
#define IOT_LOG_H

#include "os.h"
#include "cm_os.h"
#include "cm_sys.h"

// 日志定义，打印函数史，行号，以及换行回车
#undef LOG
#define LOG(fmt, ...) cm_log_printf("[iot] %s():%d " fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)


#endif /* IOT_LOG_H */