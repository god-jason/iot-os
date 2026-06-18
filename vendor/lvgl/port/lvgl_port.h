/**
 * @file lv_port_disp_templ.h
 *
 */

#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <drv_display.h>
#include "drv_input.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 ************************************************************************************
 * @brief           LVGL初始化
 *
 * @param[in]       parameter        入参
 * @return          无.
 ************************************************************************************
 */ 
int LVGL_PortInit(DispDevice_t *pDispDev, InputDevice_t *touch, InputDevice_t *keypad, InputDevice_t *button);

/**
 ************************************************************************************
 * @brief           锁LVGL UI刷新
 *
 * @param[in]       parameter        入参
 * @return          无.
 ************************************************************************************
 */ 
void LVGL_Lock(void);

/**
 ************************************************************************************
 * @brief           释放LVGL UI刷新
 *
 * @param[in]       parameter        入参
 * @return          无.
 ************************************************************************************
 */ 
void LVGL_Unlock(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PORT_H*/
