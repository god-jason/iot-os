#ifndef IOT_LOG_LINUX_H
#define IOT_LOG_LINUX_H

#include <stdio.h>

#define LOG(fmt, ...) printf("[iot] %s():%d " fmt "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif