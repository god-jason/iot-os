/**
 * @file        cm_eloop.h
 * @brief       Event Loop模块接口定义
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By wangk
 * @date        2021/08/06
 *
 * @defgroup eloop eloop
 * @ingroup SYS
 * @{
 */

#ifndef __CMO_ELOOP_H__
#define __CMO_ELOOP_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/** 表示timeout不超时 */
#define CM_ELOOP_WAIT_FOREVER 0xFFFFFFFF

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** Event Loop对象句柄 */
struct cm_eloop_t;
typedef struct cm_eloop_t *cm_eloop_handle_t;

/** Event对象句柄 */
struct cm_eloop_event_t;
typedef struct cm_eloop_event_t *cm_eloop_event_handle_t;

/**
  Event Loop回调函数类型
  @param event_obj      [in] 事件对象
  @param cb_param       [in] 用户参数
*/
typedef void (*cm_eloop_event_cb)(cm_eloop_event_handle_t event, void *cb_param);

/**
  Event Loop回调函数类型
  @param event_obj      [in] 事件对象
  @param cb_param       [in] 用户参数
  @param cb_param2      [in] 用户参数2
*/
typedef void (*cm_eloop_event_cb_ext)(cm_eloop_event_handle_t event, void *cb_param, void *cb_param2);

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

/**
 * @brief 取得系统默认Event Loop对象
 *
 * @return
 *   Event Loop对象
 *
 * @details 返回系统默认Event Loop对象
 */
cm_eloop_handle_t cm_eloop_default(void);

/**
 * @brief 创建Event Loop对象
 *
 * @param [in] max_num 最大可注册的事件个数
 *
 * @return  
 *   Event Loop对象或NULL
 *
 * @details 创建Event Loop对象(分配资源)
 */
cm_eloop_handle_t cm_eloop_create(uint32_t max_num);

/**
 * @brief 删除Event Loop对象
 *
 * @param [in] eloop 要释放的Event Loop对象
 *
 * @details 删除Event Loop对象(释放资源)
 */
void cm_eloop_delete(cm_eloop_handle_t eloop);

/**
 * @brief 等待Event触发或者超时时间到达
 *
 * @param [in] eloop      Event Loop对象
 * @param [in] timeout    超时时间(ms), CM_ELOOP_WAIT_FOREVER表示不超时
 *
 * @return  
 *   > 0 - 已触发的事件个数 \n
 *   = 0 - 超时 \n
 *   < 0 - 出错
 *
 * @details
 *   挂起当前线程, 等待注册的任意Event触发或者超时时间到达 \n
 *   如果有Event触发, 将在处理完对应的事件回调函数后再返回
 */
int32_t cm_eloop_wait_event(cm_eloop_handle_t eloop, uint32_t timeout);

/**
 * @brief 注册事件到Event Loop
 *
 * @param [in] eloop    要运行的Event Loop对象
 * @param [in] event_cb 事件回调函数
 * @param [in] cb_param 回调函数用户参数
 *
 * @return  
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 * @details 
 */
cm_eloop_event_handle_t cm_eloop_register_event(cm_eloop_handle_t eloop, cm_eloop_event_cb event_cb, void *cb_param);

/**
 * @brief 从Event Loop注销事件
 *
 * @param [in] event    要注销的事件
 *
 * @details 
 */
void cm_eloop_unregister_event(cm_eloop_event_handle_t event);

/**
 * @brief 发送事件到Event Loop
 *
 * @param [in] event  要发送的事件
 *
 * @return  
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 * @details 
 */
int32_t cm_eloop_post_event(cm_eloop_event_handle_t event);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_ELOOP_H__ */

/** @}*/
