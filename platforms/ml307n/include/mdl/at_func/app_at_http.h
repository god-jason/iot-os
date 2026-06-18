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

#ifndef __APP_AT_HTTP_H__
#define __APP_AT_HTTP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "http_application_api.h"

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
/* 文件上传序列, 一共可上传 12 路。范围: 0-11. 若<index>已配置，再次使用该命令进行配置相同<index>的时候, 则覆盖之前的配置信息 */
#define AT_HTTP_UL_FILE_INDEX_MIN                 (0)
#define AT_HTTP_UL_FILE_INDEX_MAX                 (11)
#define AT_HTTP_UL_FILE_CNT                       (AT_HTTP_UL_FILE_INDEX_MAX - AT_HTTP_UL_FILE_INDEX_MIN + 1)

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum
{
    AT_HTTP_CFG_UNKNOWN = 0,                   /* 不能识别的命令 */
    AT_HTTP_CFG_CID,                       /* CID */
    AT_HTTP_CFG_REQUEST_HEADER,            /* requestheader */
    AT_HTTP_CFG_RESPONSE_HEADER,           /* responseheader */
    AT_HTTP_CFG_SSL_ID,                    /* SSL ID */
    AT_HTTP_CFG_CONTENT_TYPE,              /* contenttype */
    AT_HTTP_CFG_RSPOUT_AUTO,               /* rspout/auto */
    AT_HTTP_CFG_CLOSED_IND,                /* closed/ind */
    //AT_HTTP_CFG_WINDOW_SIZE,               /* windowsize */
    //AT_HTTP_CFG_CLOSE_WAIT_TIME,           /* closewaittime */
    AT_HTTP_CFG_AUTH,                      /* auth */
    AT_HTTP_CFG_CUSTOM_HEADER,             /* custom_header */
    AT_HTTP_CFG_MAX,
}AT_HttpCfgType; /* 参数配置类型 */

typedef enum
{
    AT_HTTP_CFG_EX_UNKNOWN = 0,                   /* 不能识别的命令 */
    AT_HTTP_CFG_EX_SEND_ADD,
    AT_HTTP_CFG_EX_SEND_DEL,
    AT_HTTP_CFG_EX_MAX,
}AT_HttpCfgExType; /* 文件参数配置类型 */

typedef enum
{
    AT_HTTP_TASK_SENDTO_NONE           = 0,
    AT_HTTP_TASK_SENDTO_AT,       /* 通过 recv_from 从 LWIP 收到的数据, 发给 AT 框架 */
    AT_HTTP_TASK_SENDTO_HTTP,     /* 通过回调从 AT 框架收到的数据, 发给 HTTP */
}AT_HttpTaskSendDirection;        /* 任务收到数据时发送方向 */

typedef enum
{
    AT_HTTP_REQUEST_HEADER_DISABLE             = 0,    /* 禁用 */
    AT_HTTP_REQUEST_HEADER_ENABLE              = 1,    /* 启用 */
}AT_HttpRequestHeaderEnabled;        /* 禁用或启用自定义 HTTP(S) 请求头信息, 默认值0 */

typedef enum
{
    AT_HTTP_RESPOND_HEADER_DISABLE             = 0,    /* 禁用 */
    AT_HTTP_RESPOND_HEADER_ENABLE              = 1,    /* 启用 */
}AT_HttpRespondHeaderEnabled;        /* 禁用或启用自定义 HTTP(S) 响应头信息, 默认值0 */

typedef enum
{
    AT_HTTP_CONTENT_TYPE_APP_WWW               = 0,    /* application/x-www-form-urlencoded */
    AT_HTTP_CONTENT_TYPE_TEXT_PLAIN            = 1,    /* text/plain */
    AT_HTTP_CONTENT_TYPE_APP_OCT               = 2,    /* application/octet-stream */
    AT_HTTP_CONTENT_TYPE_MULTI_DATA            = 3,    /* multipart/form-data */
    AT_HTTP_CONTENT_TYPE_APP_JSON              = 4,    /* application/json */
    AT_HTTP_CONTENT_TYPE_IMAGE_JPEG            = 5,    /* image/jpeg */
}AT_HttpContentType;        /* HTTP(S) 体的数据类型, 默认值0 */

typedef enum
{
    AT_HTTP_AUTO_OUTRSP_DISABLE                = 0,    /* 禁用 */
    AT_HTTP_AUTO_OUTRSP_ENABLE                 = 1,    /* 启用 */
}AT_HttpAutoOutrspEnabled;        /* 禁用或启用自动输出 HTTP(S) 响应头信息, 默认值0. 若启用自动输出 HTTP(S)
                                    响应头信息, AT+QHTTPREAD 和 AT+QHTTPREADFILE 会执行失败 */

typedef enum
{
    AT_HTTP_CLOSED_IND_DISABLE                 = 0,    /* 禁用 */
    AT_HTTP_CLOSED_IND_ENABLE                  = 1,    /* 启用 */
}AT_HttpClosedindEnabled;        /* 禁用或启用上报 HTTP(S) 会话关闭 URC +QHTTPURC: "closed", 默认值0.  */

typedef enum
{
    AT_HTTP_POST_FILE_TYPE_SEND_DIRECT         = 0,    /* 直接发送当前文件 */
    AT_HTTP_POST_FILE_TYPE_RECORD_NAME         = 1,    /* 记录当前要发送的文件名 */
    AT_HTTP_POST_FILE_TYPE_SEND_ORDER          = 2,    /* 将文件 1 和文件 2 按顺序发出 */
}AT_HttpPostFileType;        /* HTTP(S) POST 分段发送文件的方式.  */

typedef enum
{
    AT_HTTP_PUT_FILE_TYPE_REQ_SELECT         = 0,  /* 若<request_header>设置为 0, 表示请求体文件; 若<request_header>设置为 1,表示请求头和请求体文件 */
    AT_HTTP_PUT_FILE_TYPE_REQ_HEADER         = 1,  /* 表示请求头文件, 此时<request_header>取值须为 1 */
    AT_HTTP_PUT_FILE_TYPE_REQ_BODY           = 2,  /* 表示请求体文件, 此时<request_header>取值须为 1 */
}AT_HttpPutFileType;        /* HTTP(S) PUT 发送的文件信息. 仅在<request_header>=0 时该参数才可省略  */

typedef enum
{
    AT_HTTP_PUT_FILE_INDEX_ADDED               = 0,   /* 未添加配置参数 */
    AT_HTTP_PUT_FILE_INDEX_NO_ADD              = 1,   /* 已添加配置参数 */
}AT_HttpPutFileIndexStatus;        /* 当前<index>是否被配置*/

typedef enum
{
    AT_HTTP_CONN_STATE_INITIAL                 = 0,   /* 初始态(连接未建立) */
    AT_HTTP_CONN_STATE_CONNECTING              = 1,   /* 作为客户端, 正在连接中 */
    AT_HTTP_CONN_STATE_LISTENING               = 2,   /* 作为服务器, 正在监听中 */
    AT_HTTP_CONN_STATE_LISTENED                = 3,   /* 作为服务器, 已监听到客户端发起的连接，服务器正在连接中 */
    AT_HTTP_CONN_STATE_CONNECTED               = 4,   /* 作为客户端, 已连接 */
    AT_HTTP_CONN_STATE_ACCEPTED                = 5,   /* 作为服务器, 已连接 */
    AT_HTTP_CONN_STATE_ACTIVE_CLOSING          = 6,   /* 本端发起的关闭, 关闭中(本端主动关闭) */
    AT_HTTP_CONN_STATE_PASSIVE_CLOSING         = 7,   /* 远端发起的关闭, 关闭中(本端被动关闭) */
    AT_HTTP_CONN_STATE_ACTIVE_CLOSED           = 8,   /* 本端发起的关闭, 已关闭(本端主动关闭) */
    AT_HTTP_CONN_STATE_PASSIVE_CLOSED          = 9,   /* 远端发起的关闭, 已关闭(本端被动关闭) */
}AT_HTTPConnState; /* TCP/IP 连接状态 */

typedef enum
{
    AT_HTTP_SEND_TRANSPARENT_DISABLE           = 0,
    AT_HTTP_SEND_TRANSPARENT_ENABLE,
}AT_HTTPSendTransparentFlag;        /* 透传模式下发送数据的标志 */

typedef enum
{
    AT_HTTP_SEND_TIMEOUT_STOP                  = 0,   /* 数据发送的定时器计时结束 */
    AT_HTTP_SEND_TIMEOUT_START                 = 1,   /* 数据发送的定时器计时开始 */
}AT_HTTPSendTimeoutFlag;        /* 透传模式下发送数据计时开始和结束 */

typedef enum
{
    AT_HTTP_TYPE_HTTPS                         = 0,   /* HTTP */
    AT_HTTP_TYPE_HTTP                          = 1,   /* HTTPS */
}AT_HTTPType;        /* HTTP 还是 HTTPS */

typedef enum
{
    AT_HTTP_CFG_EX_NO_ADDED                    = 0,   /* 未添加配置参数 */
    AT_HTTP_CFG_EX_ADDED                       = 1,   /* 已添加配置参数 */
}AT_HTTPTCfgExFlag;        /* 当前<index>是否被配置 */

typedef enum
{
    AT_HTTP_CONN_CREATE_FALSE                      = 0,   /* 未创建连接 */
    AT_HTTP_CONN_CREATE_TRUE                       = 1,   /* 已创建连接 */
}AT_HTTPTCreateFlag;        /* HTTP 是否创建连接 */

typedef enum
{
    AT_HTTP_DATA_REQ_TYPE_URL = 0,        /* 设置服务器 URL */
    AT_HTTP_DATA_REQ_TYPE_CREATE,         /* 创建 HTTP 连接  */
    AT_HTTP_DATA_REQ_TYPE_GET,            /* 发送 GET 请求到 HTTP(S) 服务器 */
    AT_HTTP_DATA_REQ_TYPE_POST,           /* 通过 UART/USB 发送 POST 请求到 HTTP(S) 服务器 */
    AT_HTTP_DATA_REQ_TYPE_POST_FILE,      /* 通过文件发送 POST 请求到 HTTP(S) 服务器 */
    AT_HTTP_DATA_REQ_TYPE_PUT,            /* 通过 UART/USB 发送 PUT 请求到 HTTP(S)服务器 */
    AT_HTTP_DATA_REQ_TYPE_PUT_FILE,       /* 通过文件发送 PUT 请求到 HTTP(S)服务器 */
    AT_HTTP_DATA_REQ_TYPE_READ,           /* 通过 UART/USB 读取 HTTP(S)服务器响应信息 */
    AT_HTTP_DATA_REQ_TYPE_READ_FILE,      /* 通过文件读取 HTTP(S)服务器响应信息 */
    AT_HTTP_DATA_REQ_TYPE_SEND,           /* 通过文件向 HTTP(S)发送 POST 请求 */
    AT_HTTP_DATA_REQ_TYPE_NONE,
    AT_HTTP_DATA_REQ_TYPE_MORE,
}AT_HTTPDataSendTransparentType;        /* 透传模式下数据请求类型 */

typedef enum
{
    AT_HTTP_CB_EVENT_ACCEPT = 0,
    AT_HTTP_CB_EVENT_SENT = 1,
    AT_HTTP_CB_EVENT_RECVD = 2,
    AT_HTTP_CB_EVENT_CONNECTED = 3,
    AT_HTTP_CB_EVENT_POLL = 4,
    AT_HTTP_CB_EVENT_CLOSE = 5,
    AT_HTTP_CB_EVENT_ERR = 6,
}AT_HttpCallbackEvent; /* 从 LWIP 回调的事件类型, 与 tcp.h 文件中 lwip_event 的枚举值对应 */

typedef enum
{
    AT_HTTP_ERR_OPT_OK                         = 0,    /* Operation successful */
    AT_HTTP_ERR_UNKNOWN_ERROR                  = 701,  /* HTTP(S) unknown error */
    AT_HTTP_ERR_TIMEOUT                        = 702,  /* HTTP(S) timeout */
    AT_HTTP_ERR_BUSY                           = 703,  /* HTTP(S) busy */
    AT_HTTP_ERR_UART_BUSY                      = 704,  /* HTTP(S) UART busy */
    AT_HTTP_ERR_NO_GET_POST_REQUEST            = 705,  /* HTTP(S) no GET/POST requests */
    AT_HTTP_ERR_NETWORK_BUSY                   = 706,  /* HTTP(S) network busy */
    AT_HTTP_ERR_NETWORK_OPEN_FAILED            = 707,  /* HTTP(S) network open failed */
    AT_HTTP_ERR_NETWORK_NO_CFG                 = 708,  /* HTTP(S) network no configuration */
    AT_HTTP_ERR_NETWORK_DEACT                  = 709,  /* HTTP(S) network deactivated */
    AT_HTTP_ERR_NETWORK_ERROR                  = 710,  /* HTTP(S) network error */
    AT_HTTP_ERR_URL_ERROR                      = 711,  /* HTTP(S) URL error */
    AT_HTTP_ERR_URL_EMPTY                      = 712,  /* HTTP(S) empty URL */
    AT_HTTP_ERR_IP_ERROR                       = 713,  /* HTTP(S) IP address error */
    AT_HTTP_ERR_DNS_ERROR                      = 714,  /* HTTP(S) DNS error */
    AT_HTTP_ERR_SOCK_CREAT_ERROR               = 715,  /* HTTP(S) socket create error */
    AT_HTTP_ERR_SOCK_CONNECT_ERROR             = 716,  /* HTTP(S) socket connect error */
    AT_HTTP_ERR_SOCK_READ_ERROR                = 717,  /* HTTP(S) socket read error */
    AT_HTTP_ERR_SOCK_WRITE_ERROR               = 718,  /* HTTP(S) socket write error */
    AT_HTTP_ERR_SOCK_CLOSED                    = 719,  /* HTTP(S) socket closed */
    AT_HTTP_ERR_DATA_ENCODE_ERROR              = 720,  /* HTTP(S) data encode error */
    AT_HTTP_ERR_DATA_DECODE_ERROR              = 721,  /* HTTP(S) data decode error */
    AT_HTTP_ERR_READ_TIMEOUT                   = 722,  /* HTTP(S) read timeout */
    AT_HTTP_ERR_RESPOND_FAILED                 = 723,  /* HTTP(S) response failed */
    AT_HTTP_ERR_INCOMINT_CALL_BUSY             = 724,  /* Incoming call busy */
    AT_HTTP_ERR_VOICE_CALL_BUSY                = 725,  /* Voice call busy */
    AT_HTTP_ERR_INPUT_TIMEOUT                  = 726,  /* Input timeout */
    AT_HTTP_ERR_WAIT_DATA_TIMEOUT              = 727,  /* Wait data timeout */
    AT_HTTP_ERR_WAIT_RESPONSE_TIMEOUT          = 728,  /* Wait HTTP(S) response timeout */
    AT_HTTP_ERR_MEMORY_ALLOC_FAILED            = 729,  /* Memory allocation failed */
    AT_HTTP_ERR_INVALID_PARAMETER              = 730,  /* Invalid parameter */
    AT_HTTP_ERR_RESPONSE_BUF_NO_ENOUGH         = 731,  /* response buf not enough */
}AT_HttpErrCode;    /* <err> 错误码 */

typedef enum
{
    AT_HTTP_RSP_CODE_OK                             = 200,  /* 正常响应OK */
    AT_HTTP_RSP_CODE_CREATED                        = 201,  /* Created */
    AT_HTTP_RSP_CODE_ACCEPTED                       = 202,  /* Accepted */
    AT_HTTP_RSP_CODE_PARTIAL_CONTENT                = 206,  /* 断点续传成功 */
    AT_HTTP_RSP_CODE_BAD_REQUEST                    = 400,  /* Bad Request */
    AT_HTTP_RSP_CODE_UNAUTHORIZED                   = 401,  /* Unauthorized */
    AT_HTTP_RSP_CODE_PAYMENT_REQUIRED               = 402,  /* Payment Required */
    AT_HTTP_RSP_CODE_FORBIDDEN                      = 403,  /* Forbidden */
    AT_HTTP_RSP_CODE_NOT_FOUND                      = 404,  /* Not found */
    AT_HTTP_RSP_CODE_METHOD_NOT_ALLOWED             = 405,  /* Method Not Allowed */
    AT_HTTP_RSP_CODE_NOT_ACCEPTABLE                 = 406,  /* Not Acceptable */
    AT_HTTP_RSP_CODE_PROXY_AUTHENTICATION           = 407,  /* Proxy Autuentication */
    AT_HTTP_RSP_CODE_REQUEST_TIMEOUT                = 408,  /* Request Timeout */
    AT_HTTP_RSP_CODE_CONFILCT                       = 409,  /* Conflict */
    AT_HTTP_RSP_CODE_GONE                           = 410,  /* Gone */
    AT_HTTP_RSP_CODE_LEN_REQUIRED                   = 411,  /* Length required */
    AT_HTTP_RSP_CODE_PRECONDITION_FAILED            = 412,  /* Precondition Failed */
    AT_HTTP_RSP_CODE_PAYLOAD_TOO_LARGE              = 413,  /* Payload Too Large */
    AT_HTTP_RSP_CODE_URL_TOO_LONG                   = 414,  /* URI Too Long */
    AT_HTTP_RSP_CODE_UNSUPPORTED_MEDIA_TYPE         = 415,  /* Unsupported Media Type */
    AT_HTTP_RSP_CODE_RANGE_NOT_SATISFIABLE          = 416,  /* Range Not Satisfiable */
    AT_HTTP_RSP_CODE_EXPECTATION_FAILED             = 417,  /* Expectation Failed */
    AT_HTTP_RSP_CODE_INTERNAL_SERVER_ERR            = 500,  /* Internal server error */
    AT_HTTP_RSP_CODE_NOT_IMPLEMENTED                = 501,  /* Not Implemented */
}AT_HttpRspCode;    /* <httprspcode>响应代码 */

typedef struct
{
    uint8_t   rspTimeoutFlag;     /* HTTP(S) GET/POST/PUT 响应超时时间计时标志. 1: 开始计时, 0:停止计时 */
    uint32_t  reqInputLen;        /* HTTP(S) GET/POST/PUT 请求信息的长度.包括 HTTP(S) GET/POST/PUT 请求头和请求体信息.范围: 1-2048;单位: 字节 */
    uint16_t  rspTimeout;         /* HTTP(S) GET/POST/PUT 响应超时时间. 范围: 1-65535;默认值: 60; 单位: 秒 */
    uint16_t  reqInputTimeout;    /* HTTP(S) GET/POST/PUT 请求输入超时时间. 范围: 1-65535;默认值: 60; 单位: 秒 */
}AT_HttpDataTime; /* 超时时间相关参数 */

typedef struct
{
    uint8_t          cid;              /* PDP 上下文 id, 指定当前实例使用的 PDP 上下文. 默认值1 */
    uint8_t          connectId;
    uint8_t          channelId;        /* AT 命令通道号 */
    uint8_t          isHttp;           /* 1: HTTP; 0: HTTPS */
    uint8_t          reqHeadEn;        /* 禁用或启用自定义 HTTP(S) 请求头信息. 默认值0 */
    uint8_t          rspHeadEn;        /* 禁用或启用输出 HTTP(S)响应头信息. 默认值0 */
    uint8_t          sslId;            /* HTTP(S) 的 SSL 上下文 ID. 范围:0-5, 默认值1 */
    uint8_t          contentType;      /* HTTP(S)体的数据类型. 默认值0 */
    uint8_t          autoOutrsp;       /* 禁用或启用自动输出 HTTP(S)响应头信息. 默认值0 */
    uint8_t          closedInd;        /* 禁用或启用上报 HTTP(S)会话关闭 URC +QHTTPURC: "closed". 默认值0 */
    uint32_t         windowSize;       /* HTTP(S) socket 滑动窗口大小.范围: 1-4294967295;默认值:16384(16 KB); 单位: 字节 */
    uint32_t         closeWaitTime;    /* HTTP(S) socket 关闭等待时间. 范围:0-4294967295; 默认值: 60000; 单位: 毫秒 */
    char             *userName;        /* HTTP(S) 登录的用户名 */
    char             *password;        /* HTTP(S) 登录的密码 */
    char             *customValue;     /* 用户自定义 HTTP 头 */
#ifdef CONFIG_HTTP_SECURE
    AT_SslContextEx  *sslCtxEx;        /* SSL 上下文参数 */
#endif
}AT_HttpParamCfg;     /* 本地通用HTTP(S) 服务器配置参数(配置一次后永久生效) */

typedef struct
{
    uint8_t   fileNameIndex;      /* 文件名设置的序号 */
    uint8_t   contexTypeIndex;    /* 发送内容的数据类型的序号 */
}AT_HttpParamCfgExIndex;     /* 记录参数被配置的序号 */

typedef struct
{
    uint8_t   flag;            /* 当前<index>是否被配置. 0: 未配置; 1: 已配置 */
    char      *name;           /* 需要配置的参数名 */
    char      *fileName;       /* 需要发送的文件 */
    char      *contentType;    /* 需要发送内容的数据类型 */
    char      *value;          /* 需要发送到服务器的数据 */
}AT_HttpParamCfgEx;     /* 用于配置 AT+QHTTPSEND 要发送的文件和参数 */

typedef struct
{
    uint8_t   state;              /* TCP/IP 连接状态 */
    uint8_t   reqMethod;          /* 数据请求方法. URL/GET/POST/PUT */
    uint8_t   sendIndicate;       /* 数据透传发送指示标志 */
    uint8_t   waitTimeoutFlag;    /* 透传模式下, 输入数据超时时间到达的标志. 数值1表示进入定时器开始计时, 超时时间到达直接发送数据 */
    uint8_t   sendFlag;           /* 数据发送标志 */
    uint8_t   isCreate;           /* HTTP 是否创建连接. 1: 已创建; 0: 未创建 */
    uint32_t  rangeStart;         /* HTTP(S) GET 请求头信息中 (Range) 的取值范围的开始位置 */
    uint32_t  rangeLen;           /* HTTP(S) GET 请求头信息中 (Range) 的取值范围的开始位置 */
    uint32_t  responseCode;       /* HTTP(S) 响应码 */
    uint32_t  responseBodyLen;    /* HTTP(S) 响应内容体信息长度 */
    char      *filePath;          /* 文件存放路径 */
    char      *urlString;         /* URL 字符串, GET/POST/PUT 时使用, 必须以 http:// 或 https:// 开头. 仅本次连接有效, GET/POST/PUT 完后需要释放内存 */
    char      *requestHeader;     /* HTTP(S) GET 请求头信息, 内容由用户传递 */
    char      *responseHeader;    /* HTTP(S) 响应头信息, 内容由服务器应答时填充, 内存需由用户申请 */
    char      *responseBody;      /* HTTP(S) 响应内容体信息, 内容由服务器应答时填充, 内存需由用户申请 */
    char      *postBody;          /* HTTP(S) POST/PUT 请求体, 内容由用户传递 */
    http_client_t *client;
    AT_HttpDataTime time[AT_HTTP_DATA_REQ_TYPE_MORE];
}AT_HttpParamData;

typedef struct
{
    AT_HttpParamCfg     cfg;
    AT_HttpParamData    data;
    AT_HttpParamCfgEx   cfgEx[AT_HTTP_UL_FILE_CNT];
}AT_HttpParamLocal;     /* 本地参数信息 */

typedef struct
{
    AT_HttpParamCfg         cfg;
    AT_HttpParamData        data;
    AT_HttpParamCfgExIndex  cfgExIndex;
    AT_HttpParamCfgEx       cfgEx[AT_HTTP_UL_FILE_CNT];
}AT_HttpParam;     /* 参数管理 */

typedef struct
{
    uint16_t        reqInputTimeout; /* HTTP(S) URL/GET/POST/PUT 请求信息的最大输入时间 */
    uint16_t        rspTimeout;     /* HTTP(S) URL/GET/POST/PUT 响应最大超时时间 */
    struct osTimer  *reqInputTimer;
    struct osTimer  *rspTimer;
    struct osTimer  *connectTimer; /* HTTP(S) 保持 TCP 连接时长的定时器指针 */
}AT_HttpTimeout;     /* 定时器超时时间 */


/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void AT_MhttpCfgSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpCfgRead(uint8_t channelId);
void AT_MhttpCfgTest(uint8_t channelId);
void AT_MhttpUrlSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpUrlRead(uint8_t channelId);
void AT_MhttpUrlTest(uint8_t channelId);
void AT_MhttpGetSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpGetTest(uint8_t channelId);
void AT_MhttpGetExec(uint8_t channelId);
void AT_MhttpGetExSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpGetExTest(uint8_t channelId);
void AT_MhttpPostSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpPostTest(uint8_t channelId);
void AT_MhttpPostFileSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpPostFileTest(uint8_t channelId);
void AT_MhttpPutSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpPutTest(uint8_t channelId);
void AT_MhttpPutFileSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpPutFileTest(uint8_t channelId);
void AT_MhttpReadSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpReadExec(uint8_t channelId);
void AT_MhttpReadTest(uint8_t channelId);
void AT_MhttpReadFileSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpReadFileTest(uint8_t channelId);
void AT_MhttpCfgExSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpCfgExTest(uint8_t channelId);
void AT_MhttpSendSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MhttpSendExec(uint8_t channelId);
void AT_MhttpSendTest(uint8_t channelId);
void AT_MhttpStopTest(uint8_t channelId);
void AT_MhttpStopExec(uint8_t channelId);
int8_t AT_HttpInit();

#ifdef __cplusplus
}
#endif
#endif

