#ifndef _BUS_MUX_API_H
#define _BUS_MUX_API_H

#include <stdint.h>
#include <stdbool.h>
#include "bus_config.h"

typedef struct BUS_TunnelStruct     BUS_Tunnel;
typedef struct BUS_MuxHandleStruct  BUS_MuxHandle;
typedef struct BUS_MuxChannelStruct BUS_MuxChannel;

typedef enum {
    BUSMUX_OK                       = 0,        // 操作成功
    BUSMUX_ERR                      = -1,       // 未知错误
    BUSMUX_ERR_BUSY                 = -2,       // 设备忙
    BUSMUX_ERR_TIMEOUT              = -3,       // 操作超时
    BUSMUX_ERR_UNSUPPORTED          = -4,       // 不支持的操作
    BUSMUX_ERR_PARAMETER            = -5,       // 参数错误
    BUSMUX_ERR_FULL                 = -6,       // 内存满
    BUSMUX_ERR_EMPTY                = -7,       // 链表空
    BUSMUX_ERR_CHANNLE_EXIST        = -8,       // 通道ID已存在
    BUSMUX_ERR_CHANNLE_NOT_EXIST    = -9,       // 通道ID已存在
    BUSMUX_ERR_SPECIFIC             = -10,      // 私有错误的起始编号
    BUSMUX_ERR_INITED               = -11,
    BUSMUX_ERR_UNINITED             = -12,
    BUSMUX_ERR_STATE                = -13,
    BUSMUX_ERR_CHANNEL_INVALID      = -14,      // 通道无效

    BUSMUX_ERR_WRITE_TIMEOUT        = -15,      // 读超时
    BUSMUX_ERR_WRITE_TIMEOUT_2      = (BUSMUX_ERR_WRITE_TIMEOUT - 1),
    BUSMUX_ERR_WRITE_TIMEOUT_3      = (BUSMUX_ERR_WRITE_TIMEOUT - 2),
    BUSMUX_ERR_WRITE_TIMEOUT_4      = (BUSMUX_ERR_WRITE_TIMEOUT - 3),
    BUSMUX_ERR_WRITE_TIMEOUT_5      = (BUSMUX_ERR_WRITE_TIMEOUT - 4),

    BUSMUX_ERR_READ_TIMEOUT         = (BUSMUX_ERR_WRITE_TIMEOUT_5 - 1),      // 写超时
    BUSMUX_ERR_READ_TIMEOUT_2       = (BUSMUX_ERR_READ_TIMEOUT - 1),
    BUSMUX_ERR_READ_TIMEOUT_3       = (BUSMUX_ERR_READ_TIMEOUT - 2),
    BUSMUX_ERR_READ_TIMEOUT_4       = (BUSMUX_ERR_READ_TIMEOUT - 3),
    BUSMUX_ERR_READ_TIMEOUT_5       = (BUSMUX_ERR_READ_TIMEOUT - 4),
    BUSMUX_ERR_READ_CRC             = (BUSMUX_ERR_READ_TIMEOUT - 5),
}BUS_MuxErrCode;

typedef enum {
    BUSMUX_CMD_GET_DATA_NUMBER      = 0,        // 获取通道中还未发送的数据量
    BUSMUX_CMD_FREE_DATA,                       // 释放未发送的数据，并返回未发送的数据地址
    BUSMUX_CMD_SUSPEND,                         // 通道挂起，暂停发送
    BUSMUX_CMD_RESUME,                          // 通道恢复
    BUSMUX_CMD_RESTART,                         // 通道重启
}BUS_MuxCmdCode;

typedef enum {
    BUS_CHANNEL_TRANS_END_EVENT = 0,            // 数据发送成功
    BUS_CHANNEL_DATA_IN_EVENT,                  // 收到数据
    BUS_CHANNEL_TRANS_ERROR_EVENT,              // 发送失败
}BUS_CHANNEL_EVENT;

typedef enum {
    BUS_MUX_EVENT_LINKUP = 0,
    BUS_MUX_EVENT_LINKDOWN,
}BUS_MUX_EVENT;

typedef struct {
    uint8_t         num;                    // 包个数
    uint8_t        *addr;                   // 只用于接收，buffer首地址
    uint8_t        *data[BUSMUX_EVENT_MSG_NUMBER_MAX];
    uint32_t        len[BUSMUX_EVENT_MSG_NUMBER_MAX];
}BUS_EventMsg;

typedef void (*BUS_EventFunc)(BUS_MuxChannel *chl, BUS_EventMsg *msg, void *data, uint32_t event);

typedef void (*BUS_ChannelEventFunc)(BUS_MuxChannel *chl, BUS_EventMsg *msg, void *data, uint32_t event);
typedef void (*BUS_MuxEventFunc)(void *param, BUS_MUX_EVENT event);

/**
 ************************************************************************************
 * @brief           初始化busmux
 *
 * @return          BUSMUX_OK  -  成功
 *                  其他       -  失败
 ************************************************************************************
*/
BUS_MuxHandle *BUS_MuxInit(const char *name, BUS_Tunnel *tunnel);       // 不使用，分支同步升级后删除
BUS_MuxHandle *BUS_MuxCreate(const char *name, BUS_Tunnel *tunnel, BUS_MuxEventFunc callback, void *param);

/**
 ************************************************************************************
 * @brief           去初始化busmux
 *
 * @return          BUSMUX_OK  -  成功
 *                  其他       -  失败
 ************************************************************************************
*/
BUS_MuxErrCode BUS_MuxDeInit(BUS_MuxHandle *hdl);

/**
 ************************************************************************************
 * @brief           插入新的通道
 * 
 * @param hdl       mux句柄
 * @param id        通道ID
 * @param name      通道名称
 * @param priority  通道优先级
 * @param callback  回调
 * @param param     参数
 * 
 * @return          通道handle
 * ************************************************************************************
 */
BUS_MuxChannel *BUS_MuxInsert(BUS_MuxHandle *hdl, uint8_t id, const char *name, uint8_t priority, BUS_EventFunc callback, void *param);

/**
 ************************************************************************************
 * @brief           移除通道
 *
 * @return          BUSMUX_OK  -  成功
 *                  其他       -  失败
 ************************************************************************************
*/
BUS_MuxErrCode BUS_MuxRemove(BUS_MuxChannel *chl);
/**
 ************************************************************************************
 * @brief           发送数据
 *
 * @return          BUSMUX_OK  -  成功
 *                  其他       -  失败
 ************************************************************************************
*/
BUS_MuxErrCode BUS_MuxSend(BUS_MuxChannel *chl, uint8_t *data, uint32_t len, bool immediately);
BUS_MuxErrCode BUS_MuxSendPool(BUS_MuxChannel *chl, uint8_t *data, uint32_t len, uint32_t timeout);
uint32_t BUS_MuxReadPool(BUS_MuxChannel *chl, BUS_EventMsg *msg, uint8_t *data, uint32_t len, uint32_t timeout);
/**
 ************************************************************************************
 * @brief           启动传输
 *
 * @return          BUSMUX_OK  -  成功
 *                  其他       -  失败
 ************************************************************************************
*/
BUS_MuxErrCode BUS_MuxStart(BUS_MuxChannel *chl);

/**
 ************************************************************************************
 * @brief           停止传输
 *
 * @return          BUSMUX_OK  -  成功
 *                  其他       -  失败
 ************************************************************************************
*/
BUS_MuxErrCode BUS_MuxStop(BUS_MuxChannel *chl);

/**
 ************************************************************************************
 * @brief           释放通道，并返回缓存数据
 *
 * @return          BUSMUX_OK  -  成功
 *                  其他       -  失败
 ************************************************************************************
*/
BUS_MuxErrCode BUS_MuxRelease(BUS_MuxChannel *chl, BUS_EventMsg *msg, uint32_t *remain);

#endif