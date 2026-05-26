/**
 * @file        cm_asocket.h
 * @brief       异步socket接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By wangk
 * @date        2021/08/05
 *
 * @defgroup asocket asocket
 * @ingroup NET
 * @{
 */

#ifndef __CMO_ASOCKET_H__
#define __CMO_ASOCKET_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
// 包含平台特定的socket头文件
#include "lwip/sockets.h" // lwip socket
#include "cm_eloop.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** 异步socket事件类型 */
typedef enum
{
    CM_ASOCKET_EV_NONE = 0,
    // responses
    CM_ASOCKET_EV_CONNECT_OK,   /**< connect response ok */
    CM_ASOCKET_EV_CONNECT_FAIL, /**< connect response fail */
    // indications
    CM_ASOCKET_EV_RECV_IND,   /**< data receve indication(通过ioctlsocket(FIONREAD)获取接收缓存中可读的数据长度) */
    CM_ASOCKET_EV_SEND_IND,   /**< send buffer available indication(表示发送缓冲区剩余空间超过水位线) */
    CM_ASOCKET_EV_ACCEPT_IND, /**< accept indication */
    CM_ASOCKET_EV_ERROR_IND,  /**< error indication(通过getsockopt(SOL_SOCKET, SO_ERROR)获取socket错误码) */
    CM_ASOCKET_EV_SEQ_IND,     /**< 空口回传 */
    CM_ASOCKET_EV_DROP_IND    /**< data drop*/
} cm_asocket_event_e;

/**
  异步socket事件回调函数类型
  @param sock       [in] 发生事件的socket描述符
  @param event      [in] 事件类型
  @param cb_param   [in] 用户参数(创建socket时指定)
*/
typedef void (*cm_asocket_event_cb)(int sock, cm_asocket_event_e event, void *ev_param, void *cb_param);

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
 * @brief 获取异步socket模块运行的Event Loop
 *
 * @return  
 *   本模块运行的Event Loop
 *
 * @details 获取本模块运行的Event Loop(初始化传入或者系统默认Event Loop)
 */
cm_eloop_handle_t cm_asocket_eloop(void);

/**
 * @brief 开启异步socket
 *
 * @param [in] domain       socket协议族
 * @param [in] type         socket类型
 * @param [in] protocol     协议类型 
 * @param [in] event_cb     异步事件回调函数
 * @param [in] cb_param     用户参数(将作为参数传入event_cb)
 *
 * @return  
 *   >= 0 - 异步socket描述符 \n
 *   = -1 - 失败(可通过errno获取具体错误类型)
 *
 * @details 创建异步socket(分配资源, 返回socket描述符)
 */
int cm_asocket_open(int domain, int type, int protocol,
                    cm_asocket_event_cb event_cb, void *cb_param);

/**
 * @brief 异步socket connect操作
 *
 * @param [in] sock     异步socket描述符
 * @param [in] addr     远端地址信息
 * @param [in] addr_len size of addr
 *
 * @return  
 *   = 0  - 成功 \n
 *   = -1 - 失败(可通过errno获取具体错误类型,注意EINPROGRESS的处理)
 *
 * @details 请求建立连接
 */
int cm_asocket_connect(int sock, const struct sockaddr *addr, socklen_t addr_len);

/**
 * @brief 关闭异步socket
 *
 * @param [in] sock   要关闭的异步socket描述符
 *
 * @return  
 *   = 0 - 成功 \n
 *   < 0 - 失败(可通过errno获取具体错误类型)
 *
 * @details 关闭异步socket(释放资源)
 */
int cm_asocket_close(int sock);


/// 其他接口全部使用标准socket接口
#define cm_asocket_bind bind
#define cm_asocket_shutdown shutdown
#define cm_asocket_getpeername getpeername
#define cm_asocket_getsockname getsockname
#define cm_asocket_setsockopt setsockopt
#define cm_asocket_getsockopt getsockopt
#define cm_asocket_recv recv
#define cm_asocket_recvmsg recvmsg
#define cm_asocket_recvfrom recvfrom
#define cm_asocket_send send
#define cm_asocket_sendmsg sendmsg
#define cm_asocket_sendto sendto
#define cm_asocket_ioctl ioctlsocket

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_ASOCKET_H__ */

/** @}*/
