#ifndef IOT_ADC_YOPEN_H
#define IOT_ADC_YOPEN_H

#include "../../iot_types.h"
#include "yopen_adc.h"

#define iot_adc_t               yopen_adc_chan_id

#define iot_adc_init(channel, config) \
    do {} while(0)

#define iot_adc_deinit(channel) \
    do {} while(0)

#define iot_adc_read(channel) \
    ({ \
        int _val = 0; \
        yopen_adc_get_volt_raw((yopen_adc_chan_id)channel, YOPEN_ADC_AIO_RESDIV_RATIO_1, &_val); \
        _val; \
    })

#define iot_adc_read_voltage(channel) \
    ({ \
        int _val = 0; \
        yopen_adc_get_volt((yopen_adc_chan_id)channel, &_val); \
        _val; \
    })

#define iot_adc_read_raw(channel, div) \
    ({ \
        int _val = 0; \
        yopen_adc_get_volt_raw((yopen_adc_chan_id)channel, (YOPEN_AdcAioResDiv_e)div, &_val); \
        _val; \
    })

#endif
