/**
 * @file iot_sim.c
 * @brief ML307N 平台 SIM卡适配器实现
 * @details 基于 cm_sim 接口实现 SIM卡功能封装，
 *          提供统一的 SIM卡操作函数。
 */

#include "iot_sim.h"

/**
 * @brief 查询SIM卡状态
 * @return SIM卡状态枚举值
 */
iot_sim_state_t iot_sim_get_state(void)
{
    return cm_sim_get_state();
}

/**
 * @brief 获取SIM卡类型
 * @return SIM卡类型枚举值
 */
iot_sim_type_t iot_sim_get_sim_type(void)
{
    return cm_sim_get_sim_type();
}

/**
 * @brief 获取IMSI（国际移动用户识别码）
 * @param[out] imsi IMSI字符串缓冲区（>=16字节）
 * @return >=0 IMSI字符串长度，<0 失败
 */
int32_t iot_sim_get_imsi(char *imsi)
{
    return cm_sim_get_imsi(imsi);
}

/**
 * @brief 获取ICCID（集成电路卡识别码）
 * @param[out] iccid ICCID字符串缓冲区（>=21字节）
 * @return >=0 ICCID字符串长度，<0 失败
 */
int32_t iot_sim_get_iccid(char *iccid)
{
    return cm_sim_get_iccid(iccid);
}

/**
 * @brief 获取电话号码
 * @param[out] phone_num 电话号码字符串缓冲区（>=21字节）
 * @return >=0 电话号码字符串长度，<0 失败
 */
int32_t iot_sim_get_phone_num(char *phone_num)
{
    return cm_sim_get_phone_num(phone_num);
}

/**
 * @brief 获取SIM卡标识
 * @param[out] sim_id 标识缓冲区（>=21字节）
 * @return >=0 标识字符串长度，<0 失败
 */
int32_t iot_sim_get_sim_id(char *sim_id)
{
    return cm_sim_get_sim_id(sim_id);
}

/**
 * @brief 获取运营商名称
 * @param[out] oper_name 运营商名称缓冲区（>=33字节）
 * @return >=0 运营商名称长度，<0 失败
 */
int32_t iot_sim_get_oper_name(char *oper_name)
{
    return cm_sim_get_oper_name(oper_name);
}