
/**
 * @file lv_port_indev_templ.h
 *
 */

#ifndef LVGL_PORT_INDEV_H
#define LVGL_PORT_INDEV_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
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
void LVGL_IndevInit(InputDevice_t *touch, InputDevice_t *keypad, InputDevice_t *button);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PORT_INDEV_H*/
