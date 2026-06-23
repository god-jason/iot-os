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

#ifndef __APP_AT_SSL_H__
#define __APP_AT_SSL_H__

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************************
 *                                 平台相关适配
 ************************************************************************************/
#include "os_def.h"
//#include "app_at_pub.h"

#define AT_SSL_DEBUG_PRINTF(format, ...)            APP_AT_PRINT_DEBUG(format, ##__VA_ARGS__)
#define AT_SSL_INFO_PRINTF(format, ...)             APP_AT_PRINT_INFO(format, ##__VA_ARGS__)
#define AT_SSL_WARN_PRINTF(format, ...)             APP_AT_PRINT_WARN(format, ##__VA_ARGS__)
#define AT_SSL_ERROR_PRINTF(format, ...)            APP_AT_PRINT_ERROR(format, ##__VA_ARGS__)

#define AT_SSL_ASSERT(condition)                    OS_ASSERT(condition)
#define AT_SSL_SNPRINTF                             osSnprintf
#define AT_SSL_MALLOC                               osMalloc
#define AT_SSL_FREE                                 osFree
#define AT_SSL_MEMSET                               osMemset
#define AT_SSL_SPRINTF                              osSprintf
#define AT_SSL_STRNCPY                              osStrncpy
#define AT_SSL_STRLEN                               osStrlen

#define AT_SSL_SLIST_OBJECT_INIT                    OS_SLIST_OBJECT_INIT
#define AT_SSL_SLIST_FIRST                          osSlistFirst
#define AT_SSL_SLIST_NEXT                           osSlistNext
#define AT_SSL_SLIST_ENTRY                          osListEntry
#define AT_SSL_SLIST_APPEND                         osSlistAppend
#define AT_SSL_SLIST_REMOVE                         osSlistRemove

#define AT_SSL_PARSER_PARSE_CMD                     at_parser_parse_cmd

#define AT_SSL_LWIP_SOCKET_REGISTER_CALLBACK        lwip_socket_register_callback
#define AT_SSL_LWIP_SOCKET_UNREGISTER_CALLBACK      lwip_socket_unregister_callback

typedef struct osMutex                              AT_SslMutex;
typedef struct osSlistNode                          AT_SslSlistNode;
typedef struct
{
    struct osTimer *timeOut;
	uint32_t        ms;
} AT_SslTimer;
typedef void (*AT_SslTimerCallbackFunc)             (void *argument);


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define AT_SSL_OK                                   1
#define AT_SSL_ERROR                                0
#define AT_SSL_FALSE                                0
#define AT_SSL_TRUE                                 1
#define AT_SSL_MAX_CMD_NAME                         30
#define AT_SSL_MAX_SSL_CTX_NUM                      6
#define AT_SSL_MIN_PDP_CTX_ID                       APP_AT_PDP_CONTEXT_MIN
#define AT_SSL_MAX_PDP_CTX_ID                       APP_AT_PDP_CONTEXT_MAX
#define AT_SSL_MAX_CLIENT_NUM                       12
#define AT_SSL_MAX_PATH_LEN                         100
#define AT_SSL_MAX_RSP_LEN                          100
#define AT_SSL_MAX_URC_LEN                          40
#define AT_SSL_MAX_ADDR_LEN                         40
#define AT_SSL_MAX_PORT_LEN                         10

/* SSL连接读取超时时间, 单位毫秒 */
#define AT_SSL_READ_TIMEOUT_MS                      1000

/* SSL协商阶段的最大超时时间, 单位秒 */
#define AT_SSL_NEGOTIATE_TIME_MIN                   10
#define AT_SSL_NEGOTIATE_TIME_MAX                   300
#define AT_SSL_NEGOTIATE_TIME_DEFAULT               300

/* PDP上下文标识符 */
#define AT_SSL_PDP_CTX_ID_MIN                       1
#define AT_SSL_PDP_CTX_ID_MAX                       15

/* SSL上下文标识符 */
#define AT_SSL_CTX_ID_MIN                           0
#define AT_SSL_CTX_ID_MAX                           5
#define AT_SSL_CTX_ID_CHECK(ID) ((ID) >= AT_SSL_CTX_ID_MIN  && (ID) <= AT_SSL_CTX_ID_MAX)

/* Socket索引 */
#define AT_SSL_CLIENT_ID_MIN                        0
#define AT_SSL_CLINET_ID_MAX                        11

/* 发送数据的长度, 单位字节 */
#define AT_SSL_SEND_LEN_MIN                         1
#define AT_SSL_SEND_LEN_MAX                         1460

/* 读取数据的长度, 单位字节 */
#define AT_SSL_READ_LEN_MIN                         0
#define AT_SSL_READ_LEN_MAX                         1500

/* AT+ISSLCLOSE执行的超时时间，单 位 取 决 于AT+ISSLCFG="closetimemode" 的 配 置。 */
/* 若closetimemode配置为0，单位为秒；若closetimemode配置为1，则单位为毫秒。*/
#define AT_SSL_CLOSE_TIMEOUT_MIN                    0
#define AT_SSL_CLOSE_TIMEOUT_MAX                    65535
#define AT_SSL_CLOSE_TIMEOUT_DEFAULTE               10

/* Socket服务的COM口 */
#define AT_SSL_USB_MODEM_PORT                       "usbmodem"
#define AT_SSL_USB_AT_PORT                          "usbat"
#define AT_SSL_UART_PORT1                           "uart1"
#define AT_SSL_MUX_PORT1                            "cmux1"
#define AT_SSL_MUX_PORT2                            "cmux2"
#define AT_SSL_MUX_PORT3                            "cmux3"
#define AT_SSL_MUX_PORT4                            "cmux4"

#define AT_MBEDTLS_SEED_BUF                         "at_mbedtls"    /* 为计数器模式伪随机数生成器（CTR_DRBG）提供种子数据。*/

#define AT_SSL_MTU                                  (1460)          /* MTU */
#define AT_SSL_MTU_CHECK(mtu)                       ((mtu) <= AT_SSL_MTU )

#define AT_SSL_RECV_BUF_SIZE_MIN                    (10240)         /* 数据接收缓存大小 */
#define AT_SSL_RECV_BUF_SIZE_MAX                    (524288)
#define AT_SSL_RECV_BUF_SIZE_DEFAULT                (10240)

/* 十六进制数值，SSL 加密套件 */
#define AT_SSL_CIPHER_SUITES                                                \
    X_MACRO(AT_TLS_RSA_WITH_AES_256_CBC_SHA,                    0x0035)     \
    X_MACRO(AT_TLS_RSA_WITH_AES_128_CBC_SHA,                    0x002F)     \
    X_MACRO(AT_TLS_RSA_WITH_RC4_128_SHA,                        0x0005)     \
    X_MACRO(AT_TLS_RSA_WITH_RC4_128_MD5,                        0x0004)     \
    X_MACRO(AT_TLS_RSA_WITH_3DES_EDE_CBC_SHA,                   0x000A)     \
    X_MACRO(AT_TLS_RSA_WITH_AES_256_CBC_SHA256,                 0x003D)     \
    X_MACRO(AT_TLS_ECDH_ECDSA_WITH_RC4_128_SHA,                 0xC002)     \
    X_MACRO(AT_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA,            0xC003)     \
    X_MACRO(AT_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA,             0xC004)     \
    X_MACRO(AT_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA,             0xC005)     \
    X_MACRO(AT_TLS_ECDHE_ECDSA_WITH_RC4_128_SHA,                0xC007)     \
    X_MACRO(AT_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA,           0xC008)     \
    X_MACRO(AT_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,            0xC009)     \
    X_MACRO(AT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,            0xC00A)     \
    X_MACRO(AT_TLS_ECDHE_RSA_WITH_RC4_128_SHA,                  0xC011)     \
    X_MACRO(AT_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA,             0xC012)     \
    X_MACRO(AT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,              0xC013)     \
    X_MACRO(AT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,              0xC014)     \
    X_MACRO(AT_TLS_ECDH_RSA_WITH_RC4_128_SHA,                   0xC00C)     \
    X_MACRO(AT_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA,              0xC00D)     \
    X_MACRO(AT_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA,               0xC00E)     \
    X_MACRO(AT_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA,               0xC00F)     \
    X_MACRO(AT_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,         0xC023)     \
    X_MACRO(AT_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,         0xC024)     \
    X_MACRO(AT_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256,          0xC025)     \
    X_MACRO(AT_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384,          0xC026)     \
    X_MACRO(AT_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,           0xC027)     \
    X_MACRO(AT_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384,           0xC028)     \
    X_MACRO(AT_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256,            0xC029)     \
    X_MACRO(AT_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384,            0xC02A)     \
    X_MACRO(AT_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,           0xC02F)     \
    X_MACRO(AT_MBEDTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,   0xC030)

#define AT_SSL_SEND_TIMEOUT_MIN                                 (0)     /* 单位:s, > 模式时, 数据输入超时时间 */
#define AT_SSL_SEND_TIMEOUT_MAX                                 (120)
#define AT_SSL_SEND_TIMEOUT_DEFAULT                             (30)


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum
{
    AT_SSL_CMD_UNKNOWN,                         /* 不能识别的命令 */
    AT_SSL_CMD_VERSION,                         /* sslversion */
    AT_SSL_CMD_CIPHER_SUITE,                    /* ciphersuite */
    AT_SSL_CMD_CACERT,                          /* cacert */
    AT_SSL_CMD_CACERTEX,                        /* cacertex */
    AT_SSL_CMD_CLIENT_CERT,                     /* clientcert */
    AT_SSL_CMD_CLIENT_KEY,                      /* clientkey */
    AT_SSL_CMD_SECLEVEL,                        /* seclevel */
    AT_SSL_CMD_IGNORE_LOCAL_TIME,               /* ignorelocaltime */
    AT_SSL_CMD_NEGOTIATE_TIME,                  /* negotiatetime */
    AT_SSL_CMD_SNI,                             /* sni */
    AT_SSL_CMD_CLOSE_TIME_MODE,                 /* closetimemode */
    AT_SSL_CMD_IGNORE_MULTI_CERTCHAIN_VERIFY,   /* ignoremulticertchainverify */
    AT_SSL_CMD_IGNORE_INVALID_CERT_SIGN,        /* ignoreinvalidcertsign */
    AT_SSL_CMD_SESSION_CACHE,                   /* session_cache */
    AT_SSL_CMD_MAX
}AT_SslCmdType;     /* 命令类型 */

typedef struct
{
    char cmdName[AT_SSL_MAX_CMD_NAME];
    AT_SslCmdType cmdType;
    int8_t (*queryFunc)(uint8_t channelId, uint8_t ctxId, void *value);
    int8_t (*setFunc)(uint8_t channelId, uint8_t ctxId, void *value);
}AT_SslCfgFunc;

typedef enum
{
    #define X_MACRO(a, b) a = b,
    AT_SSL_CIPHER_SUITES
    #undef  X_MACRO
    AT_TLS_ALL_CIPHER_SUITES = 0xFFFF,
} AT_SslCipherSuites;   /* SSL加密套件 */

typedef enum
{
    AT_SSL_VERSION_SSL_3_0 = 0,                 /* SSL 3.0 */
    AT_SSL_VERSION_TLS_1_0,                     /* TLS 1.0 */
    AT_SSL_VERSION_TLS_1_1,                     /* TLS 1.1 */
    AT_SSL_VERSION_TLS_1_2,                     /* TLS 1.2 */
    AT_SSL_VERSION_ALL,                         /* 全部 */
} AT_SslVersion;    /* SSL版本 */

typedef enum
{
    AT_SSL_NO_IGNORE_LTIMES = 0,                /* 不忽略 */
    AT_SSL_IGNORE_LTIMES,                       /* 忽略 */
} AT_SslIgnoreLtimes;   /* 是否忽略证书有效性验证 */

typedef enum
{
    AT_SSL_NO_AUTH_MODE = 0,                    /* 无身份验证模式 */
    AT_SSL_SERVER_AUTH,                         /* 进行服务器身份验证 */
    AT_SSL_CLIENT_SERVER_AUTH,                  /* 如果远程服务器要求，则进行服务器和客户端身份验证 */
} AT_SslSeclevel;   /* 身份验证模式 */

typedef enum
{
    AT_SSL_CLOSE_SERVER_NAME_INDICATION = 0,    /* 关闭服务器名称指示功能 */
    AT_SSL_OPEN_SERVER_NAME_INDICATION,         /* 打开服务器名称指示功能 */
} AT_SslSni;    /* 打开/关闭服务器名称指示功能 */

typedef enum
{
    AT_SSL_DISABLE_CLOSE_DELAY = 0,              /* 禁用, 此时SSL关闭延迟时间的单位为秒 */
    AT_SSL_ENABLE_CLOSE_DELAY,                   /* 启用, 此时SSL关闭延迟时间的单位为毫秒 */
} AT_SslCloseTimeMode;  /* 启用/禁用关闭SSL连接的延迟 */

typedef enum
{
    AT_SSL_DISABLE_CACHE = 0,                   /* 关闭SSL会话恢复功能 */
    AT_SSL_ENABLE_CACHE,                        /* 打开SSL会话恢复功能*/
} AT_SslCacheEnable;

typedef enum
{
    AT_SSL_CACHE_MODE = 0,                      /* 缓存模式 */
    AT_SSL_DIRECT_MODE,                         /* 直吐模式 */
    AT_SSL_TRANSPARENT,                         /* 透传模式 */
} AT_SslAccessMode;

typedef enum
{
    AT_SSL_CONN_STATE_INITIAL       = 0,        /* 连接尚未建立 */
    AT_SSL_CONN_STATE_OPENING       = 1,        /* 客户端正在连接 */
    AT_SSL_CONN_STATE_CONNECTED     = 2,        /* 客户端连接已建立 */
    AT_SSL_CONN_STATE_CLOSING       = 4,        /* 连接正在关闭 */
} AT_SslSocketState;     /* SSL连接状态 */

typedef enum
{
    AT_SSL_URC_RECV = 0,                        /* 通知主机接收数据 */
    AT_SSL_URC_CLOSED,                          /* 通知异常断开连接 */
    AT_SSL_URC_CONNECT_ABNORMAL,                /* 通知网络连接异常 */
} AT_SslUrcType;     /* SSL URC上报信息类型 */

typedef struct{
    char       *caCert;
    uint32_t    caCertLen;
    char       *clientCert;
    uint32_t    clientCertLen;
    char       *clientPk;                                   /* Public key information */
    uint32_t    clientPkLen;
} AT_SslCertInfo;

typedef struct
{
    int8_t          isUsing;                                /* 配置参数正在使用 */
    uint8_t         sslVersion;                             /* SSL版本 */
    int32_t         cipherSuites[2];                        /* SSL加密套件 */
    uint8_t         ignoreLtime;                            /* 是否忽略证书有效性验证 */
    char            cacertPath[AT_SSL_MAX_PATH_LEN];        /* 受信任CA证书路径 */
    char            clientCertPath[AT_SSL_MAX_PATH_LEN];    /* 客户端证书路径 */
    char            clientKeyPath[AT_SSL_MAX_PATH_LEN];     /* 客户端密钥路径 */
    uint8_t         seclevel;                               /* 身份验证模式 */
    uint16_t        negotiateTime;                          /* SSL协商阶段的最大超时时间 */
    uint8_t         sni;                                    /* 打开/关闭服务器名称指示功能 */
    uint8_t         closeTimeMode;                          /* 启用/禁用关闭SSL连接的延迟 */
    uint8_t         ignoreMulticertchainVerify;             /* 是否忽略多级证书链验证 */
    uint8_t         ignoreInvalidCertsign;                  /* 是否忽略无效证书签名 */
    uint8_t         sessionCacheEnable;                     /* 打开/关闭SSL会话恢复功能 */
    AT_SslCertInfo  certInfo;
} AT_SslCtxParam;    /* SSL上下文配置参数 */

typedef enum
{
    AT_SSL_NO_IGNORE_MULTI_CERT_CHAIN_VERIFY = 0,   /* 不忽略 */
    AT_SSL_IGNORE_MULTI_CERT_CHAIN_VERIFY,          /* 忽略 */
} AT_SslIgnoreMulticertChainVerify; /* 是否忽略多级证书链验证 */

typedef enum
{
    AT_SSL_NO_IGNORE_INVALID_CERTSIGN = 0,          /* 不忽略 */
    AT_SSL_IGNORE_INVALID_CERTSIGN,                 /* 忽略 */
} AT_SslIgnoreInvalidCertsign;  /* 是否忽略无效证书签名 */

typedef enum
{
    AT_SSL_ERR_OPERATION_SUCCESSFUL     = 0,        /* 操作成功 */
    AT_SSL_ERR_UNKNOWN                  = 550,      /* 未知错误 */
    AT_SSL_ERR_OPERATION_BLOCKED        = 551,      /* 操作被阻止 */
    AT_SSL_ERR_INVALID_PARAMETER        = 552,      /* 无效参数 */
    AT_SSL_ERR_MEMORY_NOT_ENOUGH        = 553,      /* 内存不足 */
    AT_SSL_ERR_CREATE_SOCKET_FAIL       = 554,      /* 创建Socket失败 */
    AT_SSL_ERR_OPERATION_NOT_SUPPORT    = 555,      /* 不支持该操作 */
    AT_SSL_ERR_SOCKET_BIND_FAIL         = 556,      /* Socket绑定失败 */
    AT_SSL_ERR_SOCKET_LISTEN_FAIL       = 557,      /* Socket监听失败 */
    AT_SSL_ERR_SOCKET_WIRTE_FAILED      = 558,      /* Socket写入失败 */
    AT_SSL_ERR_SOCKET_READ_FAILED       = 559,      /* Socket读取失败 */
    AT_SSL_ERR_SOCKET_ACCEPT_FAILED     = 560,      /* Socket接受失败 */
    AT_SSL_ERR_OPEN_PDP_FAIL            = 561,      /* 打开PDP上下文失败 */
    AT_SSL_ERR_CLOSE_PDP_FAIL           = 562,      /* 关闭PDP上下文失败 */
    AT_SSL_ERR_SOCKET_USED              = 563,      /* Socket标识已被使用 */
    AT_SSL_ERR_DNS_BUSY                 = 564,      /* DNS业务繁忙 */
    AT_SSL_ERR_DNS_PARSE_FAIL           = 565,      /* DNS解析失败 */
    AT_SSL_ERR_SOCKET_CONNECT_FAIL      = 566,      /* Socket连接失败 */
    AT_SSL_ERR_SOCKET_CLOSED            = 567,      /* Socket已关闭 */
    AT_SSL_ERR_OPERATION_BUSY           = 568,      /* 操作繁忙 */
    AT_SSL_ERR_OPERATION_TIMEOUT        = 569,      /* 操作超时 */
    AT_SSL_ERR_PDP_BREAK_DOWN           = 570,      /* PDP上下文崩溃 */
    AT_SSL_ERR_CANCEL_SEND              = 571,      /* 取消发送 */
    AT_SSL_ERR_OPERATION_NOT_ALLOW      = 572,      /* 不允许操作 */
    AT_SSL_ERR_APN_NO_CONFIG            = 573,      /* 未配置APN */
    AT_SSL_ERR_PORT_BUSY                = 574,      /* 端口繁忙 */
}AT_SslErrCode;    /* SSL Error错误码 */

typedef enum
{
    AT_SSL_SEND_INDICATE_NONE           = 0,
    AT_SSL_SEND_INDICATE_WITHOUT_LEN,
    AT_SSL_SEND_INDICATE_WITH_LEN,
}AT_SslSendIndicateMethod;  /* 以">(输入数据)" 形式发送数据的方式 */

typedef struct
{
    uint32_t        totalLen;    /* 存储数据的总长度 */
    uint32_t        writeLen;    /* 存储数据的当前实际存储的长度(指针指向的位置) */
    uint8_t        *buf;         /* 存储数据的 buf */
}AT_SslDataTransparent;     /* 以透传模式或以">(输入数据)" 形式发送数据时存储缓存 buf */


/************************************************************************************
 *                                 mbedtls相关定义
 ************************************************************************************/
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_PLATFORM_C)
#include "mbedtls/iot.h"
#endif

#include <stdint.h>
#include <stdio.h>
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_internal.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/net_sockets.h"

#if defined(MBEDTLS_SSL_CACHE_C)
#include "mbedtls/ssl_cache.h"
#endif

#if defined(MBEDTLS_THREADING_ALT)
#include "threading_alt.h"
#endif

#define MBEDTLS_VERIFY_SIZE            ((uint32_t) (1024))

typedef struct
{
    char host[AT_SSL_MAX_ADDR_LEN];
    char port[AT_SSL_MAX_PORT_LEN];

    mbedtls_ssl_context         *ssl;
    mbedtls_ssl_config          *conf;
    mbedtls_entropy_context     *entropy;
    mbedtls_ctr_drbg_context    *ctrDrbg;
    mbedtls_net_context         *serverFd;
#if defined(MBEDTLS_SSL_CACHE_C)
    mbedtls_ssl_cache_context   *cache;
#endif

    mbedtls_x509_crt            *cacert;
    mbedtls_x509_crt            *clientCert;
    mbedtls_pk_context          *clientPk;
} AT_SslContext;

typedef struct
{
    AT_SslCtxParam              *sslCtxParam;
    AT_SslContext                sslContex;
} AT_SslContextEx;

typedef struct
{
    AT_SslSlistNode              node;
    AT_SslContextEx              sslCtxEx;
}AT_SslContextExList;

typedef struct
{
    uint32_t  cacheRecvTotalLen;    /* 从 LWIP 接收(recvfrom)到的, 缓存区中存放的已有数据长度, 单位:字节 */
    uint64_t  sentByte;             /* 已发送的数据量, 单位:字节 */
    uint64_t  ackedByte;            /* 对端已确认接收到的数据量统计, 单位:字节 */
    uint64_t  receivedByte;         /* 本地已接收到的数据量统计, 单位:字节 */
    uint64_t  readByte;             /* 本地已接收到的已读取的数据量统计, 单位:字节 */
}AT_SslParamStatistics;     /* TCP/IP 发送接收数据信息统计(仅对本次连接有效, 关闭连接成功时置初始值0) */

typedef struct
{
    AT_SslSlistNode      node;      /* 链表节点 */
    uint16_t             readLen;   /* 一个链表节点已读取的数据长度(针对 TCP 时有用) */
    uint16_t             totalLen;  /* 一个链表节点存储的数据总长度 */
    char                *buf;       /* 存储数据 buf */
}AT_SslDataList;     /* 数据存储链表 */

#define AT_SSL_TRANS_SEND_WAITTM_DEFAULT     (2000)

typedef struct
{
    uint16_t  localPort;        /* 本地端口。范围：0-65535 */
    uint8_t   state;
    uint8_t   pdpId;
    uint8_t   clientId;
    uint8_t   sslCtxId;
    uint8_t   channelId;
    uint8_t   sendFlag;         /* 数据发送标志 */
    uint8_t   sendIndicate;     /* 以">(输入数据)" 形式发送数据的标志 */
    uint8_t   waitTimeoutFlag;  /* 透传模式下, 输入数据超时时间到达的标志. 数值1表示进入定时器开始计时, 超时时间到达直接发送数据 */
    uint8_t   waitIndicate;     /* 透传模式下, 发送数据的标志 */
    uint16_t  sendLen;          /* AT 命令输入的需要发送(send)的数据长度, 单位:字节 */
    uint8_t   accessMode;

    AT_SslTimer indicateTimer;  /* TCP/IP >模式超时定时器指针 */
    AT_SslTimer transTimer;     /* TCP/IP 透传超时定时器指针 */

    AT_SslContext           sslCtx;
    AT_SslCtxParam         *sslCtxParam;
    AT_SslParamStatistics   statistics;
} AT_SslClientContext;

#define AT_SSL_EXIT_TRANSPARENT_CMD          "+++"                 /* 退出透传模式的命令 */

typedef enum
{
    AT_SSL_SEND_TRANSPARENT_DISABLE           = 0,
    AT_SSL_SEND_TRANSPARENT_ENABLE,
}AT_SslSendTransparentFlag;        /* 透传模式下发送数据的标志 */

typedef struct
{
    char       *ca_cert;
    uint32_t    ca_cert_len;
    char       *client_cert;
    uint32_t    client_cert_len;
    char       *client_pk;
    uint32_t    client_pk_len;
    char       *client_pwd;
    uint32_t    client_pwd_len;
} AT_MbedtlsCertInfo;

typedef struct
{
    const char *hostname;
    const char *port;
} AT_MbedtlsShakehandInfo;


/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void AT_SslCfgSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SslCfgTest(uint8_t channelId);
void AT_SslOpenSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SslOpenTest(uint8_t channelId);
void AT_SslSendSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SslSendTest(uint8_t channelId);
void AT_SslRecvSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SslRecvTest(uint8_t channelId);
void AT_SslCloseSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SslCloseTest(uint8_t channelId);
void AT_SslStateSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_SslStateTest(uint8_t channelId);
void AT_SslStateExec(uint8_t channelId);
int8_t AT_SslInit();
AT_SslContextEx* AT_SslMbedtlsEstablish(const char *host, uint16_t port, uint8_t cid, uint8_t sslCtxId, uint8_t connFlag);
void AT_SslMbedtlsDestroy(AT_SslContextEx *sslCtxEx);
int32_t AT_SslRead(mbedtls_ssl_context *ssl, char *buf, size_t len, int timeoutMs);
int32_t AT_SslWrite(mbedtls_ssl_context *ssl, char *buf, size_t len);


#ifdef __cplusplus
}
#endif
#endif // End of __APP_AT_SSL_H__


