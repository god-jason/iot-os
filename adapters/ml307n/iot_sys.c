/**
 * @file iot_sys.c
 * @brief ML307N 平台系统管理适配器实现
 * @details 基于 cm_sys 接口实现系统管理功能封装，
 *          提供统一的系统信息获取函数。
 */

#include "iot_sys.h"

/**
 * @brief 获取IMEI（国际移动设备识别码）
 * @param[out] imei IMEI字符串缓冲区（>=16字节）
 * @return >=0 IMEI字符串长度，<0 失败
 */
int32_t iot_sys_get_imei(char *imei)
{
    return cm_sys_get_imei(imei);
}

/**
 * @brief 获取SN（序列号）
 * @param[out] sn SN字符串缓冲区（>=21字节）
 * @return >=0 SN字符串长度，<0 失败
 */
int32_t iot_sys_get_sn(char *sn)
{
    return cm_sys_get_sn(sn);
}

/**
 * @brief 获取固件版本号
 * @param[out] version 版本号字符串缓冲区（>=33字节）
 * @return >=0 版本号字符串长度，<0 失败
 */
int32_t iot_sys_get_version(char *version)
{
    return cm_sys_get_version(version);
}

/**
 * @brief 获取模组型号
 * @param[out] model 型号字符串缓冲区（>=33字节）
 * @return >=0 型号字符串长度，<0 失败
 */
int32_t iot_sys_get_model(char *model)
{
    return cm_sys_get_model(model);
}

/**
 * @brief 获取OpenCPU SDK版本号
 * @param[out] version 版本号字符串缓冲区（>=33字节）
 * @return >=0 版本号字符串长度，<0 失败
 */
int32_t iot_sys_get_opencpu_version(char *version)
{
    return cm_sys_get_opencpu_version(version);
}

/**
 * @brief 获取底层软件版本号
 * @param[out] version 版本号字符串缓冲区（>=33字节）
 * @return >=0 版本号字符串长度，<0 失败
 */
int32_t iot_sys_get_modem_version(char *version)
{
    return cm_sys_get_modem_version(version);
}

/**
 * @brief 获取设备唯一标识
 * @param[out] id 设备唯一标识缓冲区（>=33字节）
 * @return >=0 标识字符串长度，<0 失败
 */
int32_t iot_sys_get_device_id(char *id)
{
    return cm_sys_get_device_id(id);
}