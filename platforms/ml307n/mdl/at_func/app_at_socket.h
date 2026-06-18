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
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */

#ifndef __APP_AT_SOCKET_H__
#define __APP_AT_SOCKET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "lwip/sockets.h"

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum
{
  AT_SOCKET_CB_EVENT_ACCEPT = 0,
  AT_SOCKET_CB_EVENT_SENT = 1,
  AT_SOCKET_CB_EVENT_RECV = 2,
  AT_SOCKET_CB_EVENT_CONNECTED = 3,
  AT_SOCKET_CB_EVENT_POLL = 4,
  AT_SOCKET_CB_EVENT_ERR = 5,
}AT_SocketCallbackEvent; /* 从 LWIP 回调的事件类型, 与 tcp.h 文件中 lwip_event 的枚举值对应 */

typedef enum
{
    AT_SOCKET_DATA_MODE_NORMAL           = 0,    /* 普通模式 */
    AT_SOCKET_DATA_MODE_TRANSPARENT = 1,         /* 透传模式 */
    AT_SOCKET_DATA_MODE_STREAM = 2,              /* 流缓存模式 */
    AT_SOCKET_DATA_MODE_PACKAGE = 3,             /* 包缓存模式 */
}AT_SocketDataMode; /* 数据传输模式 */

typedef enum
{
    AT_SOCKET_CMD_UNKNOWN,        /* 不能识别的命令 */
    AT_SOCKET_CMD_CID,            /* cid */
    AT_SOCKET_CMD_ENCODING,       /* encoding */
    AT_SOCKET_CMD_TIMEOUT,        /* timemout */
    AT_SOCKET_CMD_AUTOFREE,       /* autofree */
    AT_SOCKET_CMD_SNDBUF,         /* sndbuf */
    AT_SOCKET_CMD_RCVBUF,         /* rcvbuf */
    AT_SOCKET_CMD_ACKMODE,        /* ackmode */
    AT_SOCKET_CMD_SSL,            /* ssl */
    AT_SOCKET_CMD_MAX
}AT_SocketCmdType; /* 命令类型 */

typedef enum
{
    AT_SOCKET_NAT_CLOSE           = 0,    /* NAT 功能关闭 */
    AT_SOCKET_NAT_OPEN            = 1,    /* NAT 功能打开 */
}AT_SocketNatSwitch;        /* NAT 功能关闭/打开控制 */

typedef enum
{
    AT_SOCKET_CONN_STATE_INITIAL = 0,               /* 初始态(连接未建立) */
    AT_SOCKET_CONN_STATE_CONNECTING = 1,            /* 作为客户端, 正在连接中 */
    AT_SOCKET_CONN_STATE_LISTENING = 2,             /* 作为服务器, 正在监听中 */
    AT_SOCKET_CONN_STATE_LISTENED = 3,              /* 作为服务器, 已监听到客户端发起的连接，服务器正在连接中 */
    AT_SOCKET_CONN_STATE_CONNECTED = 4,             /* 作为客户端, 已连接 */
    AT_SOCKET_CONN_STATE_ACCEPTED = 5,              /* 作为服务器, 已连接 */
    AT_SOCKET_CONN_STATE_ACTIVE_CLOSING = 6,        /* 本端发起的关闭, 关闭中(本端主动关闭) */
    AT_SOCKET_CONN_STATE_PASSIVE_CLOSING = 7,       /* 远端发起的关闭, 关闭中(本端被动关闭) */
    AT_SOCKET_CONN_STATE_ACTIVE_CLOSED = 8,         /* 本端发起的关闭, 已关闭(本端主动关闭) */
    AT_SOCKET_CONN_STATE_PASSIVE_CLOSED = 9,        /* 远端发起的关闭, 已关闭(本端被动关闭) */
}AT_SocketConnState; /* TCP/IP 连接状态 */

typedef enum
{
    AT_SOCKET_CONNECT_NORMAL = 0,           /* 本端正常连接,  建立连接成功 */
    AT_SOCKET_CONNECT_SERVER_TIMEOUT,       /* 服务器端未响应, 建立连接超时 */
    AT_SOCKET_CONNECT_REMOTE_RESET,         /* 收到远端的 RST,  建立连接失败 */
    AT_SOCKET_CONNECT_TRANS_IMEOUT,         /* 数据传输超时关闭 */
    AT_SOCKET_CONNECT_REMOTE_FIN,           /* 收到远端 FIN,  建立连接失败 */
    AT_SOCKET_CONNECT_FAIL_OTHER,           /* 其它原因导致建立连接失败 */
}AT_SocketOpenReason;    /* TCP/IP 建立连接失败的原因 */

typedef enum
{
    AT_SOCKET_CLOSE_NORMAL = 0,          /* 本端正常关闭,  关闭成功 */
    AT_SOCKET_CLOSE_SERVER_TIMEOUT,      /* 服务器端未响应，超时关闭 */
    AT_SOCKET_CLOSE_FAIL_OTHER,          /* 其它原因导致关闭连接失败 */
    AT_SOCKET_CLOSE_REMOTE_RESET,        /* 收到远端的 RST (本端被动关闭) */
    AT_SOCKET_CLOSE_TRANS_IMEOUT,        /* 数据传输超时关闭 */
    AT_SOCKET_CLOSE_REMOTE_FIN,          /* 收到远端的 FIN, 已关闭(本端被动关闭) */
}AT_SocketCloseReason;    /* TCP/IP 关闭连接失败的原因 */

typedef enum
{
    AT_SOCKET_DISCONN_REMOTE_CLOSE = 1,          /* 服务器关闭连接 */
    AT_SOCKET_DISCONN_ABNORMAL,                  /* 连接异常 */
    AT_SOCKET_DISCONN_PDP_DEACT                  /* PDP 去激活 */
}AT_SocketDisconnReason;    /* 连接异常的原因 */

typedef enum
{
    AT_SOCKET_TYPE_NONE           = 0,
    AT_SOCKET_TYPE_TCP,
    AT_SOCKET_TYPE_UDP,
}AT_SocketType;        /* TCP/IP 连接类型 */

typedef enum
{
    AT_SOCKET_URC_MODE_CLOSE       = 0,    /* 关闭 */
    AT_SOCKET_URC_MODE_OPEN        = 1,    /* 打开 */
}AT_SocketUrcMode; /* 数据主动上报模式 */

typedef enum
{
    AT_SOCKET_SEND_FORMAT_ASCII           = 0,    /* ASCII 字符串（原始数据） */
    AT_SOCKET_SEND_FORMAT_HEX,                    /* HEX 字符串 */
    AT_SOCKET_SEND_FORMAT_TRANS,                  /* 带转义的字符串 */
}AT_SocketSendFormat;        /* 数据发送格式 */

typedef enum
{
    AT_SOCKET_RECV_FORMAT_ASCII           = 0,    /* ASCII 字符串（原始数据） */
    AT_SOCKET_RECV_FORMAT_HEX,                    /* HEX 字符串 */
}AT_SocketRecvFormat;        /* 数据接收格式 */

typedef enum
{
    AT_SOCKET_FREE_MODE_NO_NEED_CLOSE           = 0,    /* 连接异常断开后自动释放资源, 无需使用 AT+MIPCLOSE 命令 */
    AT_SOCKET_FREE_MODE_NEED_CLOSE,                     /* 连接异常断开后, 需使用 AT+MIPCLOSE 命令手动释放资源 */
}AT_SocketFreeMode;        /* 连接异常断开后资源释放模式 */

typedef enum
{
    AT_SOCKET_TCP_ACK_NO_URC           = 0,    /* TCP 接收 ACK 包时不上报 URC */
    AT_SOCKET_TCP_ACK_URC,                     /* TCP 接收 ACK 包时上报 URC */
}AT_SocketAckMode;        /* TCP 发送数据时, 接收 ACK 包 URC 上报模式, 默认值0, UDP 无效 */

typedef enum
{
    AT_SOCKET_KEEPALIVE_DISABLE           = 0,    /* 关闭心跳 */
    AT_SOCKET_KEEPALIVE_ENABLE,                   /* 打开心跳 */
}AT_SocketKeepaliveEnable;        /* TCP 心跳使能开关. 默认值0 */

typedef enum
{
    AT_SOCKET_SSL_DISABLE           = 0,    /* 关闭 SSL 连接 */
    AT_SOCKET_SSL_ENABLE,                   /* 打开 SSL 连接 */
}AT_SocketSslEnable;        /* SSL 连接使能开关. 默认值0 */

typedef enum
{
    AT_SOCKET_CLOSE_MODE_WATI_DATA = 0,      /* 等待发送缓存区数据发送完毕后，关闭 TCP 连接 */
    AT_SOCKET_CLOSE_MODE_NOW,                /* 立即关闭不等待缓存区数据发送完毕 */
    AT_SOCKET_CLOSE_MODE_WAIT_2MSL,          /* 等待 2MSL (Maximum Segment Lifetime, 最大分段) 后关闭 */
    AT_SOCKET_CLOSE_MODE_SEND_RST,           /* 向服务器发送 RST 消息重置连接后关闭 */
}AT_SocketCloseMode; /* TCP/IP 关闭模式 */

typedef enum
{
    AT_SOCKET_RAI_NONE = 0,                /* 无信息 */
    AT_SOCKET_RAI_SEND_UL,                 /* 终端发送一个上行包 */
    AT_SOCKET_RAI_SEND_UL_EXPECT_DL,       /* 终端发送一个上行包，并期望接收到一个下行包 */
}AT_SocketSendRai;        /* 发送数据时, 列举帮助信息. 默认值0 */

typedef enum
{
    AT_SOCKET_PRI_FLAG_IPTOS_RELIABILE = 0,      /* IPTOS 可靠性 */
    AT_SOCKET_PRI_FLAG_IPTOS_LOW_DELAY,          /* IPTOS 低延时 */
    AT_SOCKET_PRI_FLAG_IPTOS_THROUGHPUT,         /* IPTOS 吞吐量 */
    AT_SOCKET_PRI_FLAG_IPTOS_LOW_POWER,          /* IPTOS 低消耗 */
}AT_SocketSendPriFlag;        /* 发送数据时, 优先级标志. 默认值0 */

typedef enum
{
    AT_SOCKET_URC_CONNECT_ABNORMAL           = 0,    /* 连接异常提示 */
    AT_SOCKET_URC_TCP_DATA_RECV,                     /* 接收 TCP 数据提示 */
    AT_SOCKET_URC_UDP_DATA_RECV,                     /* 接收 UDP 数据提示 */
    AT_SOCKET_URC_TCP_ACK_RECV,                      /* TCP 接收 ACK 包提示 */
    AT_SOCKET_URC_UDP_ACK_RECV,                      /* UDP 空口回传序列号提示 */
    AT_SOCKET_URC_OVERFLOW_RECV,                     /* 接收数据溢出提示 */
}AT_SocketUrcType;        /* TCP/IP URC上报信息类型 */

typedef enum
{
    AT_SOCKET_SEND_INDICATE_NONE           = 0,
    AT_SOCKET_SEND_INDICATE_WITHOUT_LEN, /* AT 命令中不带长度的参数, 输入 AT 命令后立即发送, 不需要申请 buf 对数据缓存 */
    AT_SOCKET_SEND_INDICATE_WITH_LEN,         /* AT 命令中带长度的参数, 需要申请 buf 对数据先缓存, 发送的条件是发送长度达到 AT 命令指定的长度或者发送超时时间到达 */
}AT_SocketSendIndicateMethod;        /* 以">(输入数据)" 形式发送数据的方式 */

typedef enum
{
    AT_SOCKET_SEND_TRANSPARENT_DISABLE           = 0,
    AT_SOCKET_SEND_TRANSPARENT_ENABLE,
}AT_SocketSendTransparentFlag;        /* 透传模式下发送数据的标志 */

typedef enum
{
    AT_SOCKET_ERR_PARAM_PARSE_FAIL              = 540,        /* AT 命令参数解析失败 */
    AT_SOCKET_ERR_TCPIP_UNKNOWN                 = 550,        /* TCP/IP 未知错误 */
    AT_SOCKET_ERR_TCPIP_UNUSED                  = 551,        /* TCP/IP 未被使用 */
    AT_SOCKET_ERR_TCPIP_USED                    = 552,        /* TCP/IP 已被使用 */
    AT_SOCKET_ERR_TCPIP_UNCONNECT               = 553,        /* TCP/IP 未连接 */
    AT_SOCKET_ERR_SOCKET_CREATE_FAIL            = 554,        /* SOCKET 创建失败 */
    AT_SOCKET_ERR_SOCKET_BIND_FAIL              = 555,        /* SOCKET 绑定失败 */
    AT_SOCKET_ERR_SOCKET_LISTEN_FAIL            = 556,        /* SOCKET 监听失败 */
    AT_SOCKET_ERR_SOCKET_CONNECT_REFUSED        = 557,        /* SOCKET 连接被拒绝 */
    AT_SOCKET_ERR_SOCKET_CONNECT_TIMEOUT        = 558,        /* SOCKET 连接超时 */
    AT_SOCKET_ERR_SOCKET_CONNECT_FAIL           = 559,        /* SOCKET 连接失败（其他异常） */
    AT_SOCKET_ERR_SOCKET_WRITE_ABNORMAL         = 560,        /* SOCKET 写入异常 */
    AT_SOCKET_ERR_SOCKET_READ_ABNORMAL          = 561,        /* SOCKET 读取异常 */
    AT_SOCKET_ERR_SOCKET_ACCEPT_ABNORMAL        = 562,        /* SOCKET 接受异常 */
    AT_SOCKET_ERR_SOCKET_NO_ACT                 = 570,        /* SOCKET 未激活 */
    AT_SOCKET_ERR_PDP_ACT_FAIL                  = 571,        /* PDP 激活失败 */
    AT_SOCKET_ERR_PDP_DEACT_FAIL                = 572,        /* PDP 去激活失败 */
    AT_SOCKET_ERR_APN_NO_CONFIG                 = 575,        /* APN 未配置 */
    AT_SOCKET_ERR_PORT_BUSY                     = 576,        /* 端口忙碌 */
    AT_SOCKET_ERR_UNSUPPORT_IPV4V6              = 577,        /* 不支持的IPV4/IPV6 */
    AT_SOCKET_ERR_DNS_PARSE_FAIL                = 580,        /* DNS 解析失败或错误的 IP 格式 */
    AT_SOCKET_ERR_DNS_BUSY                      = 581,        /* DNS 忙碌 */
    AT_SOCKET_ERR_PING_BUSY                     = 582,        /* PING 忙碌 */
}AT_SocketErrCode;    /* <err> 错误码 */

typedef struct
{
    uint8_t   cid;              /* PDP 上下文 id, 指定当前实例使用的 PDP 上下文 */
    uint8_t   channelId;        /* AT 命令通道号 */
    uint8_t   connectId;
    uint8_t   sendFormat;       /* 数据发送格式 */
    uint8_t   recvFormat;       /* 数据接收格式 */
    uint8_t   sendTimeout;      /* 发送超时时间, 用于 AT 命令输入数据，并采用">"模式输入时. 范围:1~180; 单位:s. 默认值10 */
    uint8_t   freeMode;         /* 连接异常断开后资源释放模式 */
    uint8_t   ackMode;          /* TCP 发送数据时, 接收 ACK 包 URC 上报模式, 默认值0, UDP 无效 */
    uint8_t   connectTimeout;   /* TCP/IP 连接超时时间, 单位:s, 默认值60 */
    uint8_t   sslEnable;        /* SSL 连接使能开关 */
    uint8_t   sslId;            /* SSL 连接上下文编号 */
    uint8_t   closeMode;        /* 关闭连接模式 */
    uint16_t  sendBufSize;      /* 发送缓存大小, 范围:1~1460；单位:字节. 默认值1460, 适用于透传模式下发送缓存的设置 */
    uint16_t  recvBufSize;      /* 接收缓存大小, 范围:1~65535；单位:字节. 默认值4096, 使用缓存模式时有效 */
}AT_SocketParamCfg;     /* 本地通用配置参数(配置一次后永久生效) */

typedef struct
{
    uint8_t   state;            /* TCP/IP 连接状态 */
    uint8_t   accessMode;       /* 数据传输模式 */
    uint8_t   protoType;        /* 连接类型(TCP 还是 UDP) */
    uint8_t   sendFlag;         /* 数据发送标志 */
    uint8_t   sendIndicate;     /* 以">(输入数据)" 形式发送数据的标志 */
    uint8_t   waitTimeoutFlag;  /* 透传模式下, 输入数据超时时间到达的标志. 数值1表示进入定时器开始计时, 超时时间到达直接发送数据 */
    uint8_t   waitIndicate;     /* 透传模式下, 发送数据的标志 */
    uint16_t  sendPacketSize;   /* 透传模式下, 指定单次发送的最大数据长度, 范围：512~1460; 单位:字节. 默认值1024. */
    uint16_t  sendWaittm;       /* 透传模式下, 传入数据未达到<packet_size>指定长度时, 超过<waittm>时间后直接发送,
                                ** 范围:100~2000;单位:ms. 默认值200 */
    uint16_t  sendLen;          /* AT 命令输入的需要发送(send)的数据长度, 单位:字节 */
    struct osTimer *connTimer;  /* TCP/IP 连接超时定时器指针 */
    struct osTimer *indicateTimer; /* TCP/IP >模式超时定时器指针 */
    struct osTimer *transTimer;  /* TCP/IP 透传超时定时器指针 */
}AT_SocketParamData;     /* TCP/IP 发送接收参数(仅对本次连接有效, 关闭连接成功时置初始值) */

typedef struct
{
    uint8_t   recvPkgCount;     /* UDP 包缓存模式时, 数据接收个数 */
    uint16_t  recvTotalLen;     /* TCP 流缓存模式时, 从 LWIP 接收(recvfrom)到的, 缓存区中存放的已有数据长度, 单位:字节 */
    uint32_t  sentByte;         /* 已发送的数据量, 单位:字节 */
    uint32_t  ackedByte;        /* 对端已确认接收到的数据量, 单位:字节 */
    uint32_t  nackByte;         /* 对端还未确认接收的数据量, 单位:字节 */
    uint32_t  receivedByte;     /* 本地已接收到的数据量, 单位:字节 */
}AT_SocketParamStatistics;     /* TCP/IP 发送接收数据信息统计(仅对本次连接有效, 关闭连接成功时置初始值0) */

typedef struct
{
    AT_SocketParamCfg     cfg;
    AT_SocketParamData    data;
    AT_SocketParamStatistics statistics;
}AT_SocketParamLocal;     /* 本地参数信息 */

typedef struct
{
    AT_SocketParamLocal     local;
}AT_SocketParam;     /* 参数管理 */


/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
/**
 ************************************************************************************
 * @brief           ASCII码转成HEX
 *
 * @param[in]       asciiStr         需要转换的ASCII码字符串指针
 * @param[in]       hexStr           转换后的HEX字符串指针
 * @param[in]       asciiLen         需要转换的ASCII码字符串长度
 *
 * @return          void
 ************************************************************************************
*/
void AT_SocketMipCfgSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketMipCfgTest(uint8_t channelId);
void AT_SocketMipTkaSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketMipTkaRead(uint8_t channelId);
void AT_SocketMipTkaTest(uint8_t channelId);
void AT_SocketMipOpenSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketMipOpenTest(uint8_t channelId);
void AT_SocketMipCloseSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketMipCloseTest(uint8_t channelId);
void AT_SocketMipSendTransparentSet(uint8_t channelId, char *paramIn, uint16_t paramLen);
void AT_SocketMipSendSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketMipSendTest(uint8_t channelId);
void AT_SocketMipReadSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketMipReadTest(uint8_t channelId);
void AT_SocketMipModeSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketMipModeTest(uint8_t channelId);
void AT_SocketMipStateSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketMipStateRead(uint8_t channelId);
void AT_SocketMipSackSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketMipSackTest(uint8_t channelId);
int8_t AT_SocketInit();
void AT_SocketDeinit();

#ifdef __cplusplus
}
#endif
#endif

