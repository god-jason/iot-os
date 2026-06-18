/**  @file
  ycom_usb.h

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

#ifndef _YOPEN_USB_H_
#define _YOPEN_USB_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "yopen_api_common.h"
#include "yopen_type.h"

/*===========================================================================
 * Macro Definition
 ===========================================================================*/
/**
* @defgroup yopen_usb usb功能
* @{
*/
	
/*===========================================================================
 * Enum
 ===========================================================================*/
/** @brief USB错误码 参考yopen_error.h*/
typedef int yopen_errcode_usb_e;

/** @brief USB插入状态*/
typedef enum
{
	YOPEN_USB_HOTPLUG_OUT = 0,    ///< USB拔出
	YOPEN_USB_HOTPLUG_IN  = 1	   ///< USB插入
}YOPEN_USB_HOTPLUG_E;

/**
* @brief usb插入状态回调函数
*
* @param state	[out]  	用于指示USB当前状态为插入/拔出
* @param ctx    [out]   回调函数传参指针(暂未使用)
*
* @return 0
*/

typedef int(*yopen_usb_hotplug_cb)(YOPEN_USB_HOTPLUG_E state, void *ctx);

/**
* @brief  注册USB事件回调函数
* 
* @param hotplug_callback	[in] 需要注册的回调函数，若为NULL,则取消注册callback
*
* @return yopen_errcode_usb_e
*/

extern yopen_errcode_usb_e yopen_usb_bind_hotplug_cb(yopen_usb_hotplug_cb hotplug_callback);


/**
* @brief 获取USB插拔状态
* 
* @return YOPEN_USB_HOTPLUG_E
*/

extern YOPEN_USB_HOTPLUG_E yopen_usb_get_hotplug_state(void);


extern bool yopen_usb_isenabled(void);


/**
* @brief 使能USB
*
* @return yopen_errcode_usb_e
*/

extern yopen_errcode_usb_e yopen_usb_enable(void);


/**
* @brief 禁能USB
*
* @return yopen_errcode_usb_e
*/

extern yopen_errcode_usb_e yopen_usb_disable(void);


/** @}*/

#ifdef __cplusplus
}
#endif
#endif

