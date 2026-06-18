/**  @file
  yopen_api_rtc.h

  @brief
  

*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------
16/12/2020        Neo         Init version
=================================================================*/


#ifndef _YOPEN_RTC_H_
#define _YOPEN_RTC_H_

#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include <stdbool.h>
#include "yopen_api_common.h"
#include "yopen_type.h"

#ifdef __cplusplus
	 extern "C" {
#endif

/**
 * @defgroup yopen_rtc rtc功能
 * @{
*/
/*========================================================================
 *  Enumeration Definition
 *========================================================================*/
typedef int yopen_errcode_rtc_e;

/** @brief rtc配置使能枚举*/
typedef enum
{
	YOPEN_RTC_CFG_DISABLE = 0,
	YOPEN_RTC_CFG_ENABLE  = 1,
}yopen_rtc_enable_e;

/*===========================================================================
 * Struct
 ===========================================================================*/
/** @brief rtc结构体*/ 

typedef struct yopen_rtc_time_struct {
	int tm_sec;	 // seconds [0,59]
	int tm_min;	 // minutes [0,59]
	int tm_hour; // hour [0,23]
	int tm_mday; // day of month [1,31]
	int tm_mon;	 // month of year [1,12]
	int tm_year; // year [2000-2100] 
	int tm_zone; // zone [-48 - +48]
}yopen_rtc_time_t;

/** @brief rtc配置结构体*/ 
typedef struct yopen_rtc_cfg_struct {
	yopen_rtc_enable_e	nv_cfg;		//无效参数
	yopen_rtc_enable_e	rtc_cfg;	//无效参数
	yopen_rtc_enable_e	nwt_cfg;	//连接基站后，是否同步基站时间到rtc时间(YOPEN_RTC_CFG_ENABLE:同步,YOPEN_RTC_CFG_DISABLE,默认同步)
}yopen_rtc_cfg_t;

/*===========================================================================
 * function 
 ===========================================================================*/

/**
 * @brief  RTC 回调函数，用于接收alarm通知
 * @param  无
 * @return 无
 */
typedef void (*yopen_rtc_cb)(void);

/**
 * @brief  设置rtc时间
 * @param  tm		 			时间结构体
 * @return yopen_errcode_rtc_e	    0 设置成功 other 错误码  
 */
extern yopen_errcode_rtc_e yopen_rtc_set_time(yopen_rtc_time_t *tm);


/**
 * @brief  获取rtc时间
 * @param  tm		 			时间结构体
 * @return yopen_errcode_rtc_e	    0 获取成功 other 错误码  
 */
extern yopen_errcode_rtc_e yopen_rtc_get_time(yopen_rtc_time_t *tm);


/**
 * @brief  获取rtc时间,以秒为单位
 * @param  无
 * @return INT64	            rtc时间,单位秒
 */
extern time_t yopen_rtc_get_time_s(void);

/**
 * @brief  设置时区，以15分钟为单位
 * @param  timezone		 		设置时区，时区参数范围：[-48，+48]
 * @return yopen_errcode_rtc_e	    0 设置时区成功 other 错误码 
 */
extern yopen_errcode_rtc_e yopen_rtc_set_timezone(int timezone);


/**
 * @brief  获取时区，以15分钟为单位
 * @param  timezone		 		时区值，时区参数范围：[-48，+48]
 * @return yopen_errcode_rtc_e	    0 获取时区成功 other 错误码 
 */
extern yopen_errcode_rtc_e yopen_rtc_get_timezone(int * timezone);


/**
 * @brief  设置rtc alarm时间
 * @param  tm		 			时间结构体
 * @return yopen_errcode_rtc_e	    0 设置alarm时间成功 other 错误码 
 */
extern yopen_errcode_rtc_e yopen_rtc_set_alarm(yopen_rtc_time_t *tm);


/**
 * @brief  获取rtc alarm时间
 * @param  tm		 			时间结构体
 * @return yopen_errcode_rtc_e	    0 获取alarm时间成功 other 错误码 
 */
extern yopen_errcode_rtc_e yopen_rtc_get_alarm(yopen_rtc_time_t *tm);

/**
 * @brief  打开和关闭rtc alarm
 * @param  on_off		 		开关, 取值1和0, 1表示打开, 0表示关闭 
 * @return yopen_errcode_rtc_e	    0 设置alarm时间成功 other 错误码 
 */
extern yopen_errcode_rtc_e yopen_rtc_enable_alarm(unsigned char on_off);


/**
 * @brief  注册rtc alarm 回调函数
 * @param  cb		 			alarm回调函数
 * @return yopen_errcode_rtc_e	    0 注册成功 other 错误码 
 */
extern yopen_errcode_rtc_e yopen_rtc_register_cb(yopen_rtc_cb cb);


/**
 * @brief  将秒转化为rtc时间
 * @param  time_t		 		秒数 
 * @param  yopen_rtc_time_t		rtc时间结构体  
 * @return yopen_errcode_rtc_e	    0 转化成功 other 错误码 
 */
extern yopen_errcode_rtc_e yopen_sec_to_rtc_time(time_t* time_t, yopen_rtc_time_t *rtc_time);


/**
 * @brief  将rtc时间转化为秒
 * @param  time_t		 		秒数 
 * @param  yopen_rtc_time_t		rtc时间结构体  
 * @return yopen_errcode_rtc_e	    0 转化成功 other 错误码 
 */
extern yopen_errcode_rtc_e yopen_rtc_to_sec_time(time_t* time_t, yopen_rtc_time_t *rtc_time);



#ifdef __cplusplus
	 } /*"C" */
#endif
 
#endif /* _YOPEN_RTC_H_ */


