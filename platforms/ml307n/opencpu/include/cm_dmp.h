/**
 *  @file    cm_dmp.h
 *  @brief   中国移动DMP入库功能接口
 *  @copyright copyright ? 2021 China Mobile IOT. All rights reserved.
 *  @author by ZXW
 *  @date 2021/8/17
 */


#ifndef __CM_DMP_H__
#define __CM_DMP_H__


#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define DM40_SUPPORT

/** DMP参数配置选项*/
typedef enum{
    CM_MOBILE_DM_INTERVAL,    /*!< 整型,更新周期。范围：1-1440，默认值为 1440；单位：minute */
    CM_MOBILE_DM_APPKEY,      /*!< 字符串,应用密钥 */
    CM_MOBILE_DM_PASSWORD,    /*!< 字符串,私钥 */
    CM_MOBILE_DM_TMLTYPE,     /*!< 整型,终端类型。默认值0--接口,1--linux   ,2--andriod */
    CM_MOBILE_DM_HOST,        /*!< 字符串，主机地址。默认地址为“m.fxltsbl.com” */
    CM_MOBILE_DM_PORT,        /*!< 整型端口。默认端口为5683 */
    CM_MOBILE_DM_APPINFO,     /*!< 字符串,终端应用程序信息，最大字符串长度为 255 字节 */
    CM_MOBILE_DM_MAC,         /*!< 字符串,终端MAC地址 */
    CM_MOBILE_DM_ROM,         /*!< 字符串,终端ROM信息*/
    CM_MOBILE_DM_RAM,         /*!< 字符串,终端RAM信息*/
    CM_MOBILE_DM_CPU,         /*!< 字符串,终端CPU信息*/
    CM_MOBILE_DM_OSVER,       /*!< 字符串,终端操作系统信息*/
    CM_MOBILE_DM_SWVER,       /*!< 字符串,终端固件版本*/
    CM_MOBILE_DM_SWNAME,      /*!< 字符串,终端固件名称*/
    CM_MOBILE_DM_VOLTE,       /*!< 字符串,终端VOLTE信息*/
    CM_MOBILE_DM_NETTYPE,     /*!< 字符串,终端网络类型*/
    CM_MOBILE_DM_ACCOUNT,     /*!< 字符串,终端宽带信息*/
    CM_MOBILE_DM_PHONENUM,    /*!< 字符串,终端电话号码*/
    CM_MOBILE_DM_LOCATION,    /*!< 字符串,终端位置信息*/
#ifdef DM40_SUPPORT
    CM_MOBILE_DM_BRAND,       /*!< 字符串,品牌*/
    CM_MOBILE_DM_MODEL,       /*!< 字符串,型号*/
    CM_MOBILE_DM_TMPLID,      /*!< 字符串,模板ID*/
    CM_MOBILE_DM_BATCAP,      /*!< 字符串,电池容量*/
    CM_MOBILE_DM_SRCNSZ,      /*!< 字符串,屏幕尺寸*/
    CM_MOBILE_DM_RTMAC,       /*!< 字符串,路由MAC信息*/
    CM_MOBILE_DM_BTMAC,       /*!< 字符串,蓝牙MAC信息*/
    CM_MOBILE_DM_GPU,         /*!< 字符串,GPU信息*/
    CM_MOBILE_DM_BOARD,       /*!< 字符串,主板信息*/
    CM_MOBILE_DM_RES,         /*!< 字符串,分辨率*/
	CM_MOBILE_DM_WEARING,     /*!< 整型,穿戴状态*/
    CM_MOBILE_DM_IMSI,        /*!< 字符串,IMSI*/
    CM_MOBILE_DM_IMSI2,       /*!< 字符串,IMSI2*/
    CM_MOBILE_DM_SN,          /*!< 字符串,SN*/
	CM_MOBILE_DM_BATCAPCURR,  /*!< 整型,电池电量状态*/
	CM_MOBILE_DM_NETSTS,      /*!< 整型,网络状态*/
	CM_MOBILE_DM_ENABLE,      /*!< 整型,DM版本使能*/
#endif
    CM_MOBILE_DM_ALL,
} cm_mobile_dm_option_e;

/**
 * @brief 启动中移DMP功能
 *
 * @return
 *  = 0  - 成功 \n
 *  < 0  - 失败
 *
 * @details
 */
int32_t cm_mobile_dm_start(void);

/**
 * @brief 关闭中移DMP功能
 *
 * @return
 *  = 0  - 成功 \n
 *  < 0  - 失败
 *
 * @details
 */
int32_t cm_mobile_dm_stop(void);

/**
 * @brief 获取DMP状态
 *
 * @return
 *  >= 0  - 运行状态 \n
 *  < 0  - 失败
 *
 * @details
 */
int32_t cm_mobile_dm_get_state(void);

/**
 * @brief 参数设置
 *
 * @param [in] type 配置参数类型
 * @param [in] param 配置参数
 * @param [in] len 参数长度
 *
 * @return
 *  = 0  - 成功 \n
 *  < 0  - 失败
 *
 * @details
 */
int32_t cm_mobile_dm_set_option(cm_mobile_dm_option_e type, const void* param, int len);

/**
 * @brief 参数读取
 *
 * @param [in] type 配置参数类型
 * @param [out] param 读取参数缓存
 * @param [in] len 缓存长度
 *
 * @return 成功返回参数长度，失败返回-1
 *
 * @details
 */
int32_t cm_mobile_dm_get_option(cm_mobile_dm_option_e type, void* param, int32_t len);

#endif

