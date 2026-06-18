/*********************************************************
 *  @file    cm_demo_mqtt.c
 *  @brief   OpenCPU mqtt示例
 *  Copyright (c) 2023 China Mobile IOT.
 *  All rights reserved.
 *  created by ShiMingRui 2023/6/29
 ********************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/
#if defined(CM_DEMO_MQTT_SUPPORT)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lwip/sockets.h"
#include "cm_ssl.h"
#include "cm_mqtt.h"



/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define CM_MQTT_CLIENT_MAX          5
#define cm_demo_printf osPrintf

/** An enumeration of the PUBLISH flags. */
typedef enum
{
    CM_MQTT_PUBLISH_DUP = 8u,
    CM_MQTT_PUBLISH_QOS_0 = ((0u << 1) & 0x06),
    CM_MQTT_PUBLISH_QOS_1 = ((1u << 1) & 0x06),
    CM_MQTT_PUBLISH_QOS_2 = ((2u << 1) & 0x06),
    CM_MQTT_PUBLISH_QOS_MASK = ((3u << 1) & 0x06),
    CM_MQTT_PUBLISH_RETAIN = 0x01
} cm_mqtt_publish_flags_e;

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* 测试前请先补充如下参数 */
static char *cmmqtt_glob_subtopic = "world";                        //订阅和发布的主题
static char *cmmqtt_glob_pubmessage_str = "3234";                   //发布的消息

static char cmqtt_glob_cfgserver[129] = "183.230.40.39";            //MQTT测试服务器地址
static int cmqtt_glob_cfgport = 6002;                               //MQTT测试端口号
static char cmqtt_glob_cfgclient_id[129] = "718658665";                //MQTT测试client_id
static char cmqtt_glob_cfguser[129] = "274637";                           //MQTT测试用户名
static char cmqtt_glob_cfgpasswd[129] = "05132";                         //MQTT测试密码

static char cmqtts_glob_cfgserver[129] = "8.137.154.246";           //MQTTS测试服务器地址
static int cmqtts_glob_cfgport = 8883;                              //MQTTS测试端口号
static char cmqtts_glob_cfgclient_id[129] = "QWER09";               //MQTTS测试client_id
static char cmqtts_glob_cfguser[129] = "";                          //MQTTS测试用户名
static char cmqtts_glob_cfgpasswd[129] = "";                        //MQTTS测试密码

static int cmqtt_glob_cfgkeepAlive = 120;                       //保活时间
static int cmqtt_glob_cfgclean = 0;                             //clean模式，是否清除会话，0为不清除，1为清除，若测试onenet平台则必须为1

static cm_mqtt_client_t *_mqtt_client[CM_MQTT_CLIENT_MAX] = {0};

extern linklist_element_t* linklist_next_element(linklist_t* list, linklist_element_t** pos);
/****************************************************************************
 * Private Functions
 ****************************************************************************/

/**
 *  \brief 连接状态回调
 *  
 *  \param [in] client mqtt客户端
 *  \param [in] session session标志
 *  \param [in] conn_res 连接状态
 *  \return 成功返回0，失败返回-1
 *  
 *  \details More details
 */
static int __mqtt_manager_default_connack_cb(cm_mqtt_client_t* client, int session, int conn_res)
{

    int i = 0;
    
    for (; i < CM_MQTT_CLIENT_MAX; i++)
    {
        if (_mqtt_client[i] == client)
        {
            break;
        }
    }
    
    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , CONNECT: %d\r\n", i, conn_res);
           
    return 0;
}

/**
 *  \brief server->client发布消息回调
 *  
 *  \param [in] client mqtt客户端
 *  \param [in] msgid 消息ID
 *  \param [in] topic 主题
 *  \param [in] payload 负载
 *  \param [in] payload_len 负载长度
 *  \param [in] total_len 负载总长度
 *  \return 成功返回0，失败返回-1
 *  
 *  \details 
 */
static int __mqtt_manager_default_publish_cb(cm_mqtt_client_t* client, unsigned short msgid, char* topic, int total_len, int payload_len, char* payload)
{
    int i = 0;
    
    for (; i < CM_MQTT_CLIENT_MAX; i++)
    {
        if (_mqtt_client[i] == client)
        {
            break;
        }
    }

    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , recv: %d,%s,%d,%d\r\n", i, msgid, topic, total_len, payload_len);

    /* 由于测试示例限制打印长度 */
    int printf_size = payload_len > 200 ? 200 : payload_len;
    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , recv: %.*s\r\n", i, printf_size, payload);

    return 0;
}

/**
 *  \brief client->server发布消息ack回调
 *  
 *  \param [in] client mqtt客户端
 *  \param [in] msgid 消息ID
 *  \param [in] dup dup标志
 *  \return 成功返回0，失败返回-1
 *  
 *  \details More details
 */
static int __mqtt_manager_default_puback_cb(cm_mqtt_client_t* client, unsigned short msgid, char dup)
{
    int i = 0;
    
    for (; i < CM_MQTT_CLIENT_MAX; i++)
    {
        if (_mqtt_client[i] == client)
        {
            break;
        }
    }
    
    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , pub_ack: %d,%d\r\n", i, msgid, dup);
    return 0;
}

/**
 *  \brief client->server发布消息recv回调
 *  
 *  \param [in] client mqtt客户端
 *  \param [in] msgid 消息ID
 *  \param [in] dup dup标志
 *  \return 成功返回0，失败返回-1
 *  
 *  \details More details
 */
static int __mqtt_manager_default_pubrec_cb(cm_mqtt_client_t* client, unsigned short msgid, char dup)
{
    int i = 0;
    
    for (; i < CM_MQTT_CLIENT_MAX; i++)
    {
        if (_mqtt_client[i] == client)
        {
            break;
        }
    }
    
    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , pub_rec: %d,%d\r\n", i, msgid, dup);
    return 0;
}

/**
 *  \brief client->server发布消息comp回调
 *  
 *  \param [in] client mqtt客户端
 *  \param [in] msgid 消息ID
 *  \param [in] dup dup标志
 *  \return 成功返回0，失败返回-1
 *  
 *  \details More details
 */
static int __mqtt_manager_default_pubcomp_cb(cm_mqtt_client_t* client, unsigned short msgid, char dup)
{
    int i = 0;
    
    for (; i < CM_MQTT_CLIENT_MAX; i++)
    {
        if (_mqtt_client[i] == client)
        {
            break;
        }
    }
    
    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , pub_comp: %d,%d\r\n", i, msgid, dup);
    return 0;
}

/**
 *  \brief 订阅ack回调
 *  
 *  \param [in] client mqtt客户端
 *  \param [in] msgid 消息ID
 *  \return 成功返回0，失败返回-1
 *  
 *  \details More details
 */
static int __mqtt_manager_default_suback_cb(cm_mqtt_client_t* client, unsigned short msgid, int count, int qos[])
{
    int i = 0;
    
    for (; i < CM_MQTT_CLIENT_MAX; i++)
    {
        if (_mqtt_client[i] == client)
        {
            break;
        }
    }
    
    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , sub_ack: %d\r\n", i, msgid);
    return 0;
}

/**
 *  \brief 取消订阅ack回调
 *  
 *  \param [in] client mqtt客户端
 *  \param [in] msgid 消息ID
 *  \return 成功返回0，失败返回-1
 *  
 *  \details More details
 */
static int __mqtt_manager_default_unsuback_cb(cm_mqtt_client_t* client, unsigned short msgid)
{
    int i = 0;
    
    for (; i < CM_MQTT_CLIENT_MAX; i++)
    {
        if (_mqtt_client[i] == client)
        {
            break;
        }
    }
    
    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , unsub_ack: %d\r\n", i, msgid);
    return 0;
}

/**
 *  \brief ping回调
 *  
 *  \param [in] client mqtt客户端
 *  \param [in] ret 消息状态，0：ping成功，1：ping超时
 *  \return 成功返回0，失败返回-1
 *  
 *  \details More details
 */
static int __mqtt_manager_default_pingresp_cb(cm_mqtt_client_t* client, int ret)
{
    int i = 0;
    
    for (; i < CM_MQTT_CLIENT_MAX; i++)
    {
        if (_mqtt_client[i] == client)
        {
            break;
        }
    }
    
    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , pingrsp: %d\r\n", i, ret);
    return 0;
}

/**
 *  \brief 消息超时回调，包括publish/subscribe/unsubscribe等
 *  
 *  \param [in] client mqtt客户端
 *  \param [in] msgid 消息ID
 *  \return 成功返回0，失败返回-1
 *  
 *  \details More details
 */
static int __mqtt_manager_default_timeout_cb(cm_mqtt_client_t* client, unsigned short msgid)
{
    int i = 0;
    
    for (; i < CM_MQTT_CLIENT_MAX; i++)
    {
        if (_mqtt_client[i] == client)
        {
            break;
        }
    }
    
    cm_demo_printf("\r\n[MQTT]CM MQTT index[%d] , timeout: %d\r\n", i, msgid);
    return 0;
}

static void __cm_get_sub_topic(cm_mqtt_client_t* client)
{
    linklist_t* list = cm_mqtt_client_get_sub_topics(client);//获取topic列表

    if (list == NULL || list->count == 0)
    {
        return;
    }

    char topic_tmp[200] = {0};
    int tmp_len = 0;
    int sub_topic_count = 0;
    linklist_element_t* element = NULL;
    cm_mqtt_topic_t* topic_msg = NULL;

    while ((element = linklist_next_element(list, &element)) != NULL)    //取出元素
    {
        topic_msg = (cm_mqtt_topic_t*)element->content;

        if (topic_msg->state != CM_MQTT_TOPIC_SUBSCRIBED)
        {
            continue;
        }
        
        /* 已订阅 */
        sub_topic_count++;
        memcpy(topic_tmp + tmp_len, topic_msg->topic, topic_msg->topic_len);
        tmp_len += topic_msg->topic_len;
        tmp_len += snprintf(topic_tmp + tmp_len, sizeof(topic_tmp) - tmp_len, ",%d,", topic_msg->qos);
    }

    topic_tmp[tmp_len - 1] = '\0';

    if (sub_topic_count > 0)
    {
        cm_demo_printf("\r\n%s\r\n", topic_tmp);
    }
}

static int __mqtt_client_init(void)
{
    if(_mqtt_client[0])
    {
        cm_demo_printf("\r\n[MQTT]CM MQTT CLIENT IS RUN!!!\r\n");
        return -1;
    }
    
    _mqtt_client[0] = cm_mqtt_client_create();  //client初始化，最大支持6个实例
    
    if (NULL == _mqtt_client[0])
    {
        cm_demo_printf("\r\n[MQTT]CM MQTT CREATE CLIENT ERROR!!!\r\n");
        return -1;
    }
    
    /* 设置回调函数，连接、订阅、发布等接口均为异步接口，结果请根据回调函数返回进行判断 */
    cm_mqtt_client_cb_t callback = {0};
    callback.connack_cb = __mqtt_manager_default_connack_cb;
    callback.publish_cb = __mqtt_manager_default_publish_cb;
    callback.puback_cb = __mqtt_manager_default_puback_cb;
    callback.pubrec_cb = __mqtt_manager_default_pubrec_cb;
    callback.pubcomp_cb = __mqtt_manager_default_pubcomp_cb;
    callback.suback_cb = __mqtt_manager_default_suback_cb;
    callback.unsuback_cb = __mqtt_manager_default_unsuback_cb;
    callback.pingresp_cb = __mqtt_manager_default_pingresp_cb;
    callback.timeout_cb = __mqtt_manager_default_timeout_cb;

    /* 设置client参数 */
    int version = 4;//版本3.1.1
    int pkt_timeout = 10;//发送超时10秒
    int reconn_times = 3;//重连三次
    int reconn_cycle = 20;//重连间隔20秒
    int reconn_mode = 0;//以固定间隔尝试重连
    int retry_times = 3;//重传三次
    int ping_cycle = 60;//ping周期60秒

    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_EVENT, (void*)&callback);
    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_VERSION, (void*)&version);
    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_PKT_TIMEOUT, (void*)&pkt_timeout);
    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_RETRY_TIMES, (void*)&retry_times);
    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_RECONN_MODE, (void*)&reconn_mode);
    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_RECONN_TIMES, (void*)&reconn_times);
    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_RECONN_CYCLE, (void*)&reconn_cycle);
    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_PING_CYCLE, (void*)&ping_cycle);
    return 0;
}

/* 配置SSL参数 */
/* 更详细的SSL通道配置请参考cm_demo_ssl.c中的函数__on_cmd_SSL_CONN，包含证书验证模式和配置证书，请注意该文件中给出的证书格式示例 */
static int __mqtt_ssl_init(void)
{
    int ssl_id = 1;     //SSL通道
    int verify = 0;     //验证类型，0是无证书验证
    int negotime = 60;  //握手超时(s)，60秒
    int ssl_enable = 1; //开启MQTTS
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERIFY, (void *)(uintptr_t)verify); 
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_NEGOTIME, (void *)(uintptr_t)negotime);
    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_SSL_ID, &ssl_id);   //将SSL通道注册到mqtt实例中
    cm_mqtt_client_set_opt(_mqtt_client[0], CM_MQTT_OPT_SSL_ENABLE, &ssl_enable);   //该实例开启MQTTS
    return 0;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/*
 *  MQTT功能调试使用示例
 *  测试命令:mqtt opt
 *  opt取0-8代表操作项，0-8依次代表client初始化、mqtt连接、mqtts连接、订阅、发布、取消订阅、获取当前订阅主题、断开连接和销毁client
 */
void cm_test_mqtt(char len, char** cmd) 
{
    if(len < 2 || cmd == NULL)
    {
        cm_demo_printf("param invalid\r\n");
    }

    if(cmd[1] == NULL)
    {
        cm_demo_printf("param invalid\r\n");
    }
    int opt = atoi((const char *)cmd[1]);
    switch(opt)
    {
        /* 初始化client */
        case 0:
        {
            __mqtt_client_init();
            break;
        }

        /* mqtt连接 */
        case 1:
        {
            /* 配置连接参数，对于字符串参数，内部仅保留指针，不分配空间 */
            cm_mqtt_connect_options_t conn_options = {
                .hostport = (unsigned short)cmqtt_glob_cfgport,
                .hostname = (const char*)cmqtt_glob_cfgserver,
                .clientid = (const char*)cmqtt_glob_cfgclient_id,
                .username = (const char*)cmqtt_glob_cfguser,
                .password = (const char*)cmqtt_glob_cfgpasswd,
                .keepalive = (unsigned short)cmqtt_glob_cfgkeepAlive,
                .will_topic = NULL,
                .will_message = NULL,
                .will_message_len = 0,
                .will_flag = 0, //若要使用遗嘱机制请置1，并补充相关遗嘱信息
                .clean_session = (char)cmqtt_glob_cfgclean,
                };
            
            cm_mqtt_client_connect(_mqtt_client[0], &conn_options);//连接
            break;
        }

        /* mqtts连接 */
        case 2:
        {
            /* SSL初始化 */
            __mqtt_ssl_init();
            
            /* 配置连接参数，对于字符串参数，内部仅保留指针，不分配空间 */
            cm_mqtt_connect_options_t conn_options = {
                .hostport = (unsigned short)cmqtts_glob_cfgport,
                .hostname = (const char*)cmqtts_glob_cfgserver,
                .clientid = (const char*)cmqtts_glob_cfgclient_id,
                .username = (const char*)cmqtts_glob_cfguser,
                .password = (const char*)cmqtts_glob_cfgpasswd,
                .keepalive = (unsigned short)cmqtt_glob_cfgkeepAlive,
                .will_topic = NULL,
                .will_message = NULL,
                .will_message_len = 0,
                .will_flag = 0, //若要使用遗嘱机制请置1，并补充相关遗嘱信息
                .clean_session = (char)cmqtt_glob_cfgclean,
                };
            
            cm_mqtt_client_connect(_mqtt_client[0], &conn_options);//连接
            break;
        }

        /* 订阅 */
        case 3:
        {
            char *topic_tmp[1] = {0};
            topic_tmp[0] = cmmqtt_glob_subtopic;
            char qos_tmp[1] = {0};
            qos_tmp[0] = 0;
            int ret = cm_mqtt_client_subscribe(_mqtt_client[0], (const char**)topic_tmp, qos_tmp, 1);
            
            if (0 > ret)
            {
                cm_demo_printf("\r\n[MQTT]CM MQTT subscribe ERROR!!!, ret = %d\r\n", ret);
            }
            break;
        }

        /* 发布 */
        case 4:

        {
            uint8_t publish_flags = 0;
            publish_flags |= CM_MQTT_PUBLISH_QOS_1;
            char *pubmessage = cmmqtt_glob_pubmessage_str;
            int ret = cm_mqtt_client_publish(_mqtt_client[0], cmmqtt_glob_subtopic, pubmessage, strlen(pubmessage), publish_flags);
            
            if (0 >= ret)
            {
                cm_demo_printf("\r\n[MQTT]CM MQTT publish ERROR!!!, ret = %d\r\n", ret);
            }
            break;
        }

        /* 取消订阅 */
        case 5:
        {
            char *topic_tmp[1] = {0};
            topic_tmp[0] = cmmqtt_glob_subtopic;
            int ret = cm_mqtt_client_unsubscribe(_mqtt_client[0], (const char**)topic_tmp, 1);
            
            if (0 > ret)
            {
                cm_demo_printf("\r\n[MQTT]CM MQTT unsubscribe ERROR!!!, ret = %d\r\n", ret);
            }
            break;
        }

        /* 查看topic */
        case 6:
        {
            __cm_get_sub_topic(_mqtt_client[0]);
            break;
        }

        /* 关闭连接 */
        case 7:
        {
            cm_mqtt_client_disconnect(_mqtt_client[0]);
            break;
        }

        /* 释放client */
        case 8:
        {
            cm_mqtt_client_destroy(_mqtt_client[0]);
            _mqtt_client[0] = NULL;
            break;
        }
    }
}

#include "nr_micro_shell.h"
NR_SHELL_CMD_EXPORT(mqtt, cm_test_mqtt);
#endif



