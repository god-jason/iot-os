/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file    app_at_mqtt.h
 *
 * @brief
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-08-28     ict team          创建
 ************************************************************************************
 */
#ifndef __AT_MQTT_QI_H__
#define __AT_MQTT_QI_H__

#include "stddef.h"
#include "stdint.h"

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define         MQTT_QI_MAX_TOPIC_NUM              3

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
#if 0
typedef enum
{
    MQTT_QI_UNKNOW_ERR = 600,
    MQTT_QI_UNVALID_PARA_ERR,
    MQTT_QI_CONNECT_FAILED_ERR,
    MQTT_QI_CONNECTING,
    MQTT_QI_CONNECTED,
    MQTT_QI_NET_ERR,
    MQTT_QI_STORAGE_ERR,
    MQTT_QI_STATE_ERR,
    MQTT_QI_DNS_ERR
}MqttQiErrCode;

typedef enum
{
    MQTT_QI_CONN,
    MQTT_QI_PUBNMI,
    MQTT_QI_DROP,
    MQTT_QI_PUBLISH,
    MQTT_QI_PINGRESP,
    MQTT_QI_TIMEOUT,
    MQTT_QI_SUBACK,
    MQTT_QI_UNSUBACK,
    MQTT_QI_PUBACK,
    MQTT_QI_PUBREC,
    MQTT_QI_PUBCOMP
}MqttQiUrcType;

typedef enum
{
    MQTT_QI_CONN_STATE_SUCCEED,
    MQTT_QI_CONN_STATE_ONGOING,
    MQTT_QI_CONN_STATE_CLIENT_DSICONNECTED,
    MQTT_QI_CONN_STATE_SERVER_REFUSED,
    MQTT_QI_CONN_STATE_SERVER_DISCONNECTED,
    MQTT_QI_CONN_STATE_PING_TIMEOUT,
    MQTT_QI_CONN_STATE_NET_ERR,
    MQTT_QI_CONN_STATE_UNKNOW_ERR = 255
}MqttQiConnState;

typedef enum
{
    MQTT_QI_PING_RET_SUCCEED,
    MQTT_QI_PING_RET_TIMEOUT
}MqttQiPingRet;

typedef enum
{
    SUB_SUCCEED_QOS0,
    SUB_SUCCEED_QOS1,
    SUB_SUCCEED_QOS2,
    SUB_FAILED = 128,
}MqttQiCode;

typedef enum
{
    MQTT_QI_DUP_NORAML_DATA,
    MQTT_QI_DUP_RETRY_DATA
}MqttQiDup;

typedef enum
{
    ASCII_STR = 0,
    HEX_STR,
    CONVERT_STR
}MqttQiMsgFormat;

typedef struct
{
    uint8_t connect_id;
    MqttConnState state;
}MqttQiConnUrcInfo;

typedef struct
{
    uint8_t connect_id;
    uint16_t mid;
    uint32_t data_len;
}MqttQiPubNmiUrcInfo;

typedef struct
{
    uint8_t connect_id;
    uint32_t dropped_length;
}MqttQiDropUrcInfo;

typedef struct
{
    uint8_t connect_id;
    uint16_t mid;
    uint8_t *topic;
    uint32_t total_len;
    uint32_t payload_len;
    uint8_t *payload;
}MqttQiPublishUrcInfo;

typedef struct
{
    uint8_t connect_id;
    MqttPingRet ping_ret;
}MqttQiPingrespUrcInfo;

typedef struct
{
    uint8_t connect_id;
    uint16_t mid;
}MqttTimeoutUrcInfo;

typedef struct
{
    uint8_t connect_id;
    uint16_t mid;
    uint8_t code_number;
    MqttCode code[MQTT_QI_MAX_TOPIC_NUM];
}MqttQiSubackUrcInfo;

typedef struct
{
    uint8_t connect_id;
    uint16_t mid;
}MqttQiUnsubackUrcInfo;

typedef struct
{
    uint8_t connect_id;
    uint16_t mid;
    MqttDup dup;
}MqttQiPubackUrcInfo;

typedef struct
{
    uint8_t connect_id;
    uint16_t mid;
    MqttDup dup;
}MqttQiPubrecUrcInfo;

typedef struct
{
    uint8_t connect_id;
    uint16_t mid;
    MqttDup dup;
}MqttQiPubcompUrcInfo;
#endif
/************************************************************************************
 *                                 函数声明
 ************************************************************************************/

int APP_MqttQiInit(void);

void AT_MqttQiCfgSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MqttQiCfgTest(uint8_t channelId);

void AT_MqttQiOpenSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MqttQiOpenRead(uint8_t channelId);
void AT_MqttQiOpenTest(uint8_t channelId);

void AT_MqttQiCloseSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MqttQiCloseTest(uint8_t channelId);


void AT_MqttQiConnSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MqttQiConnRead(uint8_t channelId);
void AT_MqttQiConnTest(uint8_t channelId);

void AT_MqttQiDiscSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MqttQiDiscTest(uint8_t channelId);

void AT_MqttQiSubSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MqttQiSubTest(uint8_t channelId);

void AT_MqttQiUnsSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MqttQiUnsTest(uint8_t channelId);

// void AT_MqttQiPubSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
// void AT_MqttQiPubTest(uint8_t channelId);

void AT_MqttQiPubexSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MqttQiPubexTest(uint8_t channelId);

void AT_MqttQiRecvSet(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void AT_MqttQiRecvRead(uint8_t channelId);
void AT_MqttQiRecvTest(uint8_t channelId);

#endif
