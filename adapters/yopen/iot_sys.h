/**
 * @file iot_sys.h
 * @brief YOPEN 平台系统信息适配器头文件
 * @details 提供系统信息获取功能封装。
 */

#ifndef IOT_SYS_YOPEN_H
#define IOT_SYS_YOPEN_H

#include "../../iot_types.h"

/* ============================================================
 * 系统信息宏定义
 * ============================================================ */

/**
 * @brief 获取SDK版本号
 * @return SDK版本字符串
 */
const char* iot_sys_get_sdk_version(void);

/**
 * @brief 获取软件版本号
 * @return 软件版本字符串
 */
const char* iot_sys_get_software_version(void);

/**
 * @brief 获取硬件版本号
 * @return 硬件版本字符串
 */
const char* iot_sys_get_hardware_version(void);

/**
 * @brief 获取设备型号
 * @return 设备型号字符串
 */
const char* iot_sys_get_device_model(void);

/**
 * @brief 获取设备ID
 * @param[out] dev_id 设备ID缓冲区
 * @param[in] size 缓冲区大小
 * @return 0 成功，<0 失败
 */
int32_t iot_sys_get_device_id(char *dev_id, uint32_t size);

/**
 * @brief 获取芯片名称
 * @return 芯片名称字符串
 */
const char* iot_sys_get_chip_name(void);

/**
 * @brief 获取CPU频率
 * @return CPU频率(Hz)
 */
uint32_t iot_sys_get_cpu_freq(void);

/**
 * @brief 获取系统运行时间(毫秒)
 * @return 运行时间(毫秒)
 */
uint64_t iot_sys_get uptime_ms(void);

/**
 * @brief 系统复位
 * @return 0 成功，<0 失败
 */
int32_t iot_sys_reboot(void);

#endif /* IOT_SYS_YOPEN_H */
