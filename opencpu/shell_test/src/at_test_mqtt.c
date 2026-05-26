#ifdef CM_DEMO_WIFISCAN_SUPPORT

#include <os.h>
#include <stdlib.h>

#include "at_api.h"
#include "at_parser.h"
#include "cm_mqtt.h"
#include "cm_mem.h"
#include "cm_demo_common.h"

typedef AT_CommandItem at_cmd_t;

static char cmmqtt_glob_subtopic[5][50];                         //订阅和发布的主题
static char cmmqtt_glob_pubmessage_str[100];                   //发布的消息
static char cmmqtt_glob_willtopic[30];                         //遗嘱标题
static char cmmqtt_glob_willmessage[100];                   //遗嘱信息
static char cmqtt_glob_cfgclient_id[129] ;                  //MQTT测试client_id
static int cmqtt_glob_cfgkeepAlive;                       //保活时间
static char cmqtt_glob_cfguser[129];                       //MQTT测试用户名
static char cmqtt_glob_cfgpasswd[129];          //MQTT测试密码
static int cmqtt_glob_cfgclean;  



#define FTP_LOG(fmt, ...) osPrintf("[MQTT][%s][%u][%s]:" fmt "\r\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define FTP_CMD_STR_LEN 512

#define CM_FTP_RET_CODE_OK 0 //成功
#define CM_FTP_RET_CODE_MALLOC_FAIL 23 //内存分配失败 23
#define CM_FTP_RET_CODE_PARAM_ERROR 50 //参数错误 50

cm_mqtt_client_t* mqtt_client = NULL;

// AT+MQTT="cm_mqtt_client_create"
int func_cm_mqtt_client_create(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    cm_mqtt_client_t* p_client= cm_mqtt_client_create();

    if (p_client == NULL)
    {
        cm_uart_printf_urc("+MQTTCTEAR: fail");
    }
    else
    {
        mqtt_client = p_client;
    }
    return 0;
}

// AT+MQTT="cm_mqtt_client_destroy"
int func_cm_mqtt_client_destroy(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;

    handle = cm_mqtt_client_destroy(mqtt_client);

    if (handle < 0 )
    {
        cm_uart_printf_urc("+MQTTDESTORY: %d", handle);
    }
    mqtt_client = NULL;

    return 0;
}


// AT+MQTT="cm_mqtt_client_connect"
int func_cm_mqtt_client_connect(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;
    char* hostname=cm_malloc(30);   /*!< 服务器域名或IP*/
    char clientid[15]={0};   /*!< clientid*/
    char username[15]={0};   /*!< 用户名*/
    char password[40]={0};   /*!< 密码*/
    cm_mqtt_connect_options_t option = {0};

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%hu,%s.u,%s.u,%s.u,%s.u,%hhu", 
            &option.hostport,hostname,30,cmqtt_glob_cfgclient_id,15,cmqtt_glob_cfguser,15,cmqtt_glob_cfgpasswd,40,&option.will_flag))
    {
        FTP_LOG("mqtt_client_connect:xs_at_parse_cm1 err");
        cm_free(hostname);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }
    option.hostname = hostname;
    option.clientid = cmqtt_glob_cfgclient_id;
    option.username = cmqtt_glob_cfguser;
    option.password = cmqtt_glob_cfgpasswd;
    if(option.will_flag)
    {
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u,%s.u,%u,%hhu,%hhu,%hhu,%hhu,%hu", cmmqtt_glob_willtopic,30,cmmqtt_glob_willmessage,100,
            &option.will_message_len,&option.will_qos,&option.will_retain,&option.clean_session,&option.conn_flags,&option.keepalive))
        {
            FTP_LOG("mqtt_client_connect:xs_at_parse_cm2 err");
            cm_free(hostname);
            return CM_FTP_RET_CODE_PARAM_ERROR;
        }
    }
    else
    {
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",$,$,$,$,$,%hhu,%hhu,%hu", 
            &option.clean_session,&option.conn_flags,&option.keepalive))
        {
            FTP_LOG("mqtt_client_connect:xs_at_parse_cm3 err");
            cm_free(hostname);
            return CM_FTP_RET_CODE_PARAM_ERROR;
        }
    }
    option.will_topic = cmmqtt_glob_willtopic;
    option.will_message = cmmqtt_glob_willmessage;

    handle = cm_mqtt_client_connect(mqtt_client, &option);

    if (handle < 0)
    {
        cm_uart_printf_urc("+MQTTCONNECT: %d", handle);
    }
    cm_free(hostname);

    return 0;
}

// AT+MQTT="cm_mqtt_client_publish"
int func_cm_mqtt_client_publish(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;
    int payload_len = 0;
    char publish_flags = 0;
    char* topic=cm_malloc(50);
    char* payload=cm_malloc(100);

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u,%s.u,%u,%hhu", 
            topic,50,payload,100,&payload_len,&publish_flags))
    {
        FTP_LOG("func_cm_mqtt_client_publish:xs_at_parse_cm err");
        cm_free(topic);
        cm_free(payload);
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }
    handle = cm_mqtt_client_publish(mqtt_client,topic,payload,
    payload_len > strlen(payload) ? strlen(payload) : payload_len,
    publish_flags);
    if (handle < 0)
    {
        cm_uart_printf_urc("+MQTTPUBLISH: %d", handle);
    }
    cm_free(topic);
    cm_free(payload);
    return 0;
}

// AT+MQTT="cm_mqtt_client_subscribe"
int func_cm_mqtt_client_subscribe(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;
    int count = 0;
    int i,j;
    char qos[5] = {0};
    char *topic_tmp[5] = {0};

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &count))
    {
        FTP_LOG("func_cm_mqtt_client_subscribe:xs_at_parse_cm1 err");
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    for(i=0;i<count;i++)
    {
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u,%hhu",cmmqtt_glob_subtopic[i],50, &qos[i]))
        {

            FTP_LOG("func_cm_mqtt_client_subscribe:xs_at_parse_cm2 err");
            return CM_FTP_RET_CODE_PARAM_ERROR;
        }
        topic_tmp[i] = cmmqtt_glob_subtopic[i];
    }

    handle = cm_mqtt_client_subscribe(mqtt_client,(const char**)topic_tmp,qos,count);

    if (handle < 0)
    {
        cm_uart_printf_urc("+MQTTSUBSCRIBE: %d", handle);
    }

    return 0;
}

// AT+MQTT="cm_mqtt_client_unsubscribe"
int func_cm_mqtt_client_unsubscribe(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;
    int count = 0;
    int i,j;
    char *topic_tmp[5] = {0};

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &count))
    {
        FTP_LOG("func_cm_mqtt_client_unsubscribe:xs_at_parse_cm1 err");
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }

    for(i=0;i<count;i++)
    {
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%s.u",cmmqtt_glob_subtopic[i],50))
        {
            FTP_LOG("func_cm_mqtt_client_unsubscribe:xs_at_parse_cm2 err");
            return CM_FTP_RET_CODE_PARAM_ERROR;
        }
        topic_tmp[i] = cmmqtt_glob_subtopic[i];
    }

    handle = cm_mqtt_client_unsubscribe(mqtt_client,(const char**)topic_tmp,count);

    if (handle < 0)
    {
        cm_uart_printf_urc("+MQTTUNSUBSCRIBE: %d", handle);
    }


    return 0;
}

// AT+MQTT="cm_mqtt_client_disconnect"
int func_cm_mqtt_client_disconnect(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;
    handle = cm_mqtt_client_disconnect(mqtt_client);

    if (handle < 0)
    {
        cm_uart_printf_urc("+MQTTDISCONNECT: fail");
    }
    return 0;
}
static int __mqtt_default_connack_cb(cm_mqtt_client_t* client, int session, cm_mqtt_conn_state_e conn_res)
{
    osPrintf("\r\n[MQTT]CM MQTT: %d\r\n", conn_res);
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
static int __mqtt_default_publish_cb(cm_mqtt_client_t* client, unsigned short msgid, char* topic, int total_len, int payload_len, char* payload)
{
    osPrintf("\r\n[MQTT]CM MQTT recv: %d,%s,%d,%d\r\n", msgid, topic, total_len, payload_len);

    /* 由于测试示例限制打印长度 */
    int printf_size = payload_len > 600 ? 600 : payload_len;
    osPrintf("\r\n[MQTT]CM MQTT recv: %.*s\r\n", printf_size, payload);

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
static int __mqtt_default_puback_cb(cm_mqtt_client_t* client, unsigned short msgid, char dup)
{
    osPrintf("\r\n[MQTT]CM MQTT, pub_ack: %d,%d\r\n", msgid, dup);
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
static int __mqtt_default_pubrec_cb(cm_mqtt_client_t* client, unsigned short msgid, char dup)
{
    osPrintf("\r\n[MQTT]CM MQTT , pub_rec: %d,%d\r\n", msgid, dup);
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
static int __mqtt_default_pubrel_cb(cm_mqtt_client_t* client, unsigned short msgid, char dup)
{
    osPrintf("\r\n[MQTT]CM MQTT, pub_rel: %d,%d\r\n", msgid, dup);
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
static int __mqtt_default_pubcomp_cb(cm_mqtt_client_t* client, unsigned short msgid, char dup)
{
    osPrintf("\r\n[MQTT]CM MQTT , pub_comp: %d,%d\r\n", msgid, dup);
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
static int __mqtt_default_suback_cb(cm_mqtt_client_t* client, unsigned short msgid, int count, int qos[])
{
    int i = 0;
    
    for (; i < count; i++)
    {
        osPrintf("\r\n[MQTT]CM MQTT, sub_ack: qos[%d]=%d\r\n", i, qos[i]);
    }
    
    osPrintf("\r\n[MQTT]CM MQTT, sub_ack: msgid=%d\r\n",msgid);
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
static int __mqtt_default_unsuback_cb(cm_mqtt_client_t* client, unsigned short msgid)
{
    osPrintf("\r\n[MQTT]CM MQTT, unsub_ack: %d\r\n", msgid);
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
static int __mqtt_default_pingresp_cb(cm_mqtt_client_t* client, int ret)
{
    osPrintf("\r\n[MQTT]CM MQTT, pingrsp: %d\r\n", ret);
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
static int __mqtt_default_timeout_cb(cm_mqtt_client_t* client, unsigned short msgid)
{
    osPrintf("\r\n[MQTT]CM MQTT, timeout: %d\r\n", msgid);
    return 0;
}

// AT+MQTT="cm_mqtt_client_set_opt"
int func_cm_mqtt_client_set_opt(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;
    int option = 0;
    int param = 0;

    if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &option))
    {
        FTP_LOG("func_cm_mqtt_client_set_opt:xs_at_parse_cm err");
        return CM_FTP_RET_CODE_PARAM_ERROR;
    }
    if(option < CM_MQTT_OPT_EVENT)
    {
        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p, &lenOfPara, ",%u", &param))
        {
            FTP_LOG("func_cm_mqtt_client_set_opt:xs_at_parse_cm1 err");
            return CM_FTP_RET_CODE_PARAM_ERROR;
        }
        handle = cm_mqtt_client_set_opt(mqtt_client,option,&param);
    }
    else
    {
        switch(option)
        {
            case CM_MQTT_OPT_EVENT:
                cm_mqtt_client_cb_t callback = {0};
                callback.connack_cb = __mqtt_default_connack_cb;
                callback.publish_cb = __mqtt_default_publish_cb;
                callback.puback_cb = __mqtt_default_puback_cb;
                callback.pubrec_cb = __mqtt_default_pubrec_cb;
                callback.pubrel_cb = __mqtt_default_pubrel_cb;
                callback.pubcomp_cb = __mqtt_default_pubcomp_cb;
                callback.suback_cb = __mqtt_default_suback_cb;
                callback.unsuback_cb = __mqtt_default_unsuback_cb;
                callback.pingresp_cb = __mqtt_default_pingresp_cb;
                callback.timeout_cb = __mqtt_default_timeout_cb;
                handle = cm_mqtt_client_set_opt(mqtt_client,option,(void*)&callback);
                break;
            case CM_MQTT_OPT_EVENT_CONNACK_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_connack_cb);
                break;
            case CM_MQTT_OPT_EVENT_PUBLISH_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_publish_cb);
                break;
            case CM_MQTT_OPT_EVENT_PUBACK_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_puback_cb);
                break;
            case CM_MQTT_OPT_EVENT_PUBREC_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_pubrec_cb);
                break;
            case CM_MQTT_OPT_EVENT_PUBREL_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_pubrel_cb);
                break;
            case CM_MQTT_OPT_EVENT_PUBCOMP_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_pubcomp_cb);
                break;
            case CM_MQTT_OPT_EVENT_SUBACK_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_suback_cb);
                break;
            case CM_MQTT_OPT_EVENT_UNSUBACK_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_unsuback_cb);
                break;
            case CM_MQTT_OPT_EVENT_PINGRESP_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_pingresp_cb);
                break;
            case CM_MQTT_OPT_EVENT_TIMEOUT_CB:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_timeout_cb);
                break;
            default:
                handle = cm_mqtt_client_set_opt(mqtt_client,option,__mqtt_default_timeout_cb);
                break;
        }
        
    }

    if (handle < 0)
    {
        cm_uart_printf_urc("+MQTTSETOPT: %d", handle);
    }
    return 0;
}

// AT+MQTT="cm_mqtt_client_get_msgid"
int func_cm_mqtt_client_get_msgid(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;
    handle = cm_mqtt_client_get_msgid(mqtt_client);

    cm_uart_printf_urc("+MQTTGETMSGID: %d", handle);
    return 0;
}

// AT+MQTT="cm_mqtt_client_get_state"
int func_cm_mqtt_client_get_state(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int handle = -1;
    handle = cm_mqtt_client_get_state(mqtt_client);

    cm_uart_printf_urc("+MQTTGETSTATE: %d", handle);
    return 0;
}

// AT+MQTT="cm_mqtt_client_get_sub_topics"
int func_cm_mqtt_client_get_sub_topics(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    linklist_t* linkList = NULL;
    linkList = cm_mqtt_client_get_sub_topics(mqtt_client);

    if(linkList == NULL)
        cm_uart_printf_urc("+MQTTGETSUBTOPICS: fail");
    else
        cm_uart_printf_urc("+MQTTGETSUBTOPICS: count=%d",linkList->count);
    return 0;
}

// AT+MQTT="XX", param1, param2, ...
// "XX" 为函数名
void mqtt_oc_test_entry(os_uint8_t id, os_uint8_t *para_p, os_uint16_t lenOfPara)
{
    int ret = 0;
    os_uint8_t *para_p_new = para_p;
    os_uint16_t lenOfPara_new = lenOfPara;

    FTP_LOG("id[%d] para_p[%s] lenOfPara[%d]", id, para_p, lenOfPara);

    do
    {
        para_p_new = para_p;
        lenOfPara_new = lenOfPara;

        char func_name[30] = {0};

        if (AT_ERRNO_NO_ERROR != xs_at_parse_cm(&para_p_new, &lenOfPara_new, "%s.u", func_name, 30))
        {
            ret = CM_FTP_RET_CODE_PARAM_ERROR;
            break;
        }

        if (strcmp(func_name, "cm_mqtt_client_create") == 0)
        {
            ret = func_cm_mqtt_client_create(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_destroy") == 0)
        {
            ret = func_cm_mqtt_client_destroy(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_connect") == 0)
        {
            ret = func_cm_mqtt_client_connect(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_publish") == 0)
        {
            ret = func_cm_mqtt_client_publish(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_subscribe") == 0)
        {
            ret = func_cm_mqtt_client_subscribe(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_unsubscribe") == 0)
        {
            ret = func_cm_mqtt_client_unsubscribe(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_disconnect") == 0)
        {
            ret = func_cm_mqtt_client_disconnect(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_set_opt") == 0)
        {
            ret = func_cm_mqtt_client_set_opt(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_get_msgid") == 0)
        {
            ret = func_cm_mqtt_client_get_msgid(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_get_state") == 0)
        {
            ret = func_cm_mqtt_client_get_state(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else if (strcmp(func_name, "cm_mqtt_client_get_sub_topics") == 0)
        {
            ret = func_cm_mqtt_client_get_sub_topics(id, (os_uint8_t *)para_p_new, lenOfPara_new);
        }
        else
        {
            FTP_LOG("Error func name[%s]", func_name);
            ret = CM_FTP_RET_CODE_PARAM_ERROR;
        }
    } while(0);

    cm_uart_printf_result(id, ret);
}


AT_CMD_TABLE_SECTION_USER at_cmd_t g_at_tables_mqtt_oc[] =
{
    {.name = "+MQTT",   .setFunc = mqtt_oc_test_entry,   .readFunc = NULL,   .testFunc = NULL,      .execFunc = NULL},
};

#endif


