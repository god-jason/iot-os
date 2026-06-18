/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */

#ifndef __SVC_USIM_H__
#define __SVC_USIM_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define SVC_USIM_ICCID_SIZE (20)
#define SVC_USIM_PHONE_NUMBER_SIZE (20)
#define SVC_USIM_IMSI_SIZE (16)
/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
/**
 * @brief 卡插拔信息上报，对应消息  SVC_EVT_CMUSLOT_IND
 * @param slot，卡槽 取值范围 0,1
 * @param slot_state, 卡状态 0 拔卡 1 插卡
 */
typedef struct {
    uint8_t  slot;
    uint8_t  slot_state;
} SVC_USIM_Cmuslot;

/**
 * @brief 卡ICCID上报，对应消息  SVC_EVT_CMUSLOT_IND
 * @param iccid，iccid 值20个字符
 */
typedef struct {
    char  iccid[SVC_USIM_ICCID_SIZE+1]; // iccid 20位数字，多1为了字符串结束符
} SVC_USIM_IccidInfo;

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief 获取SIM卡ICCID值 发送AT+ICCID命令
 *
 * @param[out] p_iccid 至少SVC_USIM_ICCID_SIZE大小空间，用于存放ICCID字符
 * @param[in] buf_len  描述p_iccid空间大小
 *
 * @return  osOK：成功，其他值：失败
 * @note   同步接口会阻塞任务，禁止在AT命令处理函数中调用
 ************************************************************************************
*/
int svc_usim_read_iccid(char *p_iccid, uint32_t buf_len);

/**
 ************************************************************************************
 * @brief 获取SIM卡电话号码 发送AT+CNUM命令
 *
 * @param[out] p_phonenumber 至少SVC_USIM_PHONE_NUMBER_SIZE大小空间，用于存放电话号码字符
 * @param[in] buf_len  描述p_phonenumber空间大小
 *
 * @return  osOK：成功，其他值：失败
 * @note   同步接口会阻塞任务，禁止在AT命令处理函数中调用
 ************************************************************************************
*/
int svc_usim_read_phonenumber(char *p_phonenumber, uint32_t buf_len);
/**
 ************************************************************************************
 * @brief 获取SIM卡IMSI
 *
 * @param[out] p_imsi 至少SVC_USIM_IMSI_SIZE大小空间，用于存放IMSI
 * @param[in] buf_len  描述p_imsi空间大小
 *
 * @return  osOK：成功，其他值：失败
 * @note   同步接口会阻塞任务，禁止在AT命令处理函数中调用
 ************************************************************************************
*/
int svc_usim_read_imsi(char *p_imsi, uint32_t buf_len);
#ifdef __cplusplus
}
#endif
#endif /* __SVC_USIM_H__ */
