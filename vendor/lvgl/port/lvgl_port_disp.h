/**
 * @file lv_port_disp_templ.h
 *
 */

#ifndef LVGL_PORT_DISPL_H
#define LVGL_PORT_DISPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl.h"
#include "drv_display.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/* Initialize low level display driver */
void LVGL_DispInit(DispDevice_t *pDispDev);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_PORT_DISP_TEMPL_H*/
