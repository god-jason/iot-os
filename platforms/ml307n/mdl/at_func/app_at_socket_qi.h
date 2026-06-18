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

#ifndef __APP_AT_SOCKET_QI_H__
#define __APP_AT_SOCKET_QI_H__

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
    AT_SOCKET_QI_DATA_MODE_BUFFER = 0,            /* 非透传 Buffer 模式 */
    AT_SOCKET_QI_DATA_MODE_PUSH = 1,              /* 非透传 Push 模式 */
    AT_SOCKET_QI_DATA_MODE_TRANSPARENT = 2,       /* 透传模式 */
    AT_SOCKET_QI_DATA_MODE_MORE,
}AT_SocketQiDataMode; /* 数据访问模式 */

typedef enum
{
    AT_SOCKET_QI_CMD_UNKNOWN,              /* 不能识别的命令 */
    AT_SOCKET_QI_CMD_TRANS_PKGSIZE,        /* transpktsize */
    AT_SOCKET_QI_CMD_TRANS_WAITTM,         /* transwaittm */
    AT_SOCKET_QI_CMD_TCP_ACCEPT,           /* tcp/accept */
    AT_SOCKET_QI_CMD_PASSIVE_CLOSED,       /* passiveclosed */
    AT_SOCKET_QI_CMD_DATA_FORMATE,         /* dataformat */
    AT_SOCKET_QI_CMD_VIEW_MODE,            /* viewmode */
    AT_SOCKET_QI_CMD_IISEND_TIMEOUT,       /* iisend/timeout */
    AT_SOCKET_QI_CMD_SEND_AUTO,            /* send/auto */
    AT_SOCKET_QI_CMD_RECVIND,              /* recvind */
    AT_SOCKET_QI_CMD_FORMAT_CFG,           /* formatcfg */
    AT_SOCKET_QI_CMD_CLOSE_MODE,           /* close/mode */
    AT_SOCKET_QI_CMD_UDP_SENDMODE,         /* udp/sendmode */
    AT_SOCKET_QI_CMD_UDP_READMODE,         /* udp/readmode */
    AT_SOCKET_QI_CMD_TCP_KEEPALIVE,        /* tcp/keepalive */
    AT_SOCKET_QI_CMD_WAKEUP_DATA,          /* wakeup/data */
    AT_SOCKET_QI_CMD_RECV_IGNORE,          /* recv/ignore */
    AT_SOCKET_QI_CMD_SEND_BUFFERSIZE,      /* send/buffersize */
    AT_SOCKET_QI_CMD_TCP_SEND_MODE,        /* TCP/SendMode */
    //AT_SOCKET_QI_CMD_DNS_CACHE,            /* dns/cache */
    //AT_SOCKET_QI_CMD_RECV_BUFFERSIZE,      /* recv/buffersize */
    AT_SOCKET_QI_CMD_MAX
}AT_SocketQiCmdType; /* 命令类型 */

typedef enum
{
    AT_SOCKET_QI_NAT_CLOSE           = 0,    /* NAT 功能关闭 */
    AT_SOCKET_QI_NAT_OPEN            = 1,    /* NAT 功能打开 */
}AT_SocketQiNatSwitch;        /* NAT 功能关闭/打开控制 */

typedef enum
{
    AT_SOCKET_QI_CONN_STATE_INITIAL = 0,               /* 初始态(连接未建立) */
    AT_SOCKET_QI_CONN_STATE_CONNECTING = 1,            /* 作为客户端, 正在连接中 */
    AT_SOCKET_QI_CONN_STATE_LISTENING = 2,             /* 作为服务器, 正在监听中 */
    AT_SOCKET_QI_CONN_STATE_LISTENED = 3,              /* 作为服务器, 已监听到客户端发起的连接，服务器正在连接中 */
    AT_SOCKET_QI_CONN_STATE_CONNECTED = 4,             /* 作为客户端, 已连接 */
    AT_SOCKET_QI_CONN_STATE_ACCEPTED = 5,              /* 作为服务器, 已连接 */
    AT_SOCKET_QI_CONN_STATE_ACTIVE_CLOSING = 6,        /* 本端发起的关闭, 关闭中(本端主动关闭) */
    AT_SOCKET_QI_CONN_STATE_PASSIVE_CLOSING = 7,       /* 远端发起的关闭, 关闭中(本端被动关闭) */
    AT_SOCKET_QI_CONN_STATE_ACTIVE_CLOSED = 8,         /* 本端发起的关闭, 已关闭(本端主动关闭) */
    AT_SOCKET_QI_CONN_STATE_PASSIVE_CLOSED = 9,        /* 远端发起的关闭, 已关闭(本端被动关闭) */
}AT_SocketQiConnState; /* TCP/IP 连接状态 */

typedef enum
{
    AT_SOCKET_QI_SOCK_STATE_INITIAL = 0,               /* "Initial"：尚未建立连接 */
    AT_SOCKET_QI_SOCK_STATE_OPENING = 1,               /* "Opening"：客户端正在连接或者服务器正尝试监听 */
    AT_SOCKET_QI_SOCK_STATE_CONNECTED = 2,             /* "Connected"：客户端连接已建立 */
    AT_SOCKET_QI_SOCK_STATE_LISTENING = 3,             /* "Listening"：服务器正在监听 */
    AT_SOCKET_QI_SOCK_STATE_CLOSING = 4,               /* "Closing"：连接断开 */
}AT_SocketQiSockState; /* TCP/IP Socket 服务状态 */

typedef enum
{
    AT_SOCKET_QI_CONNECT_NORMAL = 0,           /* 本端正常连接,  建立连接成功 */
    AT_SOCKET_QI_CONNECT_SERVER_TIMEOUT,       /* 服务器端未响应, 建立连接超时 */
    AT_SOCKET_QI_CONNECT_REMOTE_RESET,         /* 收到远端的 RST,  建立连接失败 */
    AT_SOCKET_QI_CONNECT_TRANS_IMEOUT,         /* 数据传输超时关闭 */
    AT_SOCKET_QI_CONNECT_REMOTE_FIN,           /* 收到远端 FIN,  建立连接失败 */
    AT_SOCKET_QI_CONNECT_FAIL_OTHER,           /* 其它原因导致建立连接失败 */
}AT_SocketQiOpenReason;    /* TCP/IP 建立连接失败的原因 */

typedef enum
{
    AT_SOCKET_QI_CLOSE_NORMAL = 0,          /* 本端正常关闭,  关闭成功 */
    AT_SOCKET_QI_CLOSE_SERVER_TIMEOUT,      /* 服务器端未响应，超时关闭 */
    AT_SOCKET_QI_CLOSE_FAIL_OTHER,          /* 其它原因导致关闭连接失败 */
    AT_SOCKET_QI_CLOSE_REMOTE_RESET,        /* 收到远端的 RST (本端被动关闭) */
    AT_SOCKET_QI_CLOSE_TRANS_IMEOUT,        /* 数据传输超时关闭 */
    AT_SOCKET_QI_CLOSE_REMOTE_FIN,          /* 收到远端的 FIN, 已关闭(本端被动关闭) */
}AT_SocketQiCloseReason;    /* TCP/IP 关闭连接失败的原因 */

typedef enum
{
    AT_SOCKET_QI_DISCONN_REMOTE_CLOSE = 1,          /* 服务器关闭连接 */
    AT_SOCKET_QI_DISCONN_ABNORMAL,                  /* 连接异常 */
    AT_SOCKET_QI_DISCONN_PDP_DEACT                  /* PDP 去激活 */
}AT_SocketQiDisconnReason;    /* 连接异常的原因 */

typedef enum
{
    AT_SOCKET_QI_TYPE_NONE           = 0,
    AT_SOCKET_QI_TYPE_TCP,
    AT_SOCKET_QI_TYPE_UDP,
}AT_SocketQiType;        /* TCP/IP 连接类型 */

typedef enum
{
    AT_SOCKET_QI_SERVER_TYPE_NONE = 0,
    AT_SOCKET_QI_SERVER_TYPE_TCP_LISTENER,
    AT_SOCKET_QI_SERVER_TYPE_TCP_INCOMING  ,
    AT_SOCKET_QI_SERVER_TYPE_TCP_CLIENT,
    AT_SOCKET_QI_SERVER_TYPE_UDP_SERVICE,
    AT_SOCKET_QI_SERVER_TYPE_UDP_CLIENT,
}AT_SocketQiServerType;        /* TCP/IP 服务连接类型 */

typedef enum
{
    AT_SOCKET_QI_TASK_SENDTO_NONE           = 0,
    AT_SOCKET_QI_TASK_SENDTO_AT_DATA,                 /* 通过 recv_from 从 LWIP 收到的数据, 发给 AT 框架 */
    AT_SOCKET_QI_TASK_SENDTO_AT_URC_TRANS, /* 透传模式下, 异步上报 URC 给 AT 框架*/
    AT_SOCKET_QI_TASK_SENDTO_AT_URC_PUSH, /* 直吐模式下, 异步上报 URC 给 AT 框架 */
    AT_SOCKET_QI_TASK_SENDTO_LWIP_WITH_INDICATE,     /* 直吐模式下或缓存模式下，通过回调从 AT 框架收到的数据, 发给 LWIP */
    AT_SOCKET_QI_TASK_SENDTO_LWIP_WITH_TRANSPARENT,  /* 透传模式下，通过回调从 AT 框架收到的数据, 发给 LWIP*/
    AT_SOCKET_QI_TRANSPARENT_QUIT, /* 退出透传模式 */
    AT_SOCKET_QI_SEND_TCP_RST, /* 发送 TCP RST */
    AT_SOCKET_QI_TASK_QUIT, /* 任务退出 */
}AT_SocketQiTaskSendDirection;        /* 任务收到数据时发送方向 */

typedef enum
{
    AT_SOCKET_QI_URC_MODE_CLOSE       = 0,    /* 关闭 */
    AT_SOCKET_QI_URC_MODE_OPEN        = 1,    /* 打开 */
}AT_SocketQiUrcMode; /* 数据主动上报模式 */

typedef enum
{
    AT_SOCKET_QI_SEND_FORMAT_ASCII           = 0,    /* ASCII 字符串(原始数据) */
    AT_SOCKET_QI_SEND_FORMAT_HEX,                    /* HEX 字符串 */
}AT_SocketQiSendFormat;        /* 数据发送格式(仅限非透传模式) */

typedef enum
{
    AT_SOCKET_QI_RECV_FORMAT_ASCII           = 0,    /* ASCII 字符串(原始数据) */
    AT_SOCKET_QI_RECV_FORMAT_HEX,                    /* HEX 字符串 */
}AT_SocketQiRecvFormat;        /* 数据接收格式(仅限非透传模式) */

typedef enum
{
    AT_SOCKET_QI_VIEW_MOED_DATA_NEWLINE           = 0,    /* data header\r\ndata */
    AT_SOCKET_QI_VIEW_MOED_DATA_NO_NEWLINE,               /* data header,data */
}AT_SocketQiViewMode;        /* 接收数据的输出方式(仅限非透传模式) */

typedef enum
{
    AT_SOCKET_QI_DNS_CACHE_DISABLE           = 0,    /* 关闭 DNS 缓存 */
    AT_SOCKET_QI_DNS_CACHE_ENABLE,                   /* 使能 DNS 缓存 */
}AT_SocketQiDnsCache;        /* 使能或者关闭 DNS 缓存 */

typedef enum
{
    AT_SOCKET_QI_AUTO_CLOSED_DISABLE           = 0,   /* 禁止 TCP 连接在服务器关闭后自动断开, 需使用 AT+MIPCLOSE 命令手动释放资源 */
    AT_SOCKET_QI_AUTO_CLOSED_ENABLE,                  /* 使能 TCP 连接在服务器关闭后自动断开, 无需使用 AT+MIPCLOSE 命令 */
}AT_SocketQiAutoClosedEnable;        /* 禁止或使能 TCP 连接在服务器关闭后自动断开 */

typedef enum
{
    AT_SOCKET_QI_UDP_MODE_DISABLE_BLOCK           = 0,    /* 禁止块模式 */
    AT_SOCKET_QI_UDP_MODE_ENABLE_STREAM,                  /* 使能流模式 */
}AT_SocketQiUdpReadSendMode;        /* UDP 读取或发送模式 */

typedef enum
{
    AT_SOCKET_QI_TCP_ACCEPT_AUTO_DISABLE           = 0,    /* 禁止自动接受来自客户端的 TCP 连接 */
    AT_SOCKET_QI_TCP_ACCEPT_AUTO_ENABLE,                   /* 使能自动接受来自客户端的 TCP 连接 */
}AT_SocketQiTcpAcceptAuto;        /* 禁止或使能自动接受来自客户端的 TCP 连接 */

typedef enum
{
    AT_SOCKET_QI_KEEPALIVE_DISABLE           = 0,    /* 关闭心跳 */
    AT_SOCKET_QI_KEEPALIVE_ENABLE,                   /* 打开心跳 */
}AT_SocketQiKeepaliveEnable;        /* TCP 心跳使能开关. 默认值0 */

typedef enum
{
    AT_SOCKET_QI_URC_LENGTH_DISABLE               = 0,    /* 禁止 TCP/IP 非透传 Buffer 模式下 URC */
    AT_SOCKET_QI_URC_LENGTH_ENABLE,                          /* 使能 TCP/IP 非透传 Buffer 模式下 URC */
}AT_SocketQiRecvUrcEnable;        /* 禁止或使能 TCP/IP 非透传 Buffer 模式下 URC */

typedef enum
{
    AT_SOCKET_QI_FORMAT_CFG_NOT_NEWLINE           = 0,   /* 0D0A>, >后没有\r\n */
    AT_SOCKET_QI_FORMAT_CFG_NEWLINE,                     /* 0D0A>0D0A, >后有\r\n */
}AT_SocketQiFormatCfg;        /* 控制>的输出格式 */

typedef enum
{
    AT_SOCKET_QI_TCP_URC_CLOSE_DISABLE           = 0,   /* 禁用异步断开 TCP 连接 */
    AT_SOCKET_QI_TCP_URC_CLOSE_ENABLE,                  /* 启用异步断开 TCP 连接 */
}AT_SocketQiTcpCloseMode;        /* 启用或禁用异步断开 TCP 连接 */

typedef enum
{
    AT_SOCKET_QI_TCP_SENDDATA_RETURN_OK           = 0,   /* 发送数据后立即返回 SEND OK */
    AT_SOCKET_QI_TCP_RECVACK_RETURN_OK,                  /* 收到服务器 ACK 后返回 SEND OK */
    AT_SOCKET_QI_TCP_RECVACK_RETURN_ID_OK,               /* 收到服务器 ACK 后返回 <connectID>,SEND OK */
    AT_SOCKET_QI_TCP_RECVACK_URC_OK,                     /* 收到服务器 ACK 后上报+QIURC: SEND OK */
    AT_SOCKET_QI_TCP_RECVACK_URC_ID_OK,                  /* 收到服务器 ACK 后上报+QIURC: <connectID>,SEND OK */
}AT_SocketQiTcpSendMode;        /* SEND OK 返回模式 */

typedef enum
{
    AT_SOCKET_QI_SEND_ECHO_DISABLE           = 0,    /* 不回显  */
    AT_SOCKET_QI_SEND_ECHO_ENABLE,                   /* 回显  */
}AT_SocketQiSendEcho;        /* 是否回显 AT+QISEND 要发送的数据 */

typedef enum
{
    AT_SOCKET_QI_SSL_DISABLE           = 0,    /* 关闭 SSL 连接 */
    AT_SOCKET_QI_SSL_ENABLE,                   /* 打开 SSL 连接 */
}AT_SocketQiSslEnable;        /* SSL 连接使能开关. 默认值0 */

typedef enum
{
    AT_SOCKET_QI_RAI_NONE = 0,                /* 无信息 */
    AT_SOCKET_QI_RAI_SEND_UL,                 /* 终端发送一个上行包 */
    AT_SOCKET_QI_RAI_SEND_UL_EXPECT_DL,       /* 终端发送一个上行包，并期望接收到一个下行包 */
}AT_SocketQiSendRai;        /* 发送数据时, 列举帮助信息. 默认值0 */

typedef enum
{
    AT_SOCKET_QI_PRI_FLAG_IPTOS_RELIABILE = 0,      /* IPTOS 可靠性 */
    AT_SOCKET_QI_PRI_FLAG_IPTOS_LOW_DELAY,          /* IPTOS 低延时 */
    AT_SOCKET_QI_PRI_FLAG_IPTOS_THROUGHPUT,         /* IPTOS 吞吐量 */
    AT_SOCKET_QI_PRI_FLAG_IPTOS_LOW_POWER,          /* IPTOS 低消耗 */
}AT_SocketQiSendPriFlag;        /* 发送数据时, 优先级标志. 默认值0 */

typedef enum
{
    AT_SOCKET_QI_URC_CONNECT_ABNORMAL           = 0,    /* 连接异常提示 */
    AT_SOCKET_QI_URC_TCP_DATA_RECV,                     /* 接收 TCP 数据提示 */
    AT_SOCKET_QI_URC_UDP_DATA_RECV,                     /* 接收 UDP 数据提示 */
    AT_SOCKET_QI_URC_TCP_ACK_RECV,                      /* TCP 接收 ACK 包提示 */
    AT_SOCKET_QI_URC_UDP_ACK_RECV,                      /* UDP 空口回传序列号提示 */
    AT_SOCKET_QI_URC_OVERFLOW_RECV,                     /* 接收数据溢出提示 */
}AT_SocketQiUrcType;        /* TCP/IP URC上报信息类型 */

typedef enum
{
    AT_SOCKET_QI_SEND_INDICATE_NONE           = 0,
    AT_SOCKET_QI_SEND_INDICATE_WITHOUT_LEN, /* AT 命令中不带长度的参数, 输入 AT 命令后立即发送, 不需要申请 buf 对数据缓存 */
    AT_SOCKET_QI_SEND_INDICATE_WITH_LEN,         /* AT 命令中带长度的参数, 需要申请 buf 对数据先缓存, 发送的条件是发送长度达到 AT 命令指定的长度或者发送超时时间到达 */
}AT_SocketQiSendIndicateMethod;        /* 以">(输入数据)" 形式发送数据的方式 */

typedef enum
{
    AT_SOCKET_QI_SEND_TRANSPARENT_DISABLE           = 0,
    AT_SOCKET_QI_SEND_TRANSPARENT_ENABLE,
}AT_SocketQiSendTransparentFlag;        /* 透传模式下发送数据的标志 */

typedef enum
{
    AT_SOCKET_QI_ERR_OPERATION_SUCCESSFUL          = 0,          /* 操作成功 */
    AT_SOCKET_QI_ERR_TCPIP_UNKNOWN                 = 550,        /* TCP/IP 未知错误 */
    AT_SOCKET_QI_ERR_OPERATION_BLOCKED             = 551,        /* 操作阻塞 */
    AT_SOCKET_QI_ERR_INVALID_PARAMETERS            = 552,        /* 不合法的参数 */
    AT_SOCKET_QI_ERR_MEMORY_NOT_ENOUGH             = 553,        /* 内存不够 */
    AT_SOCKET_QI_ERR_SOCKET_CREATE_FAIL            = 554,        /* SOCKET 创建失败 */
    AT_SOCKET_QI_ERR_OPERATION_NOT_SUPPORT         = 555,        /* 操作不支持 */
    AT_SOCKET_QI_ERR_SOCKET_BIND_FAIL              = 556,        /* SOCKET 绑定失败 */
    AT_SOCKET_QI_ERR_SOCKET_LISTEN_FAIL            = 557,        /* SOCKET 监听失败 */
    AT_SOCKET_QI_ERR_SOCKET_WIRTE_FAILED           = 558,        /* SOCKET 写入失败 */
    AT_SOCKET_QI_ERR_SOCKET_READ_FAILED            = 559,        /* SOCKET 读取失败 */
    AT_SOCKET_QI_ERR_SOCKET_ACCEPT_FAILED          = 560,        /* SOCKET 接受失败 */
    AT_SOCKET_QI_ERR_OPEN_PDP_FAILED               = 561,        /* 打开 PDP 上下文失败 */
    AT_SOCKET_QI_ERR_CLOSE_PDP_FAILED              = 562,        /* 关闭 PDP 上下文失败 */
    AT_SOCKET_QI_ERR_SOCKET_USED                   = 563,        /* TCP/IP 已被使用 */
    AT_SOCKET_QI_ERR_DNS_BUSY                      = 564,        /* DNS 忙碌 */
    AT_SOCKET_QI_ERR_DNS_PARSE_FAIL                = 565,        /* DNS 解析失败或错误的 IP 格式 */
    AT_SOCKET_QI_ERR_SOCKET_CONNECT_FAIL           = 566,        /* SOCKET 连接失败 */
    AT_SOCKET_QI_ERR_SOCKET_CLOSED                 = 567,        /* SOCKET 已关闭 */
    AT_SOCKET_QI_ERR_OPERATION_BUSY                = 568,        /* 操作 busy */
    AT_SOCKET_QI_ERR_OPERATION_TIMEOUT             = 569,        /* 操作超时 */
    AT_SOCKET_QI_ERR_PDP_BROKEN_DOWN               = 570,        /* PDP 上下文中断 */
    AT_SOCKET_QI_ERR_SEND_CANCLE                   = 571,        /* 取消发送 */
    AT_SOCKET_QI_ERR_OPERATION_NOT_ALLOW           = 572,        /* 操作不允许 */
    AT_SOCKET_QI_ERR_APN_NO_CONFIG                 = 573,        /* APN 未配置 */
    AT_SOCKET_QI_ERR_PORT_BUSY                     = 574,        /* 端口忙碌 */
}AT_SocketQiErrCode;    /* <err> 错误码 */

typedef enum
{
    AT_SOCKET_QI_SEND_AUTO_FORMAT_ASCII_HEX        = 0,     /* ASCII 十六进制数据 */
    AT_SOCKET_QI_SEND_AUTO_FORMAT_HEX,                      /* 十六进制数据 */
    AT_SOCKET_QI_SEND_AUTO_FORMAT_STRING,                   /* 字符串数据 */
}AT_SocketQiSendAutoFORMAT;        /* send/auto 发送的数据类型 */

typedef enum
{
    AT_SOCKET_QI_IGNORE_DATA_FORMAT_ASCII_HEX        = 0,     /* ASCII 十六进制数据 */
    AT_SOCKET_QI_IGNORE_DATA_FORMAT_HEX,                      /* 十六进制数据 */
    AT_SOCKET_QI_IGNORE_DATA_FORMAT_STRING,                   /* 字符串数据 */
}AT_SocketQiRecvIgnoreDataFORMAT;        /* wakeup/data 和 recv/ignore 数据类型 */

typedef enum
{
    AT_SOCKET_QI_FUZZY_WAKEUP_CLOSE= 0,         /* 关闭模糊唤醒 */
    AT_SOCKET_QI_FUZZY_WAKEUP_OPEN,             /* 开启模糊唤醒 */
}AT_SocketQiFuzzyWakeup;        /* fuzzy_wakeup 模糊唤醒开关 */

typedef struct
{
    uint8_t   sendFormat;       /* 数据发送格式 */
    uint8_t   recvFormat;       /* 数据接收格式 */
    uint8_t   viewMode;         /* 接收数据显示格式 */
    uint8_t   autoClosed;       /* 禁止或使能 TCP 连接在服务器关闭后自动断开 */
    uint8_t   udpReadMode;      /* UDP 读取模式 */
    uint8_t   udpSendMode;      /* UDP 发送模式 */
    uint8_t   urcLenEnable;     /* 缓存模式下, 接收到数据后上报的 URC 是否显示数据长度 */
    uint8_t   maxBackoffs;      /* TCP 重新发送最大次数 */
    uint8_t   tcpAcceptAutoEn;  /* 禁止或使能自动接受来自客户端的 TCP 连接 */
    uint8_t   formatCfg;        /* 发送AT+QISEND命令返回>的格式 */
    uint8_t   tcpCloseMode;     /* 启用或禁用异步断开 TCP 连接 */
    uint8_t   tcpSendMode;      /* SEND OK 返回模式 */
    uint8_t   sendEcho;         /* 是否回显 AT+QISEND 要发送的数据 */
    uint8_t   dnsCache;         /* 使能或者关闭 DNS 缓存 */

    uint8_t   connectTimeout;   /* TCP/IP 连接超时时间, 单位:s, 默认值60 */
    uint8_t   closeMode;        /* 关闭连接模式, 移远的未用到 */

    uint8_t   transWaittm;      /* 透传模式下, 传入数据未达到<transPktSize>指定长度时, 超过<waittm>时间后直接发送,
                                ** 范围:0~20;单位:100ms. 默认值2 */
    uint16_t  transPktSize;     /* 透传模式下, 待发数据包字节最大长度, 范围:1~1460；单位:字节. 默认值1024, 适用于透传模式下发送缓存的设置 */
    uint16_t  maxRto;           /* TCP 重新发送间隔时间 */
    uint16_t  sendTimeout;      /* 输入数据超时时间, 范围: 0-120. 默认值: 30. 单位:秒. AT+QISEND 的超时周期. 响应>后, 若在周期内无数据输入, AT+QISEND 命令将退出. */
    uint16_t  sendBufSize;      /* 单次发送的最大数据长度, 范围：1460~10240; 单位:字节. 默认值1460. */
    //uint32_t  recvBufSize;      /* 接收缓存大小, 范围: 10240-524288. 默认值: 10240.单位: 字节, 使用缓存模式时有效 */
    int       keepaliveEnable;  /* 心跳使能开关. 默认值0 */
    int       keepidle;         /* 心跳包循环间隔, 单位: 秒. 范围:1-1800 */
    int       keepinterval;     /* 心跳包发送间隔, 单位: 秒. 范围: 25-100 */
    int       keepcount;        /* 心跳包重传次数, 范围: 3-10. */
}AT_SocketQiParamCfg;     /* 本地通用配置参数(配置一次后永久生效) */

typedef struct
{
    uint32_t  period;           /* 配置指定通道定时发送指定数据的周期, 范围: 20~86400, 初始值为 0, 单位: 秒 */
    char      *msgAuto;         /* 字符串类型, 十六进制的发送数据类型, 最大长度: 400 字节. 设置时申请内存, 内存一直不释放,当设置不带 <msg_auto> 参数时(即参数为 NULL ),要把内存释放 */
    uint8_t   sendAutoFormat;   /* 整型, 设置输入的 <msg_auto> 数据类型, 范围: 0~2, 初始值为 0 */
    uint8_t   tryTimes;         /* 整型,设置重传次数, 单位:次, 范围: 0~10, 默认值为 0*/
}AT_SocketQiSendAutoCfg;     /* 本地 send/auto 配置参数(配置一次后永久生效) */

typedef struct
{
    char      *ignoreData;      /* 字符串类型,  最大长度: 256 字节 */
    uint32_t  ignoreDataLen;    /* 整型, 报文数据长度 */
    uint8_t   ignoreDataFormat; /* 整型, 输入的报文数据类型, 范围: 0~2, 默认值为 0 */
    uint8_t   fuzzyWakeup;      /* 整型, 模糊唤醒, 默认值为 0(关闭)*/
}AT_SocketQiIgnoreDataCfg;     /* wakeup/data 和 recv/ignore 配置参数(配置一次后永久生效) */

typedef struct
{
    uint8_t   cid;                  /* PDP 上下文 ID, 指定当前实例使用的 PDP 上下文 */
    uint8_t   connectId;
    uint8_t   channelId;            /* AT 命令通道号 */
    uint8_t   state;                /* TCP/IP 连接状态 */
    uint8_t   accessMode;           /* 数据访问模式 */
    uint8_t   tcpClientMaxNum;      /* TCP client 连接的最大数量 */
    uint8_t   protoType;            /* 连接类型(TCP 还是 UDP) */
    uint8_t   serverType;           /* 连接的服务类型(作为 server 还是作为 client) */
    uint8_t   sendFlag;             /* 数据发送标志 */
    uint8_t   sendIndicate;         /* 以">(输入数据)" 形式发送数据的标志 */
    uint8_t   waitTimeoutFlag;      /* 透传模式下, 输入数据超时时间到达的标志. 数值1表示进入定时器开始计时, 超时时间到达直接发送数据 */
    uint8_t   waitIndicate;         /* 透传模式下, 发送数据的标志 */
    uint32_t  sendLen;              /* AT 命令输入的需要发送(send)的数据长度, 单位:字节 */
    uint32_t  transPresendLen;      /* 透传模式下, 从 AT 框架接收的待发送的数据长度, 单位:字节 */
    fd_set    readfds;              /* TCP 监听是否有数据来到的读文件描述符集合, 用于 select */
    struct osTimer *connTimer;      /* TCP/IP 连接超时定时器指针 */
    struct osTimer *closeTimer;     /* TCP/IP 断开连接超时定时器指针 */
    struct osTimer *closeRspOKTimer;  /* TCP/IP 断开连接时响应 OK 定时器指针 */
    struct osTimer *indicateTimer;  /* TCP/IP >模式超时定时器指针 */
    struct osTimer *transTimer;     /* TCP/IP 透传超时定时器指针 */
    struct osTimer *sendautoTimer;      /* TCP/IP send/auto 超时定时器指针 */
    struct sockaddr_in localAddr;
    struct sockaddr_in6 localAddr6;
    osTick_t clientCreateTick;      /* 客户端创建时间节点 */
}AT_SocketQiParamData;     /* TCP/IP 发送接收参数(仅对本次连接有效, 关闭连接成功时置初始值) */

typedef struct
{
    uint32_t  sentByte;         /* 已发送的数据量, 单位:字节 */
    uint32_t  ackedByte;        /* 对端已确认接收到的数据量, 单位:字节 */
    uint32_t  nackByte;         /* 对端还未确认接收的数据量, 单位:字节 */
    uint32_t  totalRecvLen;     /* 缓存模式下: 已接收到的总数据量, 单位:字节 */
    uint32_t  haveReadLen;      /* 缓存模式下: 已读取的数据量, 单位:字节 */
    uint32_t  unreadLen;        /* 缓存模式下: 未读取的数据量, 单位:字节 */
}AT_SocketQiParamStatistics;     /* TCP/IP 发送接收数据信息统计(仅对本次连接有效, 关闭连接成功时置初始值0) */

typedef struct
{
    AT_SocketQiParamData    data;
    AT_SocketQiParamStatistics statistics;
}AT_SocketQiParamLocal;     /* 本地参数信息 */

typedef struct
{
    struct sockaddr_in clientAddr;
    struct sockaddr_in6 clientAddr6;
    struct sockaddr_in udpsendAddr;
    struct sockaddr_in6 udpsendAddr6;
}AT_SocketQiParamRemote;     /* 远端参数信息 */

typedef struct
{
    AT_SocketQiParamLocal     local;
    AT_SocketQiParamRemote    remote;
}AT_SocketQiParam;     /* 参数管理 */

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void AT_SocketQiCfgSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiCfgTest(uint8_t channelId);
void AT_SocketQiOpenSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiOpenTest(uint8_t channelId);
void AT_SocketQiCloseSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiCloseTest(uint8_t channelId);
void AT_SocketQiStateSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiStateRead(uint8_t channelId);
void AT_SocketQiStateExec(uint8_t channelId);
void AT_SocketQiStateTest(uint8_t channelId);
void AT_SocketQiSendTransparentSet(uint8_t channelId, char *paramIn, uint16_t paramLen);
void AT_SocketQiSendSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiSendTest(uint8_t channelId);
void AT_SocketQiReadSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiReadTest(uint8_t channelId);
void AT_SocketQiSendExSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiSendExTest(uint8_t channelId);
void AT_SocketQiModeSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiModeTest(uint8_t channelId);
void AT_SocketQiSendEchoSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiSendEchoRead(uint8_t channelId);
void AT_SocketQiSendEchoTest(uint8_t channelId);
void AT_SocketQiGetErrorExec(uint8_t channelId);
void AT_SocketQiGetErrorTest(uint8_t channelId);
void AT_SocketQiSackSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SocketQiSackTest(uint8_t channelId);
int8_t AT_SocketQiInit();
void AT_SocketQiDeinit();

#ifdef __cplusplus
}
#endif
#endif

