/**
 * @file        cm_wifiscan.h
 * @brief       wifiscan 接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By SMR
 * @date        2021/4/12
 *
 * @defgroup wifiscan
 * @ingroup wifiscan
 * @{
 */

#ifndef __CM_WIFISCAN_H__
#define __CM_WIFISCAN_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
 
#define MAX_BSSID_NUM_TOTAL         10	/*!< 扫描信息单元最大数量，请勿修改 */
#define SIZE_MAC_ADDRESS            6	/*!< MAC地址缓存区长度，请勿修改 */

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** 优先级 */
typedef enum 
{
    CM_WIFI_SCAN_LTE_HIGH =0,       /*!< 优先级设置，LTE优先 */
    CM_WIFI_SCAN_WIFI_HIGH,         /*!< 优先级设置，WIFI优先 */
}cm_wifi_scan_priority_e;

/** 扫描到的单元数据 */
typedef struct
{
  uint8_t  bssid[SIZE_MAC_ADDRESS];  /*!< Basic Service SetIDentifier，一般是AP MAC地址 */
  uint8_t  padding[2];               /*!< 预留，未使用 */
  int32_t  rssi;                     /*!< 信号值 单位：dbm */
  uint32_t channel_number;           /*!< 通道号 */
}cm_wifi_scan_cell_info_t;

/** 扫描到的全部数据 */
typedef struct
{
    uint8_t                  bssid_number;								/*!< 扫描到的单元数量 */
    cm_wifi_scan_cell_info_t channel_cell_list[MAX_BSSID_NUM_TOTAL];	/*!< 单元数据 */
}cm_wifi_scan_info_t;

/** 参数配置类型 */
typedef enum
{
    CM_WIFI_SCAN_CFG_ROUND,         /*!< 扫描轮次，不支持 */
    CM_WIFI_SCAN_CFG_MAX_COUNT,     /*!< 期望上报数量，范围：1~10，默认值5 */
    CM_WIFI_SCAN_CFG_TIMEOUT,       /*!< 总的扫描超时时间，单位：s，范围：2~255，默认值2 */
    CM_WIFI_SCAN_CFG_PRIORITY,      /*!< LTE、WIFI优先级，不支持，默认值CM_WIFI_SCAN_LTE_HIGH */
} cm_wifi_scan_cfg_type_e;

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
 * @brief 扫描回调函数
 *
 * @param [out] param        扫描结果
 * @param [out] user_param   用户参数
 *
 * @details  调用cm_wifiscan_start时传入
 */
typedef void (*cm_wifiscan_callback_t)(cm_wifi_scan_info_t *param, void *user_param);

/**
 * @brief 设置扫描参数
 *
 * @param [in] type  设置参数类型
 * @param [in] value 设置参数数值
 *
 * @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 * @details 
 */
int32_t cm_wifiscan_cfg(cm_wifi_scan_cfg_type_e type, void *value);

/**
 *  @brief 开始扫描
 *  
 * @param [in] cb           回调函数
 * @param [in] user_param   用户参数
 *
 *  @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *  
 *  @details 开启wifiscan功能，开启后结果将通过回调函数异步上报。\n
 *           切换CFUN过程中禁止扫描WiFi。
 */
int32_t cm_wifiscan_start(cm_wifiscan_callback_t cb, void *user_param);

/**
 *  @brief 停止扫描
 *  
 *  @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *  
 *  @details 用于超时时间未到或结果未完全返回时，可中断wifiscan功能。
 */
int32_t cm_wifiscan_stop(void);

/**
 * @brief 查询最近一次扫描结果
 *
 * @param [out] param  扫描结果
 *
 * @return
 *   =  0 - 成功 \n
 *   = -1 - 失败
 *
 * @details 
 */
int32_t cm_wifiscan_query(cm_wifi_scan_info_t **param);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif

/** @}*/

