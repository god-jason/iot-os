/**
 * @file        cm_pwm.h
 * @brief       PWM接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By ZHANGXW
 * @date        2021/03/09
 *
 * @defgroup pwm pwm
 * @ingroup PI
 * @{
 */
/****************************************************************************
 ***********************PWM使用注意事项*************************************
 * 1、禁止将多个引脚映射到同PWM设备上（DEV);
 *2、注意时钟源的选择，低功耗下仅支持32k的时钟；
 ***************************************************************************/

#ifndef __CMO_PWM_H__
#define __CMO_PWM_H__

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include "cm_iomux.h"

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define OPENCPU_TEST_PWM0_IOMUX CM_IOMUX_PIN_74, CM_IOMUX_FUNC_FUNCTION6     //PIN_9_MUX_AON_PWM_0
#define OPENCPU_TEST_PWM1_IOMUX CM_IOMUX_PIN_75, CM_IOMUX_FUNC_FUNCTION2    //PIN_26_MUX_PD_PWM

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** 设备ID，详情参照资源综述 ，禁止多个引脚映射到同一dev上*/
typedef enum{
    CM_PWM_DEV_0,              /*!< 设备0*/
    CM_PWM_DEV_1,              /*!< 设备1*/
    CM_PWM_DEV_NUM
} cm_pwm_dev_e;

/** 时钟源枚举 */
typedef enum{
    CM_PWM_CLK_32K = 32000,              /*!< 32k时钟源，支持低功耗下使用*/
    CM_PWM_CLK_12800K = 12800000,           /*!< 12.8M时钟源，不支持低功耗下使用*/
    CM_PWM_CLK_END
} cm_pwm_clk_e;


/****************************************************************************
 * Public Data
 ****************************************************************************/


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
*  @brief PWM 时钟源选择
*
*  @param [in] dev PWM设备ID
*  @param [in] clk 时钟源
*
*  @return
*    = 0 - 成功 \n
*    < 0 - 失败, 返回值为错误码
*
*  @details clk可以选择32k的时钟，也可以选择12.8M的时钟，低功耗下仅支持32k的时钟；需要在open函数之前设置
*  @details : 该接口 暂不需要 PWM直接 将引脚复用后 直接open即可产生波形
* 不支持选择12.8M
*/
int32_t cm_pwm_set_clk(cm_pwm_dev_e dev, cm_pwm_clk_e clk);

/**
 *  @brief 打开PWM设备
 *
 *  @param [in] dev PWM设备ID
 *  @param [in] period 周期(ns),最大为5120000
 *  @param [in] period_h 周期高电平占用时间(ns)
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details PWM由于分频和算法设计存在一定的误差，受限于软件和硬件，因此period和period_h需根据下面备注进行设置\n
 *   How to get the PV(The value of after scaled clock cycles per cycle/T): \n
 *   InputClockT=13M;PWM_T=period;
 *  InputClockT * ('PV(The value of before scaled clock cycles per cycle)) = PWM_T \n
 *      'PV   = PWM_T / InputClockT \n
 *           = PWM_T * InputClockF \n
 *      PV = 'PV /(prescale + 1) -1 \n
 *  How to get the prescale: \n
 *      Because the internal clock period counter is 10-bit, to avoid overun. We can use the prescale.\n
 *      prescale real value  = ('PV - 1(if > 1024, then need prescale)) / 1024 \n
 *
 *  How to get the Duty cycle: \n
 *      Duty cycle = (PV(the cycles per T) + 1) * ratio(Duty time/PWM_T)
*/
int32_t cm_pwm_open_ns(cm_pwm_dev_e dev, uint32_t period, uint32_t period_h);

/**
 *  @brief 关闭PWM设备
 *
 *  @param [in] dev PWM设备ID
 *
 *  @return
 *    = 0 - 成功 \n
 *    < 0 - 失败, 返回值为错误码
 *
 *  @details 需要在open函数之后使用
 */
int32_t cm_pwm_close(cm_pwm_dev_e dev);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_PWM_H__ */

/** @}*/
