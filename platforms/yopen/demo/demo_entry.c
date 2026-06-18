/**  @file
  demo_entry.c

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

#include "yopen_app_main.h"

// this file is used to whether init demo application,just for reference 

extern void hello_demo_thread(void *argv);
extern void os_demo_main_task(void* arg);
extern void os_demo_sub_task(void * argv);
extern void sockets_demo_thread(void * arg);
extern void yopen_fs_demo_task(void * arg);
extern void yopen_gpio_demo_task(void * arg);
extern void yopen_uart_demo_thread(void * arg);
extern void yopen_nw_demo_thread(void * arg);
extern void debug_demo_task(void* arg);
extern void yopen_spi_demo_task(void * arg);
extern void dev_demo_thread(void *argv);
extern void rtc_demo_thread(void *arvg);
extern void yopen_sim_demo_task(void * arg);
extern void yopen_power_demo_thread(void *arvg);
extern void usb_demo_thread(void *arvg);
extern void usbnet_demo_thread(void *arvg);
extern void yopen_adc_demo_task(void * arg);
extern void yopen_DATACALL_demo_task(void * arg);
extern void yopen_fota_demo_task(void * arg);
extern void yopen_i2c_demo_thread(void * arg);
extern void yopen_ril_demo_task(void * arg);
extern void pwm_demo_thread(void *argv);
extern void tts_demo_thread(void* arg);
extern void heartbeat_demo_thread(void * arg);
extern void yopen_i2s_demo_task(void* argv);
extern void yopen_sms_demo_task(void* argv);
extern void yopen_power_psm_demo_thread(void* argv);
extern void wifi_demo_thread(void* argv);
extern void restart_demo_thread(void* argv);
extern void flash_demo_thread(void* argv);
extern void socket_cid_demo_thread(void* argv);
extern void rtos_flags_thread(void* argv);


application_init(sockets_demo_thread, "SOCKETSDEMO", 16, 4,NULL);
application_init(socket_cid_demo_thread, "SOCKETCIDDEMO", 4, 4, NULL);
application_init(yopen_fs_demo_task, "FS", 12, 4,NULL);
//application_init(rtos_flags_thread, "RTOSFLAGS", 12, 4,NULL);
//application_init(yopen_gpio_demo_task, "GPIO", 12, 4,NULL);
// application_init(yopen_uart_demo_thread, "UART", 12, 4,NULL);
// application_init(yopen_nw_demo_thread, "NW", 12, 4,NULL);
// application_init(yopen_power_demo_thread, "PWR", 12, 4,NULL);

// application_init(debug_demo_task, "DEBUG", 4, 4, NULL);
//application_init(dev_demo_thread, "DEV", 4, 4,NULL);
//application_init(yopen_spi_demo_task, "SPI", 12, 4,NULL);
//application_init(rtc_demo_thread, "RTC", 4, 4, NULL);
//application_init(yopen_sim_demo_task, "SIM", 12, 4,NULL);

// application_init(usb_demo_thread, "USB", 4, 4, NULL);
//application_init(usbnet_demo_thread, "USBNET", 4, 4, NULL);
//application_init(yopen_adc_demo_task, "ADC", 4, 8, NULL);
//application_init(yopen_DATACALL_demo_task, "DATACALL", 4, 8, NULL);
//application_init(yopen_fota_demo_task, "FOTA", 4, 8, NULL);
//application_init(yopen_i2c_demo_thread, "I2C", 4, 8, NULL);
//application_init(yopen_ril_demo_task, "RIL", 4, 8, NULL);
// application_init(pwm_demo_thread, "PWM", 4, 4, NULL);
// application_init(tts_demo_thread, "tts", 4, 4, NULL);
// application_init(heartbeat_demo_thread, "heartbeat", 4, 4, NULL);

// application_init(yopen_i2s_demo_task, "I2S", 8, 4, NULL);
// application_init(yopen_sms_demo_task, "SMS", 8, 4, NULL);
 //application_init(yopen_power_psm_demo_thread, "PSM", 8, 4, NULL);
 //application_init(wifi_demo_thread, "WIFI", 32, 4,NULL);
//application_init(restart_demo_thread, "restart", 32, 4,NULL);

//application_init(flash_demo_thread, "FLASH", 4, 4,NULL);
