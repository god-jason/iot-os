#if 0
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
#ifndef __AT_MQTT_H__
#define __AT_MQTT_H__

#include "stddef.h"
#include "stdint.h"
#include "app_mqtt.h"
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
int APP_MqttInit(void);
void AT_MqttCfgSet(uint8_t channelId, uint8_t *para_p, uint16_t lenOfPara);
void AT_MqttCfgTest(uint8_t channelId);
void AT_MqtConnSet(uint8_t channelId, uint8_t *para_p, uint16_t lenOfPara);
void AT_MqttConnTest(uint8_t channelId);
void AT_MqttSubSet(uint8_t channelId, uint8_t *para_p, uint16_t lenOfPara);
void AT_MqttUnsubSet(uint8_t channelId, uint8_t *para_p, uint16_t lenOfPara);
void AT_MqttPubSet(uint8_t channelId, uint8_t *para_p, uint16_t lenOfPara);
void AT_MqttReadSet(uint8_t channelId, uint8_t *para_p, uint16_t lenOfPara);
void AT_MqttSateSet(uint8_t channelId, uint8_t *para_p, uint16_t lenOfPara);
void AT_MqttDiscSet(uint8_t channelId, uint8_t *para_p, uint16_t lenOfPara);
int8_t AT_MqttSendUnsolicited(MqttUrcType type, uint8_t channelId, void *urcInfo);
#endif
#endif
