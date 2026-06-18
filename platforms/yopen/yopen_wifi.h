
/**  @file
  ycom_ril.h

  @brief
  TODO

*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/
                
#ifndef _YOPEN_WIFI_H
#define _YOPEN_WIFI_H
/*> include header files here*/

#include <stdio.h>
#include "yopen_api_common.h"
#include "yopen_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/*========================================================================
*  Enumeration Definition
*========================================================================*/

#define YOPEN_WIFI_CHANNEL_MAX_NUM (14)

/****************************  error code about ADC    ***************************/
typedef enum
{
	YOPEN_WIFI_SUCCESS = 0,
    YOPEN_WIFI_NOT_SUPORRT = 10 | (YOPEN_COMPONENT_WIFI << 16),
    YOPEN_WIFI_ERROR    = 11  | (YOPEN_COMPONENT_WIFI << 16),
} yopen_errcode_wifi_e;

/*===========================================================================
 * Struct
 ===========================================================================*/

typedef struct 
{
    INT32   maxTimeOut;         //ms, AT max execution time
    UINT8   round;              //wifiscan total round
    UINT8   maxBssidNum;        //wifiscan max report num
    UINT8   scanTimeOut;        //s, max time of each round executed by RRC
    UINT8   wifiPriority;       //CmiWifiScanPriority 0：数据优先 1：WIFI优先
    UINT8   channelCount;       //channel count; if count is 1 and all channelId are 0, UE will scan all frequecny channel
    UINT8   rsvd[3];
    UINT16  channelRecLen;      //ms, max scantime of each channel  100-280ms
    UINT8   channelId[YOPEN_WIFI_CHANNEL_MAX_NUM];          //channel id 1-14: scan a specific channel
    BOOL   rptMode;  //false CMI_DEV_WIFISCAN_SYN_RPT true： CMI_DEV_WIFISCAN_ASYN_RPT
    BOOL    specificBssidPresent; //whether search a specific WIFI BSSID
    UINT8   specificBssid[6];  //only valid when specificBssidPresent is TRUE
}yopen_wifi_config_s;

#define YOPEN_WIFI_BSSID_MAX_NUM      40
#define YOPEN_WIFI_SSID_HEX_MAX_LENGTH     32
typedef struct
{
    UINT8   bssidNum;                                   // valid bssid number reported
    UINT8   rsvd;
    UINT8   ssidHexLen[YOPEN_WIFI_BSSID_MAX_NUM];        //the length of Wifi SSID Name hex data
    UINT8   ssidHex[YOPEN_WIFI_BSSID_MAX_NUM][YOPEN_WIFI_SSID_HEX_MAX_LENGTH]; //the hex data of WiFi SSID Name
    INT8    rssi[YOPEN_WIFI_BSSID_MAX_NUM];           // rssi value of scanned bssid
    UINT8   channel[YOPEN_WIFI_BSSID_MAX_NUM];        // record channel index of bssid, 2412MHz ~ 2472MHz correspoding to 1 ~ 13
    UINT8   bssid[YOPEN_WIFI_BSSID_MAX_NUM][6];       // mac address is fixed to 6 digits
} yopen_wifi_info_s;

typedef void (*wifiscan_callback_ptr)(void *ctx, yopen_wifi_info_s *info); 
/*===========================================================================
 * FUNCTION
 ===========================================================================*/


/**
 * @brief 初始化I2C主机
 * @param wifi_config   wifi scan的配置参数
 * @param wifi_info     wifi scan返回的信息
 * @return ** yopen_errcode_wifi_e 
 */
extern yopen_errcode_wifi_e yopen_wifi_scan(yopen_wifi_config_s wifi_config, yopen_wifi_info_s *wifi_info);


/**
 * @brief 异步扫描wifi
 * @param wifi_config   wifi scan的配置参数
 * @param cb            wifi scan返回的回调函数
 * @param ctx           wifi scan返回的回调函数的上下文
 * @return ** yopen_errcode_wifi_e 
 */
yopen_errcode_wifi_e yopen_wifi_scan_async(yopen_wifi_config_s wifi_config, wifiscan_callback_ptr cb, void *ctx);
#ifdef __cplusplus
} /*"C" */
#endif

#endif




