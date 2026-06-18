/**
 * @file        cm_sms.h
 * @brief       短信
 * @copyright   Copyright © 2025 China Mobile IOT. All rights reserved.
 * @author      By cmiot4594
 * @date        2025/05/21
 *
 * @defgroup sms sms
 * @ingroup sms
 * @{
 */


#ifndef __CM_SMS_H__
#define __CM_SMS_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/


/****************************************************************************
 * Public Types
 ****************************************************************************/


/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

typedef enum
{
    CM_MSG_MODE_GSM_7 = 0,                              /*编码类型为GSM 7bit 用于ASCII 文本*/
    CM_MSG_MODE_GSM_8,                                  /*编码类型为GSM 8bit 用于任意二进制数据,TXT模式不支持8bit编码*/
    CM_MSG_MODE_UCS2,                                   /*编码类型为UCS2     用于所有 Unicode 字符（含中文）*/
}cm_msg_mode_e;

typedef enum
{
    CM_SIM_ID_0 = 0,                                 /*SIM卡0*/
    CM_SIM_ID_1,                                    /*SIM卡1 */ /*该模组不支持SIM1*/
}cm_sim_id_e;

#define CM_MAIN_SIM CM_SIM_ID_0  //即使切换主卡到物理卡槽SIM1，软件层面仍将CM_SIM_ID_0视作主卡

/**
 * @brief 短信发送结果回调函数类型
 *
 * @param [in] result    发送结果: = 0 成功, < 0 失败
 * @param [in] user_data 用户传入的自定义数据
 */
typedef void (*cm_sms_send_callback)(int32_t result, void *user_data);

/**
 * @brief 发送txt模式短信
 *
 * @param [in] txt_msg 发送的txt短信数据
 * @param [in] sms_num  接收短信的电话号码
 * @param [in] msg_mode 文本模式参数
 * @param [in] sim_id   SIM卡号编号
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败
 *
 * @details More details
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，推荐使用cm_sms_send_txt_v2接口。
 */
int32_t cm_sms_send_txt(char *txt_msg, char *sms_num, cm_msg_mode_e msg_mode, cm_sim_id_e sim_id);

/**
 * @brief 发送pdu模式短信
 *
 * @param [in] pdu_msg  pdu编码信息
 * @param [in] msg_len  pdu编码信息长度
 * @param [in] msg_mode 文本模式参数
 * @param [in] sim_id   SIM卡号编号
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败
 *
 * @details More details
 *
 * @deprecated 不建议使用，该接口仅用于老版本兼容，推荐使用cm_sms_send_pdu_v2接口。
 */
int32_t cm_sms_send_pdu(char *pdu_msg, char *msg_len, cm_msg_mode_e msg_mode, cm_sim_id_e sim_id);

/**
 * @brief 发送txt模式短信
 *
 * @param [in] txt_msg   发送的txt短信数据
 * @param [in] sms_num   接收短信的电话号码
 * @param [in] msg_mode  文本模式参数  自动识别参数，接口会根据传入的text数据自动判断编码类型
 * @param [in] sim_id    SIM卡号编号   仅支持主卡发送，即传CM_MAIN_SIM（CM_SIM_ID_0）
 * @param [in] callback  发送结果回调（可为NULL，表示不关心结果）
 * @param [in] user_data 回调时带回的用户数据
 *
 * @return
 *   = 0  - 成功提交发送任务 \n
 *   < 0  - 提交失败
 *
 * @details 实际发送结果通过回调函数异步返回,TXT和PDU短信最多10条并发
 */
int32_t cm_sms_send_txt_v2(char *txt_msg, char *sms_num, cm_msg_mode_e msg_mode,
                        cm_sim_id_e sim_id, cm_sms_send_callback callback, void *user_data);

/**
 * @brief 发送pdu模式短信
 *
 * @param [in] pdu_msg   pdu编码信息
 * @param [in] msg_len   tpdu编码信息长度
 * @param [in] msg_mode  文本模式参数   保留参数，pdu消息无需再转码，此参数无实际作用
 * @param [in] sim_id    SIM卡号编号    自动识别参数，接口会根据主卡识别发送短信
 * @param [in] callback  发送结果回调（可为NULL，表示不关心结果）
 * @param [in] user_data 回调时带回的用户数据
 *
 * @return
 *   = 0  - 成功提交发送任务 \n
 *   < 0  - 提交失败
 *
 * @details 实际发送结果通过回调函数异步返回,TXT和PDU短信最多10条并发
 */
int32_t cm_sms_send_pdu_v2(char *pdu_msg, char *msg_len, cm_msg_mode_e msg_mode,
                        cm_sim_id_e sim_id, cm_sms_send_callback callback, void *user_data);

/**
 * @brief 设置短信中心号码
 *
 * @param [in] smsc    短信中心号码，格式如 "+8613800100500"
 * @param [in] sim_id  SIM卡号编号
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败
 */
int32_t cm_sms_set_smsc(char *smsc, cm_sim_id_e sim_id);

/**
 * @brief 获取短信中心号码
 *
 * @param [out] smsc    用于存储短信中心号码的缓冲区
 * @param [in]  len     缓冲区长度，建议不小于20字节
 * @param [in]  sim_id  SIM卡号编号
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败
 */
int32_t cm_sms_get_smsc(char *smsc, uint8_t len, cm_sim_id_e sim_id);


#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_SMS_H__ */

/** @}*/
