/**
 * @file iot_sim.h
 * @brief YOPEN 平台 SIM 卡适配器头文件
 * @details 基于 yopen_sim 接口实现 SIM 卡信息获取功能。
 */

#ifndef IOT_SIM_YOPEN_H
#define IOT_SIM_YOPEN_H

#include "../../iot_types.h"

/* 引入 yopen 平台 SIM 卡头文件 */
#include "yopen_sim.h"

/* ============================================================
 * SIM 卡宏定义
 * ============================================================ */

/**
 * @brief 获取SIM卡状态
 * @return SIM卡状态
 */
#define iot_sim_get_state() \
    ((iot_sim_state_t)yopen_sim_get_state())

/**
 * @brief 获取SIM卡ICCID
 * @param[out] iccid ICCID字符串缓冲区
 * @param[in] size 缓冲区大小
 * @return 0 成功，<0 失败
 */
#define iot_sim_get_iccid(iccid, size) \
    yopen_sim_get_iccid((char *)(iccid), (uint32_t)(size))

/**
 * @brief 获取SIM卡IMSI
 * @param[out] imsi IMSI字符串缓冲区
 * @param[in] size 缓冲区大小
 * @return 0 成功，<0 失败
 */
#define iot_sim_get_imsi(imsi, size) \
    yopen_sim_get_imsi((char *)(imsi), (uint32_t)(size))

/**
 * @brief 获取SIM卡电话号码
 * @param[out] num 电话号码字符串缓冲区
 * @param[in] size 缓冲区大小
 * @return 0 成功，<0 失败
 */
#define iot_sim_get_phone_num(num, size) \
    yopen_sim_get_msisdn((char *)(num), (uint32_t)(size))

/**
 * @brief 获取SIM卡PIN状态
 * @return 0 无PIN，1 需要PIN，<0 错误
 */
#define iot_sim_get_pin_status() \
    yopen_sim_get_pin_retries()

#endif /* IOT_SIM_YOPEN_H */
