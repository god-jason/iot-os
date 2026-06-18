/**  @file
  ycom_pin.h

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

#ifndef _YOPEN_PIN_H_
#define _YOPEN_PIN_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "yopen_osi_def.h"
#include "yopen_type.h"

/*========================================================================
 *	Macro
 *========================================================================*/
#ifdef TYPE_EC718M

#define YOPEN_PIN_GPIO0 82
#define YOPEN_PIN_GPIO1 22
#define YOPEN_PIN_GPIO2 23
#define YOPEN_PIN_GPIO3 54
#define YOPEN_PIN_GPIO4 80
#define YOPEN_PIN_GPIO5 81
#define YOPEN_PIN_GPIO6 55
#define YOPEN_PIN_GPIO7 56
#define YOPEN_PIN_GPIO8 66
#define YOPEN_PIN_GPIO9 67
#define YOPEN_PIN_GPIO10 28
#define YOPEN_PIN_GPIO11 29
#define YOPEN_PIN_GPIO12 64
#define YOPEN_PIN_GPIO13 63
#define YOPEN_PIN_GPIO14 62
#define YOPEN_PIN_GPIO15 49
#define YOPEN_PIN_GPIO16 38
#define YOPEN_PIN_GPIO17 39
#define YOPEN_PIN_GPIO18 17
#define YOPEN_PIN_GPIO19 18
#define YOPEN_PIN_GPIO20 5
#define YOPEN_PIN_GPIO21 6
#define YOPEN_PIN_GPIO22 19
#define YOPEN_PIN_GPIO23 100
#define YOPEN_PIN_GPIO24 101  
#define YOPEN_PIN_GPIO25 16
#define YOPEN_PIN_GPIO26 25
#define YOPEN_PIN_GPIO27 20
//#define YOPEN_PIN_GPIO28   // 内部占用
#define YOPEN_PIN_GPIO29 30
#define YOPEN_PIN_GPIO30 31
#define YOPEN_PIN_GPIO31 32
#define YOPEN_PIN_GPIO32 33
#define YOPEN_PIN_GPIO33 26
#define YOPEN_PIN_GPIO34 53
#define YOPEN_PIN_GPIO35 52
#define YOPEN_PIN_GPIO36 78
#define YOPEN_PIN_GPIO37 50
#define YOPEN_PIN_GPIO38 51

#else
#ifdef YOPEN_MODULE_X08 
#define YOPEN_PIN_GPIO0 25
#define YOPEN_PIN_GPIO1 30
#define YOPEN_PIN_GPIO2 39
#define YOPEN_PIN_GPIO3 40
#define YOPEN_PIN_GPIO4 32
#define YOPEN_PIN_GPIO5 31
#define YOPEN_PIN_GPIO6 2
#define YOPEN_PIN_GPIO7 3
#define YOPEN_PIN_GPIO8 15
#define YOPEN_PIN_GPIO9 14
#define YOPEN_PIN_GPIO10 48
#define YOPEN_PIN_GPIO11 29
#define YOPEN_PIN_GPIO12 50
#define YOPEN_PIN_GPIO13 47 // 内部占用 保持VDD EXT睡眠可以正常供电
#define YOPEN_PIN_GPIO14 27
#define YOPEN_PIN_GPIO15 41
#define YOPEN_PIN_GPIO16 5
#define YOPEN_PIN_GPIO17 49
#define YOPEN_PIN_GPIO18 33
#define YOPEN_PIN_GPIO19 37
#define YOPEN_PIN_GPIO20 38
#else
#define YOPEN_PIN_GPIO0 82
#define YOPEN_PIN_GPIO1 63
#define YOPEN_PIN_GPIO2 62
#define YOPEN_PIN_GPIO3 64
#define YOPEN_PIN_GPIO4 28
#define YOPEN_PIN_GPIO5 29
#define YOPEN_PIN_GPIO6 38
#define YOPEN_PIN_GPIO7 39
#define YOPEN_PIN_GPIO8 17
#define YOPEN_PIN_GPIO9 18

#ifdef YOPEN_MODULE_X19
#define YOPEN_PIN_GPIO10 21
#else
#define YOPEN_PIN_GPIO10 61
#endif

#define YOPEN_PIN_GPIO11 20
#define YOPEN_PIN_GPIO12 25
#define YOPEN_PIN_GPIO13 101
#define YOPEN_PIN_GPIO14 16
#define YOPEN_PIN_GPIO15 74
#define YOPEN_PIN_GPIO16 99
#define YOPEN_PIN_GPIO17 67
#define YOPEN_PIN_GPIO18 66
#define YOPEN_PIN_GPIO19 22
#define YOPEN_PIN_GPIO20 23
#endif
#endif

#ifdef __cplusplus
} /*"C" */
#endif

#endif  //END _YOPEN_GPIO_H_

