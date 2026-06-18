/**
 * @file iot_sys.h
 * @brief ML307N 平台系统管理适配器头文件
 * @details 封装 ML307N 平台 cm_sys 接口，提供统一的系统信息获取宏定义。
 *          支持获取IMEI、SN、固件版本号、模组型号等信息。
 */

#ifndef IOT_SYS_ML307N_H
#define IOT_SYS_ML307N_H

#include "../../iot_types.h"
#include "cm_sys.h"

/**
 * @brief 获取IMEI（国际移动设备识别码）
 * @param[out] imei IMEI字符串缓冲区（>=16字节）
 * @return >=0 IMEI字符串长度，<0 失败
 */
#define iot_sys_get_imei(imei) \
    cm_sys_get_imei((char *)(imei))

/**
 * @brief 获取SN（序列号）
 * @param[out] sn SN字符串缓冲区（>=21字节）
 * @return >=0 SN字符串长度，<0 失败
 */
#define iot_sys_get_sn(sn) \
    cm_sys_get_sn((char *)(sn))

/**
 * @brief 获取固件版本号
 * @param[out] version 版本号字符串缓冲区（>=33字节）
 * @return >=0 版本号字符串长度，<0 失败
 */
#define iot_sys_get_version(version) \
    cm_sys_get_version((char *)(version))

/**
 * @brief 获取模组型号
 * @param[out] model 型号字符串缓冲区（>=33字节）
 * @return >=0 型号字符串长度，<0 失败
 */
#define iot_sys_get_model(model) \
    cm_sys_get_model((char *)(model))

/**
 * @brief 获取OpenCPU SDK版本号
 * @param[out] version 版本号字符串缓冲区（>=33字节）
 * @return >=0 版本号字符串长度，<0 失败
 */
#define iot_sys_get_opencpu_version(version) \
    cm_sys_get_opencpu_version((char *)(version))

/**
 * @brief 获取底层软件版本号
 * @param[out] version 版本号字符串缓冲区（>=33字节）
 * @return >=0 版本号字符串长度，<0 失败
 */
#define iot_sys_get_modem_version(version) \
    cm_sys_get_modem_version((char *)(version))

/**
 * @brief 获取设备唯一标识
 * @param[out] id 设备唯一标识缓冲区（>=33字节）
 * @return >=0 标识字符串长度，<0 失败
 */
#define iot_sys_get_device_id(id) \
    cm_sys_get_device_id((char *)(id))

#endif /* IOT_SYS_ML307N_H */
