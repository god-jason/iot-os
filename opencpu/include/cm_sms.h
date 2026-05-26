/**
 * @file        cm_sms.h
 * @brief       短信接口
 * @copyright   Copyright © 2023 China Mobile IOT. All rights reserved.
 * @author      By ZH
 * @date        2023/06/25
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
    CM_MSG_MODE_GSM_7 = 0,                              /*编码类型为GSM 7bit*/
    CM_MSG_MODE_GSM_8,                                  /*编码类型为GSM 8bit*/
    CM_MSG_MODE_GSM_UCS2,                                  /*编码类型为GSM UCS2*/
}cm_msg_mode_e;

typedef enum
{
    CM_SIM_ID_0 = 0,                                 /*SIM卡0*/
    CM_SIM_ID_1,                                    /*SIM卡1 */ /*该模组不支持SIM1*/
}cm_sim_id_e;

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
 */
int32_t cm_sms_send_pdu(char *pdu_msg, char *msg_len, cm_msg_mode_e msg_mode, cm_sim_id_e sim_id);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_SMS_H__ */

/** @}*/
