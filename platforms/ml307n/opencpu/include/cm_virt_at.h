/**
 * @file        cm_virt_at.h
 * @brief       虚拟AT通道接口
 * @copyright   Copyright © 2023 China Mobile IOT. All rights reserved.
 * @author      By ZH
 * @date        2023/06/25
 *
 * @defgroup virt_at virt_at
 * @ingroup virt_at
 * @{
 */

#ifndef __CM_VIRT_AT_H__
#define __CM_VIRT_AT_H__

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

typedef enum{
    CM_VIRT_AT_OK,      /*!< 成功*/
    CM_VIRT_AT_TOUT,    /*!< 超时*/
} cm_virt_at_evt_e;

typedef struct {
	int32_t event;          /*!< 触发事件消息:不支持,默认全是CM_VIRT_AT_OK*/
    int32_t rsp_len;        /*!< 消息长度*/
    uint8_t *rsp;           /*!< 消息*/
    void *user_param;       /*!< 用户参数*/
} cm_virt_at_param_t;

/** 虚拟AT回调函数*/
typedef void (*cm_virt_at_cb)(cm_virt_at_param_t *param);

/** URC消息回调函数*/
typedef void (*cm_virt_at_urc_cb)(uint8_t *urc, int32_t urc_len);

/**
 * @brief 注册urc上报回调函数
 *
 * @param [in] prefix urc消息前缀
 * @param [in] cb     urc回调函数
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败
 *
 * @details More details
 */
int32_t cm_virt_at_urc_cb_reg(const uint8_t *prefix, cm_virt_at_urc_cb cb);

/**
 * @brief 取消注册urc上报回调函数
 *
 * @param [in] prefix urc消息前缀
 * @param [in] cb     urc回调函数
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败
 *
 * @details More details
 */
int32_t cm_virt_at_urc_cb_dereg(const uint8_t *prefix, cm_virt_at_urc_cb cb);

/**
 * @brief 通过AT虚拟通道异步获取数据
 *
 * @param [in] at_cmd     发送的AT指令
 * @param [in] cb         消息回调函数
 * @param [in] user_param 用户参数
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败
 *
 * @details More details
 */
int32_t cm_virt_at_send_async(const uint8_t *at_cmd, cm_virt_at_cb cb, void *user_param);

/**
 * @brief 通过AT虚拟通道同步获取数据
 *
 * @param [in] at_cmd   发送的AT指令
 * @param [out] rsp     读取的响应消息
 * @param [out] rsp_len 读取的响应消息长度, 输入时候建议赋值成申请的空间大小，预防踩内存
 * @param [in] timeout  消息超时时间
 *
 * @return
 *   = 0  - 成功 \n
 *   < 0  - 失败
 *
 * @details 同步接口不建议使用等待时间超过5s以上的指令  例如：AT+COPS=?等指令
 *          使用时确保rsp空间足够存放响应内容，防止踩内存
 */
int32_t cm_virt_at_send_sync(const uint8_t *at_cmd, uint8_t *rsp, int32_t *rsp_len, uint32_t timeout);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_VIRT_AT_H__ */

/** @}*/
