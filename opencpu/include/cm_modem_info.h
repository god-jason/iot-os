/**
 * @file        cm_modem_info.h
 * @brief       Modem_info接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By ZH
 * @date        2022/11/18
 *
 * @defgroup modem modem
 * @ingroup DS
 * @{
 */
#ifndef __CM_MODEM_INFO_H__
#define __CM_MODEM_INFO_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include<stdbool.h>

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/** 无线信息 */
typedef struct 
{
    uint8_t  rat;            /**无线接入模式 暂不支持*/
    uint16_t rsrp;           /**信号接收功率 实际数值需进行转换 rsrp = 10 * (rsrp - 140)*/
    uint16_t rsrq;           /**信号接收质量 实际数值需进行转换 rsrq = ((5 * rsrq - 195) > -30) ? -30 : (5 * rsrq - 195)*/
    uint16_t rssi;           /**信号强度指示 实际数值需进行转换 rssi = 10 *（rssi - 111）*/
    uint16_t rxlev;          /**信号接收电平 暂不支持*/
    uint16_t tx_power;       /**最近一次发射功率  暂不支持*/
    uint32_t tx_time;        /**上行累计发送时长 暂不支持*/
    uint32_t rx_time;        /**下行累计接收时长 暂不支持*/
    uint32_t last_cellid;    /**上一次SB1服务小区ID*/
    uint8_t  last_ecl;       /**上一次ECL值*/
    uint16_t last_snr;       /**上一次信噪比 单位1dBM 范围-23~40*/
    uint32_t last_earfcn;    /**上一次绝对射频频道编号*/
    uint16_t last_pci;       /**上一次小区物理ID*/
} cm_radio_info_t;


/** 小区信息 */
typedef struct 
{
    bool primary_cell;       /**是否为当前驻留小区 */
    uint8_t  mcc[8];         /**移动国家代码 实际填写前3个字节*/
    uint8_t  mnc[8];         /**移动网络代码 实际填写前3个字节*/  
    uint32_t earfcn;         /**绝对射频频道编号*/
    uint8_t  earfcn_offset;  /**绝对射频频偏 暂不支持*/
    uint16_t tac;            /**类型分配码 */   
    uint16_t pci;            /**小区物理ID */
    uint16_t rsrp;           /**信号接收功率 实际数值需进行转换 rsrp = 10 * (rsrp - 140)*/
    uint16_t rsrq;           /**信号接收质量 实际数值需进行转换 rsrq = ((5 * rsrq - 195) > -30) ? -30 : (5 * rsrq - 195)*/
    uint16_t rssi;           /**信号强度指示 实际数值需进行转换 rssi = 10 *（rssi - 111）*/
    uint16_t snr;            /**信噪比 单位1dBM 范围-60~40。*/
    uint32_t bandwidth;      /**带宽  下行带宽 暂不支持 */
    uint32_t cid;           /**小区识别码 16进制表示*/ 
} cm_cell_info_t;
/**
 * @brief 获取模组小区信息
 * 
 * @param [out] cell_info[] 小区信息数组
 * @param [in] cell_info_num 欲获取小区信息个数（最大支持16个）
 * 
 * @return 
 *   >= 0  - 实际获取到的小区信息个数 \n
 *   < 0  - 失败, 返回值为错误码
 *  
 * @details 只支持当前小区上报
 */
int32_t cm_modem_info_cell(cm_cell_info_t cell_info[], uint16_t cell_info_num);

/**
 * @brief 获取模组无线信息
 * 
 * @param [out] radio_info 无线信息
 * 
 * @return 
 *   = 0  - 成功 \n
 *   < 0  - 失败, 返回值为错误码
 *  
 * @details 该函数无法获取rat，需通过虚拟AT+COPS？获取
 */
int32_t cm_modem_info_radio(cm_radio_info_t *radio_info);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_MODEM_H__ */

/** @}*/