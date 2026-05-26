/**
 * @file        cm_sim.h
 * @brief       SIM接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By XGL
 * @date        2021/04/18
 *
 * @defgroup sim sim
 * @ingroup PHONE
 * @{
 */

#ifndef __CM_SIM_H__
#define __CM_SIM_H__

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
/* sim卡模式设置 */
typedef struct
{
    bool sim0_enable;                           /*!< SIM卡0使能 (支持双卡单待，SIM0和SIM1不能同时使能，重启生效） */
    bool sim0_hotswap_enable;                   /*!< SIM卡0热插拔使能 */
    bool sim1_enable;                           /*!< SIM卡1使能 （重启生效） */
    bool sim1_hotswap_enable;                   /*!< SIM卡1热插拔使能 (SIM1不支持热插拔） */
}cm_sim_switch_t;


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

/**
 * @brief 获取SIM卡状态
 *
 * @return
 *   >= 0  - SIM卡状态码，参考AT文档AT+CPIN说明 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details More details
 */
int32_t cm_sim_get_cpin(void);

/**
 * @brief 获取IMSI
 *
 * @param [out] imsi IMSI，长度16字节
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details More details
 */
int32_t cm_sim_get_imsi(char* imsi);

/**
 * @brief 获取ICCID
 *
 * @param [out] iccid ICCID，长度21字节
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details More details
 */
int32_t cm_sim_get_iccid(char* iccid);

/**
 * @brief 获取SIM卡eid信息
 *
 * @param [out] eid eid数据缓存指针，传入缓存大小不可小于21
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details More details
 */
int cm_sim_get_eid(char *eid);

/**
 * @brief 设置SIM卡切换及热插拔模式
 *
 * @param [in] mode 设置参数
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details More details
 */
int cm_sim_set_switch(cm_sim_switch_t mode);

/**
 * @brief 获取SIM卡切换及热插拔模式
 *
 * @param [out] mode 参数
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *
 * @details More details
 */
int cm_sim_get_switch(cm_sim_switch_t *mode);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_SIM_H__ */

/** @}*/
