


/*=================================================================

						EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN			  WHO		  WHAT, WHERE, WHY
------------	 -------	 -------------------------------------------------------------------------------

=================================================================*/
#ifndef YOPEN_PWM_H
#define YOPEN_PWM_H


/*===========================================================================
 * include files
 ===========================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "yopen_api_common.h"
#include "yopen_os.h"

#define YOPEN_PWM_ERRCODE_BASE (YOPEN_COMPONENT_BSP_PWM<<16)

/**
 * PWM errcode 参考yopen_error.h
 */
typedef int yopen_errcode_pwm;

/**
 * PWM function select
 */
typedef enum
{
    YOPEN_PWM_0,
    YOPEN_PWM_1,
    YOPEN_PWM_2,
    YOPEN_PWM_3,
    YOPEN_PWM_4,
    PWM_MAX,
}yopen_pwm_sel;

typedef enum
{
    YOPEN_CLK_RC26M,
    YOPEN_CLK_RTC_40K
}yopen_pwm_clk;

typedef struct
{
	uint32_t   period;
	uint16_t   duty;
} yopen_pwm_cfg_s;


typedef enum
{
    YOPEN_PWM_INTERRUPT_DISABLE     = 0U,  /**< Disable interrupt */
    YOPEN_PWM_INTERRUPT_LEVEL       = 1U,  /**< Level interrupt, a high level interrupt signal is generated */
    YOPEN_PWM_INTERRUPT_PULSE       = 2U,  /**< Pulse interrupt, don't need to clear interrupt flag for this kind of interrupt type,
	                                        so you can't be able to get the interrupt sources when multiply interrupts are enabled */
} yopen_pwm_interrupt_type_e;

typedef void (*yopen_pwm_int_cb)(void);

/**
 * @brief 打开PWM功能
 * 
 * @param pwm_sel 设置PWM通道
 * @return yopen_errcode_pwm 
 */
extern yopen_errcode_pwm yopen_pwm_open
(
    yopen_pwm_sel pwm_sel
);



/**
 * @brief 关闭PWM功能
 * 
 * @param pwm_sel PWM通道
 * @return yopen_errcode_pwm 
 */
extern yopen_errcode_pwm yopen_pwm_close
(
    yopen_pwm_sel pwm_sel
);



/**
 * @brief 使能PWM功能，配置相关参数
 * 
 * @param pwm_sel PWM通道
 * @param pwm_cfg PWM参数
 * @return yopen_errcode_pwm 
 */
extern yopen_errcode_pwm yopen_pwm_enable
(
    yopen_pwm_sel pwm_sel,
    yopen_pwm_cfg_s *pwm_cfg
);



/**
 * @brief 暂停PWM
 * 
 * @param pwm_sel PWM通道
 * @return yopen_errcode_pwm 
 */
extern yopen_errcode_pwm yopen_pwm_disable
(
    yopen_pwm_sel pwm_sel
);

/**
 * @brief 暂停PWM
 * 
 * @param pwm_sel PWM通道
 * @param type    中断类型
 * @param cb      中断回调函数
 * @return yopen_errcode_pwm 
 */
extern yopen_errcode_pwm yopen_pwm_int_register
(
    yopen_pwm_sel pwm_sel,
	yopen_pwm_interrupt_type_e type,
	yopen_pwm_int_cb  cb

);

/**
 * @brief 暂停PWM
 * 
 * @param pwm_sel PWM通道
 * @param duty    占峰比 0-100
 * @return yopen_errcode_pwm 
 */
extern yopen_errcode_pwm yopen_pwm_duty_set
(
    yopen_pwm_sel pwm_sel, 
    uint16_t duty
);

#endif /* YOPEN_PWM_H */

