/**
 * @file iot_sim.h
 * @brief ML307N 平台 SIM卡适配器头文件
 * @details 封装 ML307N 平台 cm_sim 接口，提供统一的 SIM卡操作宏定义。
 *          支持获取SIM卡状态、IMSI、ICCID、电话号码等信息。
 */

#ifndef IOT_SIM_ML307N_H
#define IOT_SIM_ML307N_H

#include "../../iot_types.h"
#include "cm_sim.h"

/** @brief SIM卡类型枚举重定义 */
#define iot_sim_type_t          cm_sim_type_e

/** @brief SIM卡状态枚举重定义 */
#define iot_sim_state_t         cm_sim_state_e

/**
 * @brief 查询SIM卡状态
 * @return SIM卡状态枚举值
 */
#define iot_sim_get_state() \
    cm_sim_get_state()

/**
 * @brief 获取SIM卡类型
 * @return SIM卡类型枚举值
 */
#define iot_sim_get_sim_type() \
    cm_sim_get_sim_type()

/**
 * @brief 获取IMSI（国际移动用户识别码）
 * @param[out] imsi IMSI字符串缓冲区（>=16字节）
 * @return >=0 IMSI字符串长度，<0 失败
 */
#define iot_sim_get_imsi(imsi) \
    cm_sim_get_imsi((char *)(imsi))

/**
 * @brief 获取ICCID（集成电路卡识别码）
 * @param[out] iccid ICCID字符串缓冲区（>=21字节）
 * @return >=0 ICCID字符串长度，<0 失败
 */
#define iot_sim_get_iccid(iccid) \
    cm_sim_get_iccid((char *)(iccid))

/**
 * @brief 获取电话号码
 * @param[out] phone_num 电话号码字符串缓冲区（>=21字节）
 * @return >=0 电话号码字符串长度，<0 失败
 */
#define iot_sim_get_phone_num(phone_num) \
    cm_sim_get_phone_num((char *)(phone_num))

/**
 * @brief 获取SIM卡标识
 * @param[out] sim_id 标识缓冲区（>=21字节）
 * @return >=0 标识字符串长度，<0 失败
 */
#define iot_sim_get_sim_id(sim_id) \
    cm_sim_get_sim_id((char *)(sim_id))

/**
 * @brief 获取运营商名称
 * @param[out] oper_name 运营商名称缓冲区（>=33字节）
 * @return >=0 运营商名称长度，<0 失败
 */
#define iot_sim_get_oper_name(oper_name) \
    cm_sim_get_oper_name((char *)(oper_name))

#endif /* IOT_SIM_ML307N_H */
