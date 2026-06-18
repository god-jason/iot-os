
/**  @file
  demo_pwm.c

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
                
#include <stdio.h>
#include <string.h>

#include "yopen_os.h"
#include "yopen_pwm.h"
#include "yopen_gpio.h"
#include "yopen_debug.h"


void pwm_demo_thread(void *argv)
{
  yopen_pwm_cfg_s ptr;

  yopen_trace("%s", "-------pwm demo start--------");

  yopen_rtos_task_sleep_ms(2000);

  yopen_aon_power_on();

  yopen_pin_set_func(YOPEN_PIN_GPIO4,3); //PWM0 AUX_RXD
  
  yopen_pin_set_func(YOPEN_PIN_GPIO5,3); //PWM1 AUX_TXD
  yopen_pin_set_func(YOPEN_PIN_GPIO11,0);
  
  yopen_pin_set_func(YOPEN_PIN_GPIO12,0); //PWM2 STATUS
  yopen_pin_set_func(YOPEN_PIN_GPIO2,3);

  yopen_pin_set_func(YOPEN_PIN_GPIO3,3); //PWM3 USIM2_DATAx

   yopen_pin_set_func(YOPEN_PIN_GPIO14,3); //PWM4  NET_STATUS
  //yopen_pin_set_func(YOPEN_PIN_GPIO1,3);

  ptr.duty = 10;
  ptr.period = 2000;
  yopen_pwm_open(YOPEN_PWM_0);
  yopen_pwm_enable(YOPEN_PWM_0,&ptr);//PWM0 2Khz, 10%

  ptr.duty = 10;
  ptr.period = 3000;
  yopen_pwm_open(YOPEN_PWM_1);
  yopen_pwm_enable(YOPEN_PWM_1,&ptr);//PWM1 3khz, 10%

  ptr.duty = 50;
  ptr.period = 30000;
  yopen_pwm_open(YOPEN_PWM_2);
  yopen_pwm_enable(YOPEN_PWM_2,&ptr);//PWM2 30khz, 50%

  ptr.duty = 80;
  ptr.period = 50000;
  yopen_pwm_open(YOPEN_PWM_3);
  yopen_pwm_enable(YOPEN_PWM_3,&ptr);//PWM3 50khz, 80%

  ptr.duty = 80;
  ptr.period = 50000;
  yopen_pwm_open(YOPEN_PWM_4);
  yopen_pwm_enable(YOPEN_PWM_4,&ptr);//PWM4 50khz, 80%
  // while(1)
  // {

  //     yopen_rtos_task_sleep_ms(2000);
      
  //     yopen_pwm_disable(YOPEN_PWM_2);
      
  //     yopen_rtos_task_sleep_ms(2000);
      
  //     yopen_pwm_enable(YOPEN_PWM_2,&ptr);
  // }
  yopen_rtos_task_delete(NULL);
}
