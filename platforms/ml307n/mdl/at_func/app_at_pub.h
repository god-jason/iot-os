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

#ifndef __APP_AT_PUB_H__
#define __APP_AT_PUB_H__

#include "os.h"
#include "lwip/ip.h"
#include "lwip/sockets.h"
#include "slog_print.h"
#include "at_parser.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#ifdef USE_APP_QUECTEL_AT_COMMAND
/* 将接口统一定义成发送ERROR */
//#define at_ctrl_send_rsp_err(ch_id, errCode)  at_ctrl_send_rsp(ch_id, (uint8_t *)"\r\nERROR\r\n", strlen("\r\nERROR\r\n"))
#endif

#if 1
#define APP_AT_PRINT(format, ...)        slogPrintf(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_AT, format"\r\n", ##__VA_ARGS__)
#define APP_AT_PRINT_DEBUG(format, ...)  slogPrintf(SLOG_LEVEL_DEBUG, SLOG_PRINT_SUBMDL_AT, format, ##__VA_ARGS__)
#define APP_AT_PRINT_INFO(format, ...)   slogPrintf(SLOG_LEVEL_INFO, SLOG_PRINT_SUBMDL_AT, format, ##__VA_ARGS__)
#define APP_AT_PRINT_WARN(format, ...)   slogPrintf(SLOG_LEVEL_WARN, SLOG_PRINT_SUBMDL_AT, format, ##__VA_ARGS__)
#define APP_AT_PRINT_ERROR(format, ...)  slogPrintf(SLOG_LEVEL_ERROR, SLOG_PRINT_SUBMDL_AT, format, ##__VA_ARGS__)
#else
#define APP_AT_PRINT(format, ...)        osPrintf(format"\r\n", ##__VA_ARGS__)
#define APP_AT_PRINT_DEBUG(format, ...)  osPrintf(format, ##__VA_ARGS__)
#define APP_AT_PRINT_INFO(format, ...)   osPrintf(format, ##__VA_ARGS__)
#define APP_AT_PRINT_WARN(format, ...)   osPrintf(format, ##__VA_ARGS__)
#define APP_AT_PRINT_ERROR(format, ...)  osPrintf(format, ##__VA_ARGS__)
#endif

#define APP_AT_MALLOC                  osMalloc
#define APP_AT_FREE                    osFree
#define APP_AT_SNPRINTF                osSnprintf

#define APP_AT_PORTNAME_USBAT "usbat"
#define APP_AT_PORTNAME_UART  "uart"

#define APP_AT_CHAR_ESC           (27) // 0x1B ESC Escape
#define APP_AT_CHAR_SUB           (26) // 0x1A SUB Substitute
#define APP_AT_CHAR_CR            (13) // 0x0D CR
#define APP_AT_CHAR_LF            (10) // 0x0A LF

#define APP_AT_HOST_NAME_LEN      (DNS_MAX_NAME_LENGTH) //考虑到域名长度最大支持 256 字节, 包括 '\0'
#define APP_AT_EXIT_TRANSPARENT_CMD          "+++"      /* 退出透传模式的命令 */
#define APP_AT_ENTER_INDICATE_MODE           "\r\n> "   /* 进入 ‘>' 模式 */

#define APP_AT_TimerStart(timer, cb, ms, parameter, flag)           APP_AT_TimerStartRelaxed(timer, cb, ms, osWaitForever, parameter, flag)
#define APP_AT_TimerStart_Wakeup(timer, cb, ms, parameter, flag)    APP_AT_TimerStartRelaxed(timer, cb, ms, 0, parameter, flag)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum
{
    APP_AT_RECV_END_BY_NONE = 0,
    APP_AT_RECV_END_BY_SUB,
    APP_AT_RECV_END_BY_LEN,
}APP_AT_DataRecvEndMethod;        /* 从 AT 接收数据结束的方式 */

typedef enum
{
    APP_AT_DATA_SEND_NONE = 0,
    APP_AT_DATA_SEND_TRANSPARENT, /* 透传模式发送 */
    APP_AT_DATA_SEND_WITHOUT_LEN, /* AT 命令中不带长度的参数, 输入 AT 命令后立即发送, 不需要申请 buf 对数据缓存 */
    APP_AT_DATA_SEND_WITH_LEN,         /* AT 命令中带长度的参数, 需要申请 buf 对数据先缓存, 发送的条件是发送长度达到 AT 命令指定的长度或者发送超时时间到达 */
}APP_AT_DataSendMethod;        /* 发送数据的方式 */

typedef enum
{
    APP_AT_DATA_RECV_OK                 = 0,            /* 接收完成(成功) */
    APP_AT_DATA_RECV_GOON               = -1,           /* 继续接收 */
    APP_AT_DATA_RECV_CANCEL             = -2,           /* 取消接收 */
    APP_AT_DATA_RECV_LEN_ERR            = -3,           /* 接收的长度不对 */
    APP_AT_DATA_RECV_MEM_ERR            = -4,           /* 内存错误 */
    APP_AT_DATA_RECV_BUF_ERR            = -5,           /* 接收 BUF 错误 */
    APP_AT_DATA_RECV_PROCESS_ERR        = -6,           /* 处理错误 */
} APP_AT_DataRecvErrCode; /* AT 透传数据接收错误码 */

typedef enum
{
    APP_AT_DATA_SEND_FORMAT_ASCII       = 0,            /* ASCII 字符串(原始数据) */
    APP_AT_DATA_SEND_FORMAT_HEX,                        /* HEX 字符串 */
} APP_AT_DataSendFormat;        /* 数据发送格式(仅限非透传模式) */

typedef enum
{
    APP_AT_SOCKET_CLOSE_MODE_WATI_DATA = 0,      /* 等待发送缓存区数据发送完毕后，关闭 TCP 连接 */
    APP_AT_SOCKET_CLOSE_MODE_NOW,                /* 立即关闭不等待缓存区数据发送完毕 */
    APP_AT_SOCKET_CLOSE_MODE_WAIT_2MSL,          /* 等待 2MSL (Maximum Segment Lifetime, 最大分段) 后关闭 */
    APP_AT_SOCKET_CLOSE_MODE_SEND_RST,           /* 向服务器发送 RST 消息重置连接后关闭 */
}APP_AT_SocketCloseMode; /* TCP/IP 关闭模式 */

typedef struct
{
    struct osSlistNode node;    /* 链表节点 */
    uint32_t        readLen;    /* 一个链表节点已读取的数据长度(针对 TCP 时有用) */
    uint32_t        totalLen;   /* 一个链表节点存储的数据总长度 */
    char            *buf;       /* 存储数据 buf */
}APP_AT_DataList;     /* 数据存储链表 */

typedef struct
{
    uint32_t        totalLen;    /* 存储数据的总长度 */
    uint32_t        writeLen;    /* 存储数据的当前实际存储的长度(指针指向的位置) */
    char            *buf;        /* 存储数据的 buf */
}APP_AT_DataTransparent;     /* 以透传模式或以">(输入数据)" 形式发送数据时存储缓存 buf */


/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
char *APP_AT_ChannelName_Get(uint8_t channelId);
int8_t APP_AT_BindCid(int32_t fd, uint8_t cid);
struct netif* APP_AT_NetifGetByCid(uint8_t cid);
int8_t APP_AT_GetIfNameByCid(uint8_t cid, struct ifreq *ifname);
int32_t APP_AT_FindSMSEnd(const char * source, uint32_t len);
void APP_AT_ResponseSendOK(uint8_t channelId);
void APP_AT_ResponseSendFail(uint8_t channelId, uint16_t errCode);
at_errno_t APP_AT_Bytes2Hex(char *bytes, uint32_t bytesLen, char *hex, uint32_t hexLen);
at_errno_t APP_AT_Hex2Bytes(const char *str_hex, uint32_t hex_size, uint8_t *byte_buff, uint32_t buff_size, uint32_t *data_size);
uint32_t APP_AT_Escape2Bytes(const char *str_src, uint32_t src_size, char *str_dest, uint32_t dest_size);
int8_t APP_AT_TimerStartRelaxed(struct osTimer **timer, void (*cb)(void *parameter), uint32_t ms, uint32_t relaxed_ms, void *parameter, uint8_t flag);
int8_t APP_AT_TimerRestart(struct osTimer **timer, uint32_t ms);
void APP_AT_TimerStop(struct osTimer **timer);
bool_t APP_AT_NetIsActive(uint8_t cid);
APP_AT_DataTransparent* APP_AT_DataBufMalloc(uint16_t len);
void APP_AT_DataBufFree(APP_AT_DataTransparent *dataBuf);
void APP_AT_DataBufClean(APP_AT_DataTransparent *dataBuf);
void APP_AT_DataListFree(osSlist_t *atHdrList);
void APP_AT_DataInList(osSlist_t *atHdrList, char *recvBuf, uint32_t recvLen);
APP_AT_DataList* APP_AT_DataOutList(osSlist_t *atHdrList);
void APP_AT_DataInBuf(APP_AT_DataTransparent *dataBuf, char *recvBuf, uint32_t recvLen);
APP_AT_DataTransparent* APP_AT_DataOutBuf(APP_AT_DataTransparent *dataBuf);
int8_t APP_AT_DataRecvProcess(uint8_t sendFormat, uint8_t endMethod, APP_AT_DataTransparent *dataBuf, char *recvBuf, uint32_t recvLen);
void APP_AT_UrcCallback(const char *cmd, uint32_t cmdLen);
void APP_AT_SocketFcntl(int fd);
int8_t APP_AT_SocketSetSockoptLinger(int fd, uint8_t closeMode);
int8_t APP_AT_SetSockoptRecvBuf(int fd);
int8_t APP_AT_SetSockoptReuseaddr(int fd);
int8_t APP_AT_SetSockoptTimeout(int fd);
int8_t APP_AT_SocketListen(int fd, int backlog);
int8_t APP_AT_SocketConnect(int fd, ip_addr_t remoteIp, uint16_t remotePort);
int8_t APP_AT_SocketBind(int fd, ip_addr_t remoteIp, uint16_t localPort);
int8_t APP_AT_SocketClose(int fd);
int32_t APP_AT_SocketCreate(ip_addr_t remoteIp, uint8_t domainType);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __APP_AT_PUB_H__ */
