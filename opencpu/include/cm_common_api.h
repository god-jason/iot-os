/**
 * @file        cm_onemo_at.c
 * @brief       OneMO用于公共调用的API函数(平台适配)
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By chenxy
 * @date        2021/08/12
 *
 * @defgroup onemo
 * @ingroup NP
 * @{
 */

#ifndef __CM_COMMON_API_H__
#define __CM_COMMON_API_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/
#include <os.h>
#include "cmsis_os2.h"
#include "vfs.h"


#include "stdbool.h"   //zhangQ添加bool的头文件，20220815

#include "lwip/api.h"
#include "lwip/dns.h"

#include "net_api_common.h"




#define NETIF_NAME_SIZE      (2)
#define NETIF_IP6_ADDR_NUM   (2)
typedef struct ifaddrs
{
    char ifa_name[NETIF_NAME_SIZE];
    os_uint8_t cid;
    os_uint8_t channelId;
    ip4_addr_t ifa_ip4_addr;
    ip4_addr_t ifa_ip4_netmask;
    ip6_addr_t ifa_ip6_addr[NETIF_IP6_ADDR_NUM];
}ifaddrs_t;

enum lwip_port_get_type
{
    LWIP_APP_PORT = 0,
    NAT_TCP_PORT,
    NAT_UDP_PORT,

    MAX_TYPE_PORT,
};

// cmiot
err_t lwip_gethostbyname_async(const char *host_name, ip_addr_t *ip_addr, u8_t addr_num, dns_found_callback found_cb, void *cb_param, int addr_type);

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
//#define CM_DEBUG_LOG_SHELL
#ifdef CM_DEBUG_LOG_SHELL
#define cm_log os_kprintf
#define CMEXAT_LOGI(fmt, args...)   os_kprintf("[MEX] [%s,%d]" fmt "\r\n", __FUNCTION__, __LINE__,##args)

#define CMCOM_LOGI(fmt, args...)   os_kprintf("[CM]" fmt "\r\n", ##args)
#define CMCOM_LOGW(fmt, args...)   os_kprintf("[CM_W]" fmt "\r\n", ##args)
#define CMCOM_LOGE(fmt, args...)   os_kprintf("[CM_E]" fmt "\r\n", ##args)

#define CMSOCK_LOGI(fmt, args...)   os_kprintf("[MSOCK]" fmt "\r\n", ##args)
#define CMSOCK_LOGD(fmt, args...)   os_kprintf("[MSOCK_D]" fmt "\r\n", ##args)
#define CMSOCK_LOGE(fmt, args...)   os_kprintf("[MSOCK_E]" fmt "\r\n", ##args)

#define CMASYDNS_LOGI(fmt, args...)   os_kprintf("[MDNS]" fmt "\r\n", ##args)
#define CMASYDNS_LOGD(fmt, args...)   os_kprintf("[MDNS_D]" fmt "\r\n", ##args)
#define CMASYDNS_LOGE(fmt, args...)   os_kprintf("[MADNS_E]" fmt "\r\n", ##args)

#define CMELOOP_LOGI(fmt, args...) os_kprintf("[MELP]" fmt "\r\n", ##args)
#define CMELOOP_LOGD(fmt, args...) os_kprintf("[MELP_D]" fmt "\r\n", ##args)
#define CMELOOP_LOGE(fmt, args...) os_kprintf("[MELP_E]" fmt "\r\n", ##args)

#define CMHTTP_LOGI(fmt, args...)  os_kprintf("[MHTTP]" fmt "\r\n", ##args)
#define CMMQTT_LOGI(fmt, args...)  os_kprintf("[MMQTT]" fmt "\r\n", ##args)
#define CMSSL_LOGI(fmt, args...)   os_kprintf("[MSSL]" fmt "\r\n", ##args)

#define CMTCP_LOGI(fmt, args...)  os_kprintf("[MTCP]" fmt "\r\n", ##args)
#define CMTCP_LOGD(fmt, args...)  os_kprintf("[MTCP_D]" fmt "\r\n", ##args)
#define CMTCP_LOGE(fmt, args...)  os_kprintf("[MTCP_E]" fmt "\r\n", ##args)
#else
#define cm_log app_printf
#define CMEXAT_LOGI(fmt, args...)   app_printf("[MEXAT][%s,%d]" fmt "\r\n", __FUNCTION__, __LINE__,##args)

#define CMCOM_LOGI(fmt, args...)   app_printf("[CM]" fmt "\r\n", ##args)
#define CMCOM_LOGW(fmt, args...)   app_printf("[CM_W]" fmt "\r\n", ##args)
#define CMCOM_LOGE(fmt, args...)   app_printf("[CM_E]" fmt "\r\n", ##args)

#define CMSOCK_LOGI(fmt, args...)   app_printf("[MSOCK]" fmt "\r\n", ##args)
#define CMSOCK_LOGD(fmt, args...)   app_printf("[MSOCK_D]" fmt "\r\n", ##args)
#define CMSOCK_LOGE(fmt, args...)   app_printf("[MSOCK_E]" fmt "\r\n", ##args)

#define CMASYDNS_LOGI(fmt, args...)   app_printf("[MDNS]" fmt "\r\n", ##args)
#define CMASYDNS_LOGD(fmt, args...)   app_printf("[MDNS_D]" fmt "\r\n", ##args)
#define CMASYDNS_LOGE(fmt, args...)   app_printf("[MDNS_E]" fmt "\r\n", ##args)

#define CMELOOP_LOGI(fmt, args...) app_printf("[MELP]" fmt "\r\n", ##args)
#define CMELOOP_LOGD(fmt, args...) app_printf("[MELP_D]" fmt "\r\n", ##args)
#define CMELOOP_LOGE(fmt, args...) app_printf("[MELP_E]" fmt "\r\n", ##args)

#define CMHTTP_LOGI(fmt, args...)  app_printf("[MHTTP]" fmt "\r\n", ##args)
#define CMMQTT_LOGI(fmt, args...)  app_printf("[MMQTT]" fmt "\r\n", ##args)
#define CMSSL_LOGI(fmt, args...)   app_printf("[MSSL]" fmt "\r\n", ##args)

#define CMTCP_LOGI(fmt, args...)  app_printf("[MTCP]" fmt "\r\n", ##args)
#define CMTCP_LOGD(fmt, args...)  app_printf("[MTCP_D]" fmt "\r\n", ##args)
#define CMTCP_LOGE(fmt, args...)  app_printf("[MTCP_E]" fmt "\r\n", ##args)
#endif
/*valid PDP CID range: 1~8 */
#define AT_MIN_CID              1
#define AT_MAX_CID              8

#define MD5_LEN (32)

#define CM_SUPPORT_PRINTLINE

#ifdef CM_MEMLEAK_SUPPORT
#define cm_malloc(n)        cm_dbg_malloc(n)
#define cm_realloc(p,s)     cm_dbg_realloc(p, s)
#define cm_calloc(n,s)      cm_dbg_calloc(n, s)
#define cm_free(buf)    \
    do                  \
    {                   \
        if (buf)        \
        {               \
            cm_dbg_free(buf);  \
            buf = NULL; \
        }               \
    } while (0)
#else
#define cm_malloc(n)           malloc(n)
#define cm_realloc(n,s)        realloc(n, s)
#define cm_calloc(n,s)         calloc(n, s)
#define cm_free(buf)    \
    do                  \
    {                   \
        if (buf)        \
        {               \
            free(buf);  \
            buf = NULL; \
        }               \
    } while (0)
#endif

//File system adaptation
#define CM_FS_SEEK_SET      (0)      /*!< SEEK_SET，文件开头 */
#define CM_FS_SEEK_CUR      (1)      /*!< SEEK_CUR，当前位置 */
#define CM_FS_SEEK_END      (2)      /*!< SEEK_END，文件结尾 */

//根据不同文件系统句柄类型定义
#ifdef CM8610_SDK
/**FS句柄无效值*/
#define CM_FILE_FD_ERROR    -1
// vfs_posix.h in CM8610
typedef int cm_fs_t;
#else
/**FS句柄无效值*/
#define CM_FILE_FD_ERROR    NULL
typedef VFS_File * cm_fs_t;
#endif
typedef struct
{
    uint32_t free_size;  /*!< 当前可用文件系统大小 */
    uint32_t total_size; /*!< 文件系统总大小 */
} cm_fs_system_info_t;

/*
 * 文件打开模式：
 *  0 按O_CREAT|O_RDWR|O_TRUNC模式打开, 读写方式打开，清空文件，新建文件(wb+)
 *  1 按O_CREAT|O_RDWR|O_APPEND模式打开,读写方式打开，追加文件，新建文件(ab+)
 *  2 按O_RDONLY模式打开, 只读方式打开，不新建(rb)
 *  3 按O_WRONLY|O_CREAT|O_TRUNC模式打开, 只写(O_WRONLY)，从头开始并清除已有数据(O_TRUNC)，文件不存在时自动创建(O_CREAT)(wb)
 *  4 按O_WRONLY|O_APPEND|O_CREAT模式打开, 只写(O_WRONLY)，附加模式(O_APPEND)，文件不存在时自动创建(O_CREAT)(ab)
 */
typedef enum
{
    CM_FILE_OPEN_RD_WR_TRUNC_CREAT,
    CM_FILE_OPEN_RD_WR_APPEND_CREAT,
    CM_FILE_OPEN_READ_ONLY,
    CM_FILE_OPEN_WR_TRUNC_CREAT,
    CM_FILE_OPEN_WR_APPEND_CREAT,
    CM_FILE_OPEN_MODE_MAX,
} cm_file_open_mode_t;


/**根据不同文件系统,定义句柄无效值*/
#define CM_FILE_FD_ERROR    NULL

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef struct
{
    uint8_t    MCC[3];              // 3 digitals
    uint8_t    MNC[2];              // 2 digitals
    uint8_t    band_width;
    uint8_t    earfcn_offset;
    uint8_t    padding;
    uint32_t   earfcn;
    uint16_t   pci;
    int16_t    rsrp;
    int16_t    rsrq;
    int16_t    rssi;
    int16_t    sinr;
} cm_cell_info_t;


typedef struct
{
    uint32_t   earfcn;
    uint16_t   pci;
    int16_t    rsrp;
    uint8_t    flag;
} cm_ncell_info_t;


/**线程句柄*/
typedef void *cm_thread_id_t;

/**线程处理函数*/
typedef void (*cm_thread_func_t)(void *param);

/**消息队列句柄*/
typedef void *cm_message_queue_t;

/**锁句柄*/
typedef void *cm_mutex_t;

/**事件句柄*/
typedef void *cm_eventflags_t;

/**线程句柄*/
typedef void *cm_timer_id_t;

/**定时器回调函数*/
typedef void (*cm_timer_callback_t)(void *param);

/**消息结构体*/
typedef struct
{
    uint16_t        eventid; /*消息id*/
    void *          data; /*消息内容*/
}cm_message_t;

typedef enum{
    CID_UNDEF_OR_UNREGIST = 0,
    CID_ONLY_IPV4_ACT,
    CID_ONLY_IPV6_ACT,
    CID_IPV4V6_ACT,
}cm_cid_status_t;


/**add by cmiot zhangQ，20220816**/
typedef struct
{
  uint32_t tm_sec;  /* Seconds (0-59) */
  uint32_t tm_min;  /* Minutes (0-59) */
  uint32_t tm_hour; /* Hours (0-23) */
  uint32_t tm_mday; /* Day of the month (1-31) */
  uint32_t tm_mon;  /* Month (0-11) */
  uint32_t tm_year; /* Years */
  uint32_t tm_wday; /* Day of the week (0-6),always ignore */
  uint32_t tm_yday; /* Day of the year (0-365),always ignore */
}cmtime_struct_t;

/**add by cmiot chenxy**/
enum CmeErrorGeneralType_Cmiot
{
    /* cmiot errors */
    CM_PROGRESS_ERROR = 4, //phone failure
    CM_MEMORY_ALLOC_FAIL = 23,
    CM_PARAM_INVALID = 50, //no connection to phone

    /* tcpip errors */
#ifdef CM_TCPIP_ENABLE
    CME_ERROR_CMIOT_TYPE_TCP_UNKNOWN_ERROR = 550,
    CME_ERROR_CMIOT_TYPE_TCP_NO_USE = 551,
    CME_ERROR_CMIOT_TYPE_TCP_ALREADY_USED = 552,
    CME_ERROR_CMIOT_TYPE_TCP_NO_CONNECT = 553,
    CME_ERROR_CMIOT_TYPE_TCP_SOCKET_CREATION_FAILED = 554,
    CME_ERROR_CMIOT_TYPE_TCP_SOCKET_BIND_FAILED = 555,
    CME_ERROR_CMIOT_TYPE_TCP_SOCKET_LISTEN_FAILED = 556,
    CME_ERROR_CMIOT_TYPE_TCP_SOCKET_CONNECT_REJECT = 557,
    CME_ERROR_CMIOT_TYPE_TCP_SOCKET_CONNECT_TIMEOUT = 558,
    CME_ERROR_CMIOT_TYPE_TCP_SOCKET_CONNECT_FAILED = 559,
    CME_ERROR_CMIOT_TYPE_TCP_SOCKET_WRITE_FAILED = 560,
    CME_ERROR_CMIOT_TYPE_TCP_SOCKET_READ_FAILED = 561,
    CME_ERROR_CMIOT_TYPE_TCP_SOCKET_ACCEPT_FAILED = 562,
    CME_ERROR_CMIOT_TYPE_PDP_NO_ACTIVE = 570,
    CME_ERROR_CMIOT_TYPE_PDP_ACTIVE_FAILED = 571,
    CME_ERROR_CMIOT_TYPE_PDP_DEACTIVE_FAILED = 572,
    CME_ERROR_CMIOT_TYPE_APN_NOT_CONFIG = 575,
    CME_ERROR_CMIOT_TYPE_PORT_BUSY = 576,
    CME_ERROR_CMIOT_TYPE_NO_IPV4V6 = 577,
    CME_ERROR_CMIOT_TYPE_DNS_FAILED = 580,
    CME_ERROR_CMIOT_TYPE_DNS_BUSY = 581,
    CME_ERROR_CMIOT_TYPE_NTP_BUSY = 582,
#endif

/* MQTT errors */
#ifdef CM_MQTT_ENABLE
    CME_ERROR_CMIOT_TYPE_MQTT_RET_UNKNOWN_ERR              = 600,
    CME_ERROR_CMIOT_TYPE_MQTT_RET_INVALID_PARAM            = 601,
    CME_ERROR_CMIOT_TYPE_MQTT_RET_NOT_CONNECT              = 602,
    CME_ERROR_CMIOT_TYPE_MQTT_RET_CONNECTING               = 603,
    CME_ERROR_CMIOT_TYPE_MQTT_RET_ALREADY_CONNECTED        = 604,
    CME_ERROR_CMIOT_TYPE_MQTT_RET_NET_ERR                  = 605,
    CME_ERROR_CMIOT_TYPE_MQTT_RET_MEM_ERR                  = 606,
    CME_ERROR_CMIOT_TYPE_MQTT_RET_STATE_ERR                = 607,
    CME_ERROR_CMIOT_TYPE_MQTT_RET_DNS_ERR                  = 608,
#endif

    /* http errors */
#ifdef CM_HTTP_ENABLE
    CME_ERROR_CMIOT_TYPE_HTTP_UNKNOWN_ERROR = 650, //未知错误 650
    CME_ERROR_CMIOT_TYPE_HTTP_NO_MORE_FREE_CLIENT = 651, //没有空闲客户端 651
    CME_ERROR_CMIOT_TYPE_HTTP_CLIENT_NOT_CREATE = 652, //客户端未创建 652
    CME_ERROR_CMIOT_TYPE_HTTP_CLIENT_IS_BUSY = 653, //客户端忙 653
    CME_ERROR_CMIOT_TYPE_HTTP_URL_PARSE_FAIL = 654, //URL解析失败 654
    CME_ERROR_CMIOT_TYPE_HTTP_SSL_NOT_ENABLE = 655, //SSL未使能 655
    CME_ERROR_CMIOT_TYPE_HTTP_CONNECT_FAIL = 656, //连接失败 656
    CME_ERROR_CMIOT_TYPE_HTTP_SEND_DATA_FAIL = 657, //数据发送失败 657
    CME_ERROR_CMIOT_TYPE_HTTP_OPEN_FILE_FAIL = 658, //打开文件失败 658
#endif

    CM_TLS_FAIL = 750,
    CM_TLS_INIT_FAIL,
    CM_TLS_VERIFY,
    CM_TLS_HANDSHAKE,

    CM_CERTS_FAIL = 760,
    CM_CERTS_UNVAILD,
    CM_CERTS_NO_EXIST,
    CM_CERTS_IS_EXIST,
    CM_CERTS_WRITE_FIAL,
    CM_CERTS_WRITE_BUSY,
    CM_CERTS_READ_FIAL,
    CM_CERTS_DEL_FIAL,
    CM_CERTS_LARGE,
    CM_CERTS_LOAD_FAIL,

    CM_FWUP_UNKOWN_ERROR = 850,
    CM_FWUP_CFG_ERROR,
    CM_FWUP_URL_ERROR,
    CM_FWUP_WRITE_ERROR,
    CM_FWUP_UPGRADE_ERROR,
    CM_FWUP_DOWNLOAD_ERROR,
    CM_FWUP_RETRY_ERROR,
    CM_FWUP_NONET_ERROR,

    CM_FILE_ERROR = 1050,
    CM_FILE_NO_EXIST,
    CM_FILE_IS_EXIST,
    CM_FILE_LARGER,
    CM_FILE_OPEN_MAX,
    CM_FILE_OPEN,
    CM_FILE_ALREADY_OPEN,
    CM_FILE_WRITE,
    CM_FILE_READ,
    CM_FILE_ONLYREAD,
    CM_FILE_INVALID_FD,
    CM_FILE_DELETE,
    CM_FILE_LIST,
    CM_FILE_NO_SPACE,
    CM_FILE_TIMEOUT,


    CME_ERROR_CMIOT_TYPE_BOTTON,
};

#define CM_FILENAME_MAX 128
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
 * @brief 延时函数
 *
 * @param [in] time_ms     延时时间，单位ms
 *
 * @return
 *
 * @details
 */
void cm_delay(uint32_t time_ms);

/**
 * @brief 主动死机
 *
 * @param [in] cond 判断条件，true 正常/false 死机
 *
 * @return
 *
 * @details
 */
void cm_assert(bool cond);

/**
 * @brief 创建线程
 *
 * @param [in] stack_size       栈大小
 * @param [in] priority         优先级
 * @param [in] func             线程处理函数
 *
 * @return 线程句柄
 *
 * @details 适配平台线程创建接口
 */
cm_thread_id_t cm_thread_create(const char *name, uint32_t stack_size, uint32_t priority, cm_thread_func_t func);

/**
 * @brief 线程调度
 *
 * @return 0 成功；-1 失败
 *
 * @details
 */
int32_t cm_thread_yield(void);

/**
 * @brief 退出线程
 *
 * @return
 *
 * @details 退出线程
 */
void cm_thread_exit(void);

/**
 * @brief 创建消息队列
 *
 * @param [in] inqueue_num      队列长度
 * @param [in] size             消息大小
 *
 * @return 消息队列句柄
 *
 * @details 用于线程中消息处理
 */
cm_message_queue_t cm_messagequeue_create(uint32_t inqueue_num, uint32_t size);

/**
 * @brief 发送消息
 *
 * @param [in] queue        消息队列句柄
 * @param [in] message      消息
 *
 * @return 0 成功；-1 失败
 *
 * @details 用于向线程消息队列发送消息
 */
int32_t cm_messagequeue_send(cm_message_queue_t queue, void *message);

/**
 * @brief 等待消息
 *
 * @param [in] queue        消息队列句柄
 * @param [out] message     消息
 *
 * @return 0 成功；-1 失败
 *
 * @details 用于线程消息队列等待消息
 */
int32_t cm_messagequeue_wait(cm_message_queue_t queue, void *message);

/**
 * @brief 删除消息队列
 *
 * @param [in] queue        消息队列句柄
 *
 * @return 0 成功；-1 失败
 *
 * @details 删除消息队列
 */
int32_t cm_messagequeue_delete(cm_message_queue_t queue);

/**
 * @brief 创建锁
 *
 * @return 句柄
 *
 * @details 创建锁
 */
cm_mutex_t cm_mutex_create(void);

/**
 * @brief 获取锁
 *
 * @param [in] mutexid        句柄
 * @return 0 成功；<0 失败
 *
 * @details 获取锁
 */
int32_t cm_mutex_take(cm_mutex_t mutexid);

/**
 * @brief 释放锁
 *
 * @param [in] mutexid        句柄
 * @return 0 成功；<0 失败
 *
 * @details 释放锁
 */
int32_t cm_mutex_release(cm_mutex_t mutexid);

/**
 * @brief 删除锁
 *
 * @param [in] mutexid        句柄
 * @return
 *
 * @details 删除锁
 */
void cm_mutex_delete(cm_mutex_t mutexid);

/**
 * @brief 创建事件标志
 *
 * @return 句柄
 *
 * @details 创建事件标志
 */
cm_eventflags_t cm_eventflags_create(void);

/**
 * @brief 发送事件
 *
 * @param [in] eventflagsid         句柄
 * @param [in] flags                事件标志
 * @return 0 成功；<0 失败
 *
 * @details 发送事件
 */
int32_t cm_eventflags_set(cm_eventflags_t eventflagsid, uint32_t flags);

/**
 * @brief 等待事件
 *
 * @param [in] eventflagsid         句柄
 * @param [in] flags                事件标志
 * @param [in] timeout              超时时间
 * @return 0 成功；<0 失败（-2 超时，-1 其他）
 *
 * @details 等待事件
 */
int32_t cm_eventflags_wait(cm_eventflags_t eventflagsid, uint32_t flags, uint32_t timeout);

/**
 * @brief 删除事件
 *
 * @param [in] eventflagsid         句柄
 * @return
 *
 * @details 删除事件
 */
void cm_eventflags_delete(cm_eventflags_t eventflagsid);

/**
 * @brief 创建定时器
 *
 * @param [in] timeout_s       超时时间
 * @param [in] callback        回调函数
 * @param [in] param           回调函数入参
 * @param [in] flag            timer attribution setting
 *
 * @return 定时器句柄
 *
 * @details 单次定时器，单位秒
 */
cm_timer_id_t cm_timer_start(uint32_t timeout_s, cm_timer_callback_t callback, void *param, uint32_t flag);

/**
 * @brief 关闭定时器
 *
 * @param [in] timer_id        定时器ID
 *
 * @return
 *
 * @details
 */
void cm_timer_stop(cm_timer_id_t timer_id);

/**
 * @brief 定时器是否启动
 *
 * @param [in] timer_id        定时器ID
 *
 * @return
 *
 * @details
 */
bool cm_timer_is_running(cm_timer_id_t timer_id);

/**
 * @brief 获取本地时钟（ms）
 *
 * @return 本地时钟
 *
 * @details 获取本地时钟(ms)
 */
uint64_t cm_get_time_ms(void);

/**
 * @brief 获取本地时钟（s）
 *
 * @return 本地时钟
 *
 * @details 获取本地时钟(s)
 */
uint64_t cm_get_time_s(void);

/**
 * @brief 获取系统RTC时间戳
 *
 * @param [in]
 *
 * @return -1 失败；0 成功
 *
 * @details 获取系统RTC时钟
 */
uint64_t cm_rtc_get_time_ms(void);

/**
 * @brief 获取系统RTC时间戳秒
 *
 * @param [in]
 *
 * @return -1 失败；0 成功
 *
 * @details 获取系统RTC时钟
 */
uint64_t cm_rtc_get_time_s(void);

/**
 * @brief 时间戳转换为cmtime_struct_t格式
 *
 * @param [in] 输入的时间戳,定位s

 * @param [out] 输出的cmtime_struct_t格式时间
 *
 * @return -1 失败；0 成功
 *
 * @details 获取系统RTC时钟
 */
void cm_sec_2_cmtime(uint64_t utc_sec, cmtime_struct_t *result);

/**
 * @brief 获取系统RTC时钟
 *
 * @param [in] wall_clock
 *
 * @return -1 失败；0 成功
 *
 * @details 获取系统RTC时钟
 */
int cm_rtc_get_time(cmtime_struct_t *wall_clock);

/**
 * @brief 获取时区
 *
 * @return -1 时区,范围-24~+24
 *
 * @details 获取时区
 */
int cm_get_timeZone(void);

/**
 * @brief ms转换为系统tick
 *
 * @return 返回系统tick数
 *
 * @details ms转换为系统tick
 */
uint32_t cm_ms_to_tick(uint32_t millsec);

/**
 * @brief 获取网络激活状态
 *
 * @param [in] cid        pdp上下文id
 *
 * @return 激活状态，见cm_cid_status_t
 *
 * @details
 */
cm_cid_status_t cm_get_cidstatus(uint8_t cid);

/**
 * @brief 获取已激活的cid
 *
 * @return 已激活的cid
 *
 * @details
 */
uint8_t cm_get_default_cidnum(void);

/**
 * @brief 获取已激活的cid pdp type
 *
 * @return 已激活的pdp上下文
 *
 * @details
 */
cm_cid_status_t cm_get_pdp_type(char bCid);

/**
 * @brief 设置打印串口id
 */
void cm_uart_print_set_id(uint8_t ch_id);
/**
 * @brief 获取打印串口id
 */
uint8_t cm_uart_print_get_id(void);

void cm_sock_setsockopt(int socket, uint8_t cid);
int32_t cgpaddr_get_local_addr(uint8_t cid, ip_addr_t *ipaddr, uint8_t isipv6);


/**
 * @brief 判断ip地址是否是ipv6格式地址
 *
 * @param [in] cp        ip地址字符串
 *
 * @return 1 ipv6地址，0 非ipv6地址
 *
 * @details
 */
bool cm_util_isIPv6(const char *cp);

/**
 * @brief 判断ip地址是否是ipv4格式地址
 *
 * @param [in] str        ip地址字符串
 *
 * @return 1 ipv4地址，0 非ipv4地址
 *
 * @details
 */
bool cm_util_isIPv4(const char *str);

/**
 * brief 16进制数据转换为字符串
 *
 * param [in] hex_ptr         需要转换的16进制buffer
 * param [out] dest_ptr       转换后的字符串buffer
 * param [in] hex_len         需要转换的16进制长度
 * return  转换后的长度
 *
 * details More details
 */
bool cm_util_hex2str(char* hex_ptr, uint8_t* dest_ptr, int hex_len);

/**
 * brief 字符串转换为16进制
 *
 * param [in] str            需要转换的字符串buffer
 * param [out] hex           转换后的hex buffer
 * param [in] str_len        转换前的长度
 * return  0 失败   1 成功
 *
 * details More details
 */
bool cm_util_str2hex(char* str, char* hex, int str_len);

/**
 * brief 字符转换为16进制
 *
 * param [in] character      需要转换的字符
 * param [out] hex           转换后的hex
 * return  0 失败   1 成功
 *
 * details More details
 */
bool cm_util_char2hex(uint8_t character, uint8_t *hex);

#if 0
/**
 * brief 16进制转换为unsigned short类型
 *
 * param [in] character      需要转换的字符
 * param [out] hex           转换后的hex
 * return  0 失败   1 成功
 *
 * details More details
 */
bool cm_util_hex_to_uint16(const uint8_t *source_buf_hex, uint16_t *dest_value, uint16_t source_length);

/**
 *  \brief 字符串转换为整型
 *
 *  \param [in]   s            输入的字符串
 *  \param [in]   ret          转换结果, 0 失败, 1 成功
 *  \return  -1 失败, >=0 转换后的整型
 *
 *  \details More details
 */
int cm_util_atoi(const char* s, bool* ret);
#endif
/**
 * brief 转义字符串解析
 *
 * param [in] str               需要转换的字符
 * param [out] next_start       解析缓存结尾
 * param [out] data_len         转换后的字符串长度
 * return  0 失败   1 成功
 *
 * details 将带转义的字符串转换为实际对应的字符串
 */
char *cm_at_parse_next_escstring(char* str, char** next_start, int *data_len);

/**
 * brief 转义字符串解析
 *
 * param [in] str               需要转换的字符
 * param [out] data_len         转换后的字符串长度
 * return  0 失败   1 成功
 *
 * details 将带转义的字符串转换为实际对应的字符串
 */
char *cm_at_parse_escstring(char* str, int *data_len);

/**
 *  @brief 参数解析
 *
 *  @param [in] cmd 解析缓存
 *  @param [in] next_start 解析缓存结尾
 *  @param [in] format     解析格式
 *  @return 解析后实际参数个数
 *
 *  @details More details
 */
unsigned int  cm_at_parse_all_parameters(char *cmd, char **next_start, char *format, ...);

/**
 *  @brief 参数解析
 *
 *  @param [in] cmd_string at参数
 *  @param [out] param_buffer 解析缓存
 *  @param [out] param_list 解析后参数列表
 *  @param [in] param_max_num 最长参数格式
 *  @return 解析后实际参数个数
 *
 *  @details More details
 */
unsigned int cm_at_parse_cmd(const char *cmd_string, const char *param_buffer, char *param_list[], unsigned int param_max_num);
/* zentao 8299 */
unsigned int cm_at_parse_cmd_notify(const char *cmd_string, const char *param_buffer, char *param_list[], unsigned int param_max_num);

/**
 * @brief at命令结果打印
 *
 * @param [in] at_handle           打印的通道号
 *
 * @param [in] result              打印的错误号
 *
 * @return
 *
 * @details 自动添加回车换行
 */
void cm_uart_printf_result(uint8_t at_handle, uint32_t result);

/**
 * @brief at命令同步结果打印
 *
 * @param [in] str           打印的字符串
 *
 * @return
 *
 * @details
 */
void cm_printf_ch(uint8_t id, const char *str, ...);
void cm_printf_ch_noCRLF(int chid, const char *str, ...);
/**
 * @brief at命令同步结果打印
 *
 * @param [in] string           打印的字符串
 *
 * @return
 *
 * @details
 */
void cm_uart_printf(const uint8_t *string);

/**
 * @brief at命令同步结果打印，带数据打印
 *
 * @param [in] header               头部字符串打印，只能为字符串格式
 * @param [in] header_len           头部数据长度
 * @param [in] data                 数据
 * @param [in] data_len             数据长度
 * @param [in] start_rn_flag        前端是否添加回车换行
 * @param [in] end_rn_flag          后段是否添加回车换行
 *
 * @return
 *
 * @details 按指定数据长度打印，可配置是否自动添加回车换行
 */
void cm_uart_printf_data(const uint8_t *header, uint16_t header_len, const uint8_t *data, uint16_t data_len,
    bool start_rn_flag, bool end_rn_flag);

/**
 * @brief at命令同步结果打印
 *
 * @param [in] string           打印的字符串
 *
 * @return
 *
 * @details
 */
void cm_uart_printf_withoutline(const uint8_t *outdata);

/**
 * @brief at命令同步结果打印，按指定数据长度打印，不自动添加回车换行，需多分配6字节，末尾添加\r\nOK\r\n
 *
 * @param [in] data                 数据
 * @param [in] data_len             数据长度
 *
 * @return
 *
 * @details 按指定数据长度打印，不自动添加回车换行
 */
void cm_uart_printf_data_withoutline( uint8_t *data, uint16_t data_len);

/**
 * @brief URC打印
 *
 * @param [in] fmt           格式化数据
 *
 * @return
 *
 * @details URC格式化打印，最大长度128，自动添加回车换行
 */
void cm_uart_printf_urc(const char *fmt, ...);

/**
 * @brief URC打印，带数据打印
 *
 * @param [in] header               头部字符串打印，只能为字符串格式
 * @param [in] header_len           头部数据长度
 * @param [in] data                 数据
 * @param [in] data_len             数据长度
 *
 * @return
 *
 * @details 按指定数据长度打印，自动添加回车换行
 */
void cm_uart_printf_urc_data(const uint8_t *header, uint16_t header_len, const uint8_t *data, uint16_t data_len);

/**
 * @brief URC打印，带数据打印
 *
 * @param [in] header               头部字符串打印，只能为字符串格式
 * @param [in] header_len           头部数据长度
 * @param [in] data                 数据
 * @param [in] data_len             数据长度
 * @param [in] start_rn_flag        前端是否添加回车换行
 * @param [in] end_rn_flag          后段是否添加回车换行
 *
 * @return
 *
 * @details 按指定数据长度打印，可配置是否自动添加回车换行
 */
void cm_uart_printf_urc_data_with_ctrl_flag(const uint8_t *header, uint16_t header_len, const uint8_t *data, uint16_t data_len,
        bool start_rn_flag, bool end_rn_flag);


/**
 * @brief URC打印，带数据打印
 *
 * @param [in] data                 数据
 * @param [in] data_len             数据长度
 *
 * @return
 *
 * @details 按指定数据长度打印，无回车换行
 */
void cm_uart_printf_urc_data_withoutline(const uint8_t *outdata, uint16_t lenth);


/**
 * @brief 打开文件
 *
 * @param [in] filename           文件名
 * @param [in] flag               打开模式：0 按O_CREAT|O_RDWR|O_TRUNC模式打开
 *                                          1 按O_CREAT|O_RDWR|O_APPEND模式打开
 *
 * @return 小于0 失败/其他 文件句柄
 *
 * @details 打开文件
 */
cm_fs_t cm_file_open(const char *filename, uint8_t flag);

/**
 * @brief 获取当前文件已写入大小
 *
 * @param [in] fd               文件句柄
 *
 * @return 小于0 失败/其他 文件大小
 *
 * @details 获取当前文件已写入大小
 */
int32_t cm_file_get_size(cm_fs_t fd);

/**
 * @brief 写文件
 *
 * @param [in] fd               文件句柄
 * @param [in] size             写入数据长度
 * @param [in] data             数据
 *
 * @return 小于0 失败/其他 实际写入长度
 *
 * @details 写文件
 */
int32_t cm_file_write(cm_fs_t fd, uint32_t len, const uint8_t *data);

/**
 * @brief 写文件
 *
 * @param [in] fd               文件句柄
 * @param [in] size             读取数据长度
 * @param [in] data             数据
 *
 * @return      > 0             读取数据块的个数
 *              <= 0            读到文件尾或出错
 *
 * @details 写文件
 */
int32_t cm_file_read(cm_fs_t fd, uint32_t size, void *data);


/**
 * @brief 关闭文件
 *
 * @param [in] fd               文件句柄
 *
 * @return      0               成功
 *            < 0               失败
 *
 * @details 关闭文件
 */
int32_t cm_file_close(cm_fs_t fd);

/**
 *  @brief 将处于堆栈状态的文件与存储设备中的文件同步
 *
 *  @param [in] fd 文件描述符
 *
 *  @return
 *   = 0 - 同步成功 \n
 *   < 0 - 同步失败
 *
 *  @details 本平台不支持该接口
 */
int32_t cm_fs_sync(cm_fs_t fd);

/**
 *  @brief 文件指针定位
 *
 *  @param [in] fd 文件描述符
 *  @param [in] offset 指针偏移量
 *  @param [in] base 偏移起始点，CM_FS_SEEK_SET：文件开头 CM_FS_SEEK_CUR：当前位置 CM_FS_SEEK_END：文件末尾
 *
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 *
 *  @details offset不支持为负数
 */
int32_t cm_fs_seek(cm_fs_t fd, int32_t offset, int32_t base);

/**
 ************************************************************************************
 * @brief           获取文件偏移.
 *
 * @param[in]       fp              文件句柄;
 *
 * @return          文件偏移
 * @retval          >= 0            当前偏移;
 *                  < 0             失败原因;
 ************************************************************************************
 */
long cm_fs_ftell(cm_fs_t fd);

/**
 *  @brief 删除文件
 *
 *  @param [in] file_name 文件路径
 *
 *  @return
 *   >= 0 - 成功 \n
 *   <  0 - 失败
 */
int32_t cm_fs_delete(const char *file_name);

/**
 *  @brief 删除目录, 目录为空时方可删除.
 *
 *  @param [in] dir_name 目录路径
 *
 *  @return
 *   >= 0 - 成功 \n
 *   <  0 - 失败
 */
int32_t cm_fs_dir_delete(const char *dir_name);

/**
 *  @brief 更改文件名
 *
 *  @param [in] src  原文件名
 *  @param [in] dest 新文件名
 *
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 */
int32_t cm_fs_move(const char *src, const char *dest);

/**
 *  @brief 检查文件是否存在
 *
 *  @param [in] file_name 文件名称
 *
 *  @return
 *   = 1 - 存在 \n
 *   = 0 - 不存在
 *
 *  @details 文件系统中无路径/文件夹功能，勿在path入参中传入路径信息
 */
int32_t cm_fs_exist(const char *file_name);

/**
 *  @brief 获取文件大小
 *
 *  @param [in] file_name 文件路径
 *  @return
 *
 *   >= 0 - 文件长度 \n
 *   <  0 - 操作失败
 *
 *  @details More details
 */
int32_t cm_fs_filesize(const char *file_name);
int32_t cm_fs_file_size_by_fd(cm_fs_t fd);

/**
 *  @brief 创建文件夹
 *
 *  @param [in] path 文件夹路径
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败 \n
 *
 *  @details 文件路径以/开头是根目录，/usr存放应用数据，/tmp存放临时数据
 */
int32_t cm_fs_mkdir(const char *path);

/**
 *  @brief 删除文件夹
 *
 *  @param [in] path 文件夹路径
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败 \n
 *
 *  @details 只有当目录为空目录时，才应删除该目录 \n
 */
int32_t cm_fs_rmdir(const char *path);

/**
 *  @brief 获取文件系统信息
 *
 *  @param [out] info 文件系统信息
 *
 *  @return
 *   = 0 - 成功 \n
 *   < 0 - 失败
 */
int32_t cm_fs_getinfo(cm_fs_system_info_t *info);

/* 计算md5 */
int32_t cm_fs_md5(const char *name, int32_t len, uint8_t md5[16]);

/**
 * @brief 获取imei
 *
 * @param [out] imei            数据缓存
 *
 * @return 0 失败；其他 实际获取的数据长度
 *
 * @details 获取imei
 */
int32_t cm_get_imei(uint8_t *imei);

/**
 * @brief 获取imsi
 *
 * @param [out] imsi            数据缓存
 *
 * @return 0 失败；其他 实际获取的数据长度
 *
 * @details 获取imsi
 */
int32_t cm_get_imsi(uint8_t *imsi);

/**
 * @brief 获取iccid
 *
 * @param [out] piccid          数据缓存
 *
 * @return 0 失败；其他 实际获取的数据长度
 *
 * @details 获取iccid
 */
int32_t cm_get_iccid(uint8_t *piccid);

/**
 * @brief 获取底层缓存数据大小
 *
 * @param [in] socket           socket句柄
 *
 * @return -1 失败；>=0 可读数据长度
 *
 * @details 获取底层缓存数据大小
 */
int32_t cm_get_socket_remaining_len(int32_t socket);

/**
 * @brief 获取可写数据长度
 *
 * @param [in] socket           socket句柄
 *
 * @return -1 失败；>=0 可写数据长度
 *
 * @details 获取可写数据长度
 */
int32_t cm_get_socket_send_available(int32_t socket);

/**
 * @brief 获取软件版本号
 *
 * @param [in] ver              数据缓存
 *
 * @return -1 失败；>=0 字符串长度
 *
 * @details 获取软件版本号
 */
int32_t cm_get_swver(uint8_t *ver);

/**
 * @brief 获取内控软件版本号
 *
 * @param [in] ver              数据缓存
 *
 * @return -1 失败；>=0 字符串长度
 *
 * @details 获取内控软件版本号
 */
int32_t cm_get_interver(uint8_t *ver);

/**
 * @brief 获取GIT版本号
 *
 * @param [in] ver              数据缓存
 *
 * @return -1 失败；>=0 字符串长度
 *
 * @details 获取GIT版本号
 */
int32_t cm_get_gitver(uint8_t *ver);

/**
 * @brief 获取基线版本号
 *
 * @param [in] ver              数据缓存
 *
 * @return -1 失败；>=0 字符串长度
 *
 * @details 获取基线版本号
 */
int32_t cm_get_basever(uint8_t *ver);

/**
 * @brief 获取厂商信息 CMCC
 *
 * @param [in] ver              数据缓存
 *
 * @return -1 失败；>=0 字符串长度
 *
 * @details 获取厂商信息
 */
int32_t cm_get_manufacture(uint8_t *ver);

/**
 * @brief 获取模块信息
 *
 * @param [in] ver              数据缓存
 *
 * @return -1 失败；>=0 字符串长度
 *
 * @details 获取模块信息
 */
int32_t cm_get_identification(uint8_t *ver);

/**
 * @brief 获取硬件版本号,由cm_write_hwver写入
 *
 * @param [in] ver          数据缓存
 *
 * @return -1 失败；>=0 字符串长度
 *
 * @details 获取硬件版本号,由cm_write_hwver写入
 */
int32_t cm_get_hwver(uint8_t *ver);

/**
 * @brief 写硬件版本号
 *
 * @param [in] ver          写入数据
 *
 * @return -1 失败；>=0 写入实际长度
 *
 * @details 写硬件版本号
 */
int32_t cm_write_hwver(uint8_t *ver);

/**
 * @brief 获取系统RTC时钟
 *
 * @param [in] wall_clock
 *
 * @return -1 失败；0 成功
 *
 * @details 获取系统RTC时钟
 */
int cm_rtc_get_time(cmtime_struct_t *wall_clock);

/**
 * @brief 重启模组
 *
 * @param [in] 0 软重启  1 硬重启
 *
 * @return -1 失败；0 成功
 *
 * @details 重启模组
 */
int32_t cm_sys_reboot(uint8_t mode);

/**
 * @brief 模组关机
 *
 * @param [in] 0 软件关机  1 保存特定命令配置到模组Flash中，并进入软件关机模式
 *
 * @return -1 失败；0 成功
 *
 * @details 模组关机
 */
int32_t cm_sys_poweroff(uint8_t mode);

/**
 * @brief 比较字符串，忽略大小写
 *
 * @param [in]
 *
 * @return 0 成功  其他 失败
 *
 * @details 比较字符串，忽略大小写
 */
int cm_stricmp(const char *str1, const char *str2);

/**
 * @brief 将字符串转换为大写
 *
 * @param [in]  pString 字符串, length 长度
 *
 * @return NULL
 *
 * @details 将字符串转换为大写
 */
void cm_stringToUpper (char *pString, uint64_t length);

#ifdef MBEDTLS_MD5
/**
 * @brief md5校验imei,并进行备份
 *
 * @param [in]  md5_str md5码
 *
 * @return NULL
 *
 * @details md5校验imei,并进行备份
 */
int32_t cm_imei_backup(char * md5_str);
#endif

int dns_domain_is_legal(char* domain);


void cm_psm_lock(void);

void cm_psm_unlock(void);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_UTIL_H__ */

/** @}*/

