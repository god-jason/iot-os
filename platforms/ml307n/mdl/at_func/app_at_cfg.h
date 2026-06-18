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
 * 2023-08-01     ict team          创建
 ************************************************************************************
 */

#ifndef __APP_AT_CFG_H__
#define __APP_AT_CFG_H__


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <nvm.h>

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
/* AT命令名称定义 Begin */
#define AT_X_CFG        "+QCFG"
/* AT命令名称定义 End */
/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum {
    APP_AT_CFG_URC_RI_TYPE_OFF   = 0,
    APP_AT_CFG_URC_RI_TYPE_PULSE = 1
} APP_AT_CFG_URC_RI_TYPE;

typedef enum {
    APP_AT_URC_RI_CFGTYPE_RING = 0, // 来电主动上报类
    APP_AT_URC_RI_CFGTYPE_SMS,      // 来短信主动上报类
    APP_AT_URC_RI_CFGTYPE_OTHER,    // 其他主动上报类
    APP_AT_URC_RI_CFGTYPE_MAX  = RI_PULSE_CFG_TYPE_MAX,
    APP_AT_URC_RI_CFGTYPE_NONE,     // 未知或不支持的类
} APP_AT_URC_RI_CFGTYPE;

typedef enum {
    APP_AT_CFG_RI_SIGNALTYPE_RESPECTIVE = 0, // respective
    APP_AT_CFG_RI_SIGNALTYPE_PHYSICAL,      // physical
} APP_AT_URC_RI_SIGNALTYPE;
/************************************************************************************
 *                                 函数声明
 ************************************************************************************/
void APP_AT_X_CFG_Set(uint8_t channelId, uint8_t *paramIn, uint16_t paramLen);
void APP_AT_X_CFG_Test(uint8_t channelId);

/**
 ************************************************************************************
 * @brief           获取URC RI的脉冲配置
 *
 * @param[in]       urcType URC类型，参考APP_AT_URC_RI_CFGTYPE
 * @param[out]       cfg urcType类型对应的配置
 *
 * @return   osOK : 成功   其他 : 失败
 ************************************************************************************
*/
osStatus_t APP_URC_RI_GetPluseCfg(uint8_t urcType, NV_AT_RiPulseCfg *cfg);

/**
 ************************************************************************************
 * @brief           设置URC RI的脉冲配置
 *
 * @param[in]       urcType URC类型，参考APP_AT_URC_RI_CFGTYPE
 * @param[out]      cfg urcType类型对应的配置
 *
 * @return   osOK : 成功   其他 : 失败
 ************************************************************************************
*/
osStatus_t APP_URC_RI_SetPluseCfg(uint8_t urcType, NV_AT_RiPulseCfg cfg);

/**
 ************************************************************************************
 * @brief           获取urcDelay配置值
 *
 * @param[in]       none
 *
 * @return   URC Delay的值，单位：毫秒
 ************************************************************************************
*/
uint32_t APP_URC_RI_GetURCDelay(void);

/**
 ************************************************************************************
 * @brief           设置URC Delay
 *
 * @param[in]       urcDelay URC Delay的值，单位：毫秒
 *
 * @return   osOK : 成功   其他 : 失败
 ************************************************************************************
*/
osStatus_t APP_URC_RI_SetURCDelayMS(uint32_t urcDelayMS);

/**
 ************************************************************************************
 * @brief           设置URC Delay
 *
 * @param[in]       urcDelay URC Delay的值，单位：秒
 *
 * @return   osOK : 成功   其他 : 失败
 ************************************************************************************
*/
osStatus_t APP_URC_RI_SetURCDelayS(uint8_t urcDelayS);

/**
 ************************************************************************************
 * @brief           获取ri signal type配置值
 *
 * @param[in]       none
 *
 * @return  ri signal type的值， 参考  APP_AT_URC_RI_SIGNALTYPE
 ************************************************************************************
*/
uint8_t APP_URC_RI_GetSignalType(void);
/**
 ************************************************************************************
 * @brief           设置ri signal type
 *
 * @param[in]       signaltype 参考  APP_AT_URC_RI_SIGNALTYPE
 *
 * @return   none
 ************************************************************************************
*/
void APP_URC_RI_SetSignalType(uint8_t signaltype);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __APP_AT_CFG_H__ */
