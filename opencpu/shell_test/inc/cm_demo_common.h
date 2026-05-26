/**
 * @file        cm_demo_common.h
 * @brief       opencpu 通用头文件
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By tw
 * @date        2021/03/18
 */

#ifndef __CM_COMMON_H__
#define __CM_COMMON_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/
 
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef FALSE
#define FALSE (0U)
#endif

#ifndef TRUE
#define TRUE (1U)
#endif

/****************************************************************************
 * Public Types
 ****************************************************************************/
 

/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

extern void cm_uart_printf_urc(const char *str, ...);
extern void cm_uart_printf_result(uint8_t at_handle, uint32_t result);


#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

#define cm_demo_printf osPrintf

#undef EXTERN
#ifdef __cplusplus
}
#endif

/** @}*/
/** @}*/

#endif /* __CM_COMMON_H__ */
