/**
 * @file        cm_mqtt.h
 * @brief       MQTT接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      shimingrui
 * @date        2021/09/16
 *
 * @defgroup mqtt mqtt
 * @ingroup NP
 * @{
 */

#ifndef __CM_MQTT_H__
#define __CM_MQTT_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#ifndef  CM_MQTT_CLIENT_MAX
#define CM_MQTT_CLIENT_MAX          6
#endif

#define CM_MQTT_RETAIN_0    (0x0)
#define CM_MQTT_RETAIN_1    (0x1)
#define CM_MQTT_QOS_0       (0x0)
#define CM_MQTT_QOS_1       (0x1 << 1)
#define CM_MQTT_QOS_2       (0x2 << 1)
#define CM_MQTT_DUP_0       (0x0)
#define CM_MQTT_DUP_1       (0x1 << 3)

#define CM_MQTT_TOPIC_SUBSCRIBING   0
#define CM_MQTT_TOPIC_SUBSCRIBED    1
#define CM_MQTT_TOPIC_UNSUBSCRIBING 2

/****************************************************************************
 * Public Types
 ****************************************************************************/
/**锁句柄*/
typedef void *cm_mutex_t;

/** 双端链表元素定义*/
typedef struct _linklist_element_t
{
    void* content;                     /*!< 节点内容*/
    int size;                          /*!< content长度*/
    struct _linklist_element_t* pre;   /*!< 前驱节点*/
    struct _linklist_element_t* next;  /*!< 后继节点*/
} linklist_element_t;

/** 双端链表结构定义*/
typedef struct {
    linklist_element_t* head;           /*!< 表头*/
    linklist_element_t* tail;           /*!< 表尾*/
    int count;                          /*!< 节点个数*/
    int size;                           /*!< 链表占用内存*/
    cm_mutex_t mutex;                        /*!< 互斥锁，用于多线程时链表数据同步*///公共库里的互斥锁
} linklist_t;

struct _cm_mqtt_client_t;
typedef struct _cm_mqtt_client_t cm_mqtt_client_t;

/** client状态信息*/
typedef enum{
    CM_MQTT_STATE_DISCONNECTED, /*!< 连接断开*/
    CM_MQTT_STATE_CONNECTED,    /*!< 已连接*/
    CM_MQTT_STATE_CONNECTING,   /*!< 连接中*/
    CM_MQTT_STATE_RECONNECTING,  /*!< 重连中*/
} cm_mqtt_state_e;

/** 异步状态信息*/
typedef enum{
    CM_MQTT_CONN_STATE_SUCCESS = 0,         /*!< 连接成功*/
    CM_MQTT_CONN_STATE_RECONNECTING,        /*!< 正在重连*/
    CM_MQTT_CONN_STATE_CLIENT_SHUTDOWN,     /*!< client断开*/
    CM_MQTT_CONN_STATE_SERVER_REJECED,      /*!< server拒绝*/
    CM_MQTT_CONN_STATE_SERVER_SHUTDOWN,     /*!< server断开*/
    CM_MQTT_CONN_STATE_PING_TIMEOUT,        /*!< ping超时*/
    CM_MQTT_CONN_STATE_NET_ERR,             /*!< 网络异常*/
    CM_MQTT_CONN_STATE_UNKNOWN_ERR = 255,   /*!< 未知错误*/
} cm_mqtt_conn_state_e;

/** client设置参数枚举，用于mqtt参数设置    */
typedef enum{
    CM_MQTT_OPT_START = 0,
    CM_MQTT_OPT_VERSION = CM_MQTT_OPT_START,
    CM_MQTT_OPT_CID,
    CM_MQTT_OPT_SSL_ENABLE,                 /*!< SSL开启，1：MQTTS，0：MQTT*/
    CM_MQTT_OPT_SSL_ID,                     /*!< SSL通道*/
    CM_MQTT_OPT_PING_CYCLE,                 /*!< 发送ping包的周期，单位:s*/
    CM_MQTT_OPT_PKT_TIMEOUT,                /*!< 发送超时，单位:s*/
    CM_MQTT_OPT_RETRY_TIMES,                /*!< 重传次数*/
    CM_MQTT_OPT_RECONN_MODE,                /*!< 重连模式，0:以固定间隔尝试重连，1:以上次重连间隔的2倍尝试重连*/
    CM_MQTT_OPT_RECONN_TIMES,               /*!< 重连次数*/
    CM_MQTT_OPT_RECONN_CYCLE,               /*!< 重连间隔，单位:s*/
    CM_MQTT_OPT_EVENT,                      /*!< 回调函数集合*/
    CM_MQTT_OPT_EVENT_CONNACK_CB,
    CM_MQTT_OPT_EVENT_PUBLISH_CB,
    CM_MQTT_OPT_EVENT_PUBACK_CB,
    CM_MQTT_OPT_EVENT_PUBREC_CB,
    CM_MQTT_OPT_EVENT_PUBREL_CB,
    CM_MQTT_OPT_EVENT_PUBCOMP_CB,
    CM_MQTT_OPT_EVENT_SUBACK_CB,
    CM_MQTT_OPT_EVENT_UNSUBACK_CB,
    CM_MQTT_OPT_EVENT_PINGRESP_CB,
    CM_MQTT_OPT_EVENT_TIMEOUT_CB,
    CM_MQTT_OPT_END,
} cm_mqtt_option_e;

/** 错误码定义*/
typedef enum{
    CM_MQTT_RET_OK = 0,
    CM_MQTT_RET_INVALID_PARAM = -1,
    CM_MQTT_RET_NOT_CONNECT = -2,
    CM_MQTT_RET_CONNECTING = -3,
    CM_MQTT_RET_ALREADY_CONNECTED = -4,
    CM_MQTT_RET_NET_ERR = -5,
    CM_MQTT_RET_MEM_ERR = -6,
    CM_MQTT_RET_STATE_ERR = -7,
    CM_MQTT_RET_DNS_ERR = -8,
    CM_MQTT_RET_UNKNOWN_ERR = -99,
} cm_mqtt_err_code_e;

/** topic信息*/
typedef struct{
    char state;             /*!< 订阅状态，0:订阅中,1:已订阅,2:取消订阅中*/
    char qos;               /*!< qos等级*/
    unsigned short msg_id;  /*!< 消息ID*/
    int topic_len;          /*!< topic长度*/
    char topic[0];          /*!< topic内容*/
} cm_mqtt_topic_t;

/** 连接选项*/
typedef struct{
    unsigned short hostport;         /*!< 服务器端口号*/
    const char* hostname;   /*!< 服务器域名或IP*/
    const char* clientid;   /*!< clientid*/
    const char* username;   /*!< 用户名*/
    const char* password;   /*!< 密码*/
    const char* will_topic; /*!< 遗嘱标题*/
    const char* will_message;   /*!< 遗嘱信息*/
    int will_message_len;   /*!< 遗嘱信息长度*/
    char will_flag;         /*!< 遗嘱标志*/
    char will_qos;          /*!< qos*/
    char will_retain;       /*!< retain*/
    char clean_session;     /*!< 清除标志*/
    char conn_flags;        /*!< 连接标志*/
    unsigned short keepalive;        /*!< 保活时间*/
} cm_mqtt_connect_options_t;

/** 回调函数*/
typedef struct{
    int (*connack_cb)(cm_mqtt_client_t* client, int session, cm_mqtt_conn_state_e conn_res); /*!< connack*/
    int (*publish_cb)(cm_mqtt_client_t* client, unsigned short msgid, char* topic, int total_len, int payload_len, char* payload);  /*!< s->c publish*/
    int (*puback_cb)(cm_mqtt_client_t* client, unsigned short msgid, char dup);         /*!< puback*/
    int (*pubrec_cb)(cm_mqtt_client_t* client, unsigned short msgid, char dup);         /*!< pubrec*/
    int (*pubrel_cb)(cm_mqtt_client_t* client, unsigned short msgid, char dup);         /*!< pubrel*/
    int (*pubcomp_cb)(cm_mqtt_client_t* client, unsigned short msgid, char dup);        /*!< pubcomp*/
    int (*suback_cb)(cm_mqtt_client_t* client, unsigned short msgid, int count, int qos[]);/*!< s->c suback*/
    int (*unsuback_cb)(cm_mqtt_client_t* client, unsigned short msgid);                 /*!< s->c unsuback*/
    int (*pingresp_cb)(cm_mqtt_client_t* client, int ret);                              /*!< s->c pinrsp*/
    int (*timeout_cb)(cm_mqtt_client_t* client, unsigned short msgid);                  /*!< 发送超时*/
} cm_mqtt_client_cb_t;

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

/****************************************************************************/

/**
 *  @brief 创建mqtt client
 *  
 *  @return 成功返回mqtt client，失败返回NULL
 *  
 *  @details 从client列表中创建空闲的client，最大支持CM_MQTT_CLIENT_MAX个
 */
cm_mqtt_client_t* cm_mqtt_client_create(void);

/**
 *  @brief 销毁mqtt client
 *  
 *  @param [in] client mqtt client
 *  @return 成功返回0，失败返回-1
 *  
 *  @details 清除并释放client，异步
 */
int cm_mqtt_client_destroy(cm_mqtt_client_t* client);

/**
 *  @brief 连接
 *  
 *  @param [in] client mqtt client
 *  @param [in] option 连接选项
 *  @return 成功返回0，失败返回错误码，参考cm_mqtt_err_code_e
 *  
 *  @details 异步接口，连接结果和状态请根据对应回调函数判断
 */
int cm_mqtt_client_connect(cm_mqtt_client_t* client, cm_mqtt_connect_options_t* option);

/**
 *  @brief 发布消息
 *  
 *  @param [in] client mqtt client
 *  @param [in] topic 主题
 *  @param [in] payload 负载
 *  @param [in] payload_len 负载长度
 *  @param [in] publish_flags 固定头部信息
 *  @return 成功返回publish长度，失败返回错误码，参考cm_mqtt_err_code_e
 *  
 *  @details 异步接口，是否发布成功请根据对应回调函数判断
 */
int cm_mqtt_client_publish(cm_mqtt_client_t* client, const char* topic, const char* payload, int payload_len, char publish_flags);

/**
 *  @brief 订阅
 *  
 *  @param [in] client mqtt client
 *  @param [in] topic 主题列表
 *  @param [in] qos qos列表
 *  @param [in] count 主题个数
 *  @return 成功返回发送长度，失败返回错误码，参考cm_mqtt_err_code_e
 *  
 *  @details 一个topic必须对应一个qos值，单个qos值为char型数据，取值范围是0/1/2，表示实际qos0/qos1/qos2 \n
 *           异步接口，是否订阅成功请根据对应回调函数判断
 */
int cm_mqtt_client_subscribe(cm_mqtt_client_t* client, const char* topic[], const char qos[], int count);

/**
 *  @brief 取消订阅
 *  
 *  @param [in] client mqtt client
 *  @param [in] topic 主题列表
 *  @param [in] count 主题个数
 *  @return 成功返回发送长度，失败返回错误码，参考cm_mqtt_err_code_e
 *  
 *  @details 异步接口，是否取消订阅成功请根据对应回调函数判断
 */
int cm_mqtt_client_unsubscribe(cm_mqtt_client_t* client, const char* topic[], int count);

/**
 *  @brief 断开连接
 *  
 *  @param [in] client mqtt客户端
 *  @return 成功返回0，失败返回-1
 *  
 *  @details 异步接口，是否成功断开连接结果请根据对应回调函数判断
 */
int cm_mqtt_client_disconnect(cm_mqtt_client_t* client);

/**
 *  @brief client参数设置
 *  
 *  @param [in] client mqtt客户端
 *  @param [in] option 设置项
 *  @param [in] param 设置参数
 *  @return 成功返回0，失败返回错误码，详见cm_mqtt_err_code_e
 *  
 *  @details 
 */
int cm_mqtt_client_set_opt(cm_mqtt_client_t* client, cm_mqtt_option_e option, void* param);

/**
 *  @brief 获取client当前的消息ID
 *  
 *  @param [in] client mqtt客户端
 *  @return 成功返回消息ID，失败返回-1
 *  
 *  @details 
 */
int cm_mqtt_client_get_msgid(cm_mqtt_client_t* client);

/**
 *  @brief 获取client当前的状态
 *  
 *  @param [in] client mqtt客户端
 *  @return 成功返回client当前的状态，详见cm_mqtt_state_e，失败返回-1
 *  
 *  @details 
 */
int cm_mqtt_client_get_state(cm_mqtt_client_t* client);

/**
 *  @brief 获取client订阅列表
 *  
 *  @param [in] client mqtt客户端
 *  @return 成功返回订阅列表，失败返回NULL
 *  
 *  @details 
 */
linklist_t* cm_mqtt_client_get_sub_topics(cm_mqtt_client_t* client);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_MQTT_H__ */

/** @}*/


