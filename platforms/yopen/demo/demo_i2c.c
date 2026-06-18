#include "stdio.h"
#include "string.h"

#include "yopen_debug.h"
#include "yopen_i2c.h"
#include "yopen_os.h"
#include "yopen_gpio.h"

#define DEMO_I2C_TRACE(fmt, ...) yopen_trace("[I2C] "fmt, ##__VA_ARGS__)

#define demo_i2c_no i2c_0

//AHT10温度传感器
#define DEMO_I2C_AHT10_ADDR (0x38)

void i2c_pin_init(void)
{   
    int ret;
#ifdef TYPE_EC718M
    //I2C 0
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO14, 2);
    DEMO_I2C_TRACE("========== i2c_pin_init SDA ret %d ==========", ret);

    ret = yopen_pin_set_func(YOPEN_PIN_GPIO15, 2);
    DEMO_I2C_TRACE("========== i2c_pin_init SCL ret %d ==========", ret);

    yopen_pin_set_pull(YOPEN_PIN_GPIO14, FORCE_PULL_UP);
    yopen_pin_set_pull(YOPEN_PIN_GPIO15, FORCE_PULL_UP);

    //I2C 1
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO8, 2);
    DEMO_I2C_TRACE("========== i2c_pin_init SDA ret %d ==========", ret);

    ret = yopen_pin_set_func(YOPEN_PIN_GPIO9, 2);
    DEMO_I2C_TRACE("========== i2c_pin_init SCL ret %d ==========", ret);

    yopen_pin_set_pull(YOPEN_PIN_GPIO8, FORCE_PULL_UP);
    yopen_pin_set_pull(YOPEN_PIN_GPIO9, FORCE_PULL_UP);
#else
    //{66, 	9}, I2C0 SDA 
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO18, 1);
    DEMO_I2C_TRACE("========== i2c_pin_init SDA ret %d ==========", ret);
	//{67, 	8}, I2C0 SCL
    ret = yopen_pin_set_func(YOPEN_PIN_GPIO17, 1);
    DEMO_I2C_TRACE("========== i2c_pin_init SCL ret %d ==========", ret);
#endif
}

void yopen_i2c_demo_thread(void *arvg)
{
    int ret;
    uint32_t  Humidity, Temperature, dat;

    yopen_rtos_task_sleep_ms(2000);

	DEMO_I2C_TRACE("========== i2c demo start ==========");

    i2c_pin_init();

    ret = yopen_I2cInit(demo_i2c_no, STANDARD_MODE);
    DEMO_I2C_TRACE("========== yopen_I2cInit ret %d ==========", ret);

    //AHT10初始化
    uint8_t aht10_init[3] = {0xe1, 0x08, 0x00};
    ret = yopen_I2cWrite_Noaddr(demo_i2c_no, DEMO_I2C_AHT10_ADDR, aht10_init, sizeof(aht10_init));
    DEMO_I2C_TRACE("========== yopen_I2cWrite_Noaddr AHT10 init ret %d ==========", ret);

    yopen_rtos_task_sleep_ms(100);

    while(1)
    {
        //AHT10触发测量
        uint8_t aht10_trigger[3] = {0xac, 0x33, 0x00};
        ret = yopen_I2cWrite_Noaddr(demo_i2c_no, DEMO_I2C_AHT10_ADDR, aht10_trigger, sizeof(aht10_trigger));
        DEMO_I2C_TRACE("========== yopen_I2cWrite_Noaddr ret %d ==========", ret);

        //读数据
        uint8_t aht10_read[6]={0};
        ret = yopen_I2cRead_Noaddr(demo_i2c_no, DEMO_I2C_AHT10_ADDR, aht10_read, sizeof(aht10_read));
        DEMO_I2C_TRACE("========== yopen_I2cRead_Noaddr ret %d, %x,%x,%x,%x,%x,%x ==========", ret, aht10_read[0],aht10_read[1],
                                                                                        aht10_read[2],aht10_read[3],
                                                                                        aht10_read[4],aht10_read[5]);

        // 解析数据
        dat = (((aht10_read[1] << 12) | (aht10_read[2] << 4)) | (aht10_read[3] >> 4));
        Humidity = dat * 100 / 1048576;
        dat = ((aht10_read[3] & 0x0F) << 16) | (aht10_read[4] << 8) | aht10_read[5];
        Temperature = (dat * 200 / 1048576)  - 50;

        DEMO_I2C_TRACE("========== Humidity %d Temperature %d==========", Humidity, Temperature);
        yopen_rtos_task_sleep_ms(2000);
    }
}