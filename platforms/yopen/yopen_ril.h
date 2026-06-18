
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
                
#ifndef _YOPEN_RIL_H
#define _YOPEN_RIL_H
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

/****************************  error code about ADC    ***************************/
typedef enum
{
	YOPEN_RIL_SUCCESS = 0,
    YOPEN_RIL_NOT_SUPORRT = 10 | (YOPEN_COMPONENT_RIL << 16),
    YOPEN_RIL_ERROR    = 11  | (YOPEN_COMPONENT_RIL << 16),
} yopen_errcode_ril_e;



/*===========================================================================
 * Struct
 ===========================================================================*/



/*===========================================================================
 * FUNCTION
 ===========================================================================*/
//urc 主动上报
typedef int32_t (*yopen_ril_urc_callback)(char *line, uint32_t line_len);
//AT指令回调
typedef int32_t (*yopen_ril_callback)(char *line, uint32_t line_len, void *user_data);

/**
 * @brief ril初始化
 * 
 * @param callback    URC上报回调函数  
 * @return yopen_errcode_ril_e 
 */
extern yopen_errcode_ril_e yopen_ril_init(yopen_ril_urc_callback callback);


/**
 * @brief ril发送AT指令函数
 * 
 * @param cmd AT指令字符串， 指令未需要带回车换行\r\n
 * @param cmd_len        指令长度
 * @param callback    AT指令返回结果回调函数 
 * @param param       回调函数参数
 * @return yopen_errcode_ril_e 
 */
extern yopen_errcode_ril_e yopen_ril_send_atcmd(char* cmd, uint32_t cmd_len, yopen_ril_callback callback, void *param);



/**
 * @brief ril去初始化
 * 
 * @return yopen_errcode_adc_e 
 */
yopen_errcode_ril_e yopen_ril_deinit(void);


#ifdef __cplusplus
} /*"C" */
#endif

#endif




