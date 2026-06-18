#ifndef IOT_PWM_YOPEN_H
#define IOT_PWM_YOPEN_H

#include "../../iot_types.h"
#include "yopen_pwm.h"

#define iot_pwm_t               yopen_pwm_sel

#define iot_pwm_open(pwm) \
    yopen_pwm_open((yopen_pwm_sel)pwm)

#define iot_pwm_close(pwm) \
    yopen_pwm_close((yopen_pwm_sel)pwm)

#define iot_pwm_enable(pwm, freq, duty) \
    do { \
        yopen_pwm_cfg_s cfg; \
        cfg.period = (1000000000UL / (freq)); \
        cfg.duty = duty; \
        yopen_pwm_enable((yopen_pwm_sel)pwm, &cfg); \
    } while(0)

#define iot_pwm_disable(pwm) \
    yopen_pwm_disable((yopen_pwm_sel)pwm)

#define iot_pwm_set_duty(pwm, duty) \
    yopen_pwm_duty_set((yopen_pwm_sel)pwm, duty)

#define iot_pwm_register_callback(pwm, cb) \
    yopen_pwm_int_register((yopen_pwm_sel)pwm, YOPEN_PWM_INTERRUPT_PULSE, cb)

#endif
