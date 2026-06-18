/**
 * @file        cm_camera_sensor.h
 * @brief       相机硬件驱动适配接口
 * @copyright   Copyright @2023 China Mobile IOT. All rights reserved.
 * @author      By chenxy
 * @date        2023/03/13
 *
 * @defgroup camera
 * @ingroup camera
 * @{
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>
#include "os.h"
#include "i2c_device.h"
#include "cm_camera_sensor.h"
#include "drv_gpio.h"

#include "gc032a.h"


//#include "drv_pin.h"
//#include "drv_gpio.h"

//打印LOG 需根据平台适配
#define CAMERA_DEBUG_LOG_SHELL
#ifdef CAMERA_DEBUG_LOG_SHELL
#define isp_log(fmt, args...)       os_kprintf("[ISP]%s %u:"   fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#define isp_log_debug(fmt, args...) os_kprintf("[ISP_D]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#define isp_log_err(fmt, args...)   os_kprintf("[ISP_E]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#else
#define isp_log(fmt, args...)       app_printf("[ISP]%s %u:"   fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#define isp_log_debug(fmt, args...) app_printf("[ISP_D]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#define isp_log_err(fmt, args...)   app_printf("[ISP_E]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__,  ##args)
#endif
#define isp_log_sh(fmt, args...)    os_kprintf("[ISP]%s %u:"   fmt "\r\n", __FUNCTION__, __LINE__,  ##args)

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
/* raw8 */
#define MEDIA_BUS_FMT_SBGGR8_1X8        0x3001
#define MEDIA_BUS_FMT_SGBRG8_1X8        0x3013
#define MEDIA_BUS_FMT_SGRBG8_1X8        0x3002
#define MEDIA_BUS_FMT_SRGGB8_1X8        0x3014
/* raw10 */
#define MEDIA_BUS_FMT_SBGGR10_1X10        0x3007
#define MEDIA_BUS_FMT_SGBRG10_1X10        0x300e
#define MEDIA_BUS_FMT_SGRBG10_1X10        0x300a
#define MEDIA_BUS_FMT_SRGGB10_1X10        0x300f
/* yuv422 */
#define MEDIA_BUS_FMT_UYVY8_2X8            0x2006
#define MEDIA_BUS_FMT_VYUY8_2X8            0x2007
#define MEDIA_BUS_FMT_YUYV8_2X8            0x2008
#define MEDIA_BUS_FMT_YVYU8_2X8            0x2009

/****************************************************************************
 * Public Types
 ****************************************************************************/



/****************************************************************************
 * Public Data
 ****************************************************************************/
static cm_cam_sensor_data_t *g_p_sensor_data = NULL;

/*
static GC032A_PIN_DEFINE(g_sensor_pin_config, BSP_I2C_BUS_NUM,
                         SPICAM_POWERDOWN_PIN_RES, SPICAM_POWERDOWN_PIN_MUX,
                         SPICAM_MCLK_PIN_RES, SPICAM_MCLK_PIN_MUX,
                         SPICAM_CLK_PIN_RES, SPICAM_CLK_PIN_MUX,
                         SPICAM_DI0_PIN_RES, SPICAM_DI0_PIN_MUX,
                         SPICAM_DI1_PIN_RES, SPICAM_DI1_PIN_MUX);
*/

static GC032A_PinConfig g_sensor_pin_config =
{
//    .i2cBusNum    = BSP_I2C_BUS_NUM,
//    .pinPowerDown = {.pinRes = SPICAM_POWERDOWN_PIN_RES, .pinMux = SPICAM_POWERDOWN_PIN_MUX},
//    .pinMclk      = {.pinRes = SPICAM_MCLK_PIN_RES, .pinMux = SPICAM_MCLK_PIN_MUX},
//    .pinSpiClk    = {.pinRes = SPICAM_CLK_PIN_RES, .pinMux = SPICAM_CLK_PIN_MUX},
//    .pinSpiDi0    = {.pinRes = SPICAM_DI0_PIN_RES, .pinMux = SPICAM_DI0_PIN_MUX},
//    .pinSpiDi1    = {.pinRes = SPICAM_DI0_PIN_RES, .pinMux = SPICAM_DI1_PIN_MUX},
0
};


/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/


static int32_t cm_camera_sensor_io_init(cm_cam_sensor_data_t * p_sensor_data)
{
    // 初始化引脚配置（这里需要根据实际硬件配置）
    GC032A_PinInit(&g_sensor_pin_config);
}

/**
 * @brief 使能mclk
 *
 * @param [in] enable    使能(1 打开， 0 关闭)
 *
 * @return
 *
 * @details 用于相机上电逻辑，控制mclk时钟输出
 */
void cm_camera_sensor_set_mclk(int32_t enable)
{
    isp_log("enable=%d", enable);
#if 0
    // 这里需要根据实际硬件配置实现
    // 示例：控制MCLK引脚
    if (enable) {
        // 使能MCLK
        osPrintf("Camera MCLK enabled\r\n");
        PIN_SetMux(GC032A_PIN_RES_MCLK(config), GC032A_PIN_MUX_MCLK(config)); // MCLK 没有专用管脚，仅仅复用到GPIO管脚
        GPIO_SetDir(GC032A_PIN_RES_MCLK(config), GPIO_OUTPUT);
    } else {
        // 禁用MCLK
        osPrintf("Camera MCLK disabled\r\n");
    }
#endif
}

/**
 * @brief 设置驱动参数
 *
 * @param [in] data         驱动参数
 *
 * @return
 *
 * @details 操作相机前需设置
 */
cm_cam_sensor_data_t * cm_camera_sensor_get_data(void)
{
    return g_p_sensor_data;
}


/**
 * @brief 设置驱动参数
 *
 * @param [in] data         驱动参数
 *
 * @return
 *
 * @details 操作相机前需设置
 */
void cm_camera_sensor_set_data(cm_cam_sensor_data_t *data)
{
    if (data == NULL)
    {
        isp_log_err("data NULL");
    }
    g_p_sensor_data = data;

//    cm_camera_sensor_io_init(cm_cam_sensor_data_t *data);
}

/**
 * @brief i2c读取
 *
 * @param [in] attr         i2c参数
 * @param [in] reg          寄存器地址
 * @param [out] val         读取值
 *
 * @return  0 成功；-1 失败
 *
 * @details 通过i2c读取相机寄存器
 */
int32_t cm_camera_sensor_i2c_read(const cm_cam_sensor_i2c_attr *attr, uint32_t reg, uint32_t *val)
{
    if (!attr || !val) {
        return -1;
    }

    I2C_BusDevice dev = {0};
    dev.addr = attr->addr;

    I2C_BusLock(&dev);
    int ret = 0;

    switch (attr->reg_len) {
        case CAM_I2C_8BIT:
            ret = I2C_BusDevice_ReadReg(&dev, (uint8_t)reg, (uint8_t *)val, 1);
            break;
        case CAM_I2C_16BIT:
            ret = I2C_BusDevice_ReadReg(&dev, (uint16_t)reg, (uint8_t *)val, 2);
            break;
        case CAM_I2C_32BIT:
            ret = I2C_BusDevice_ReadReg(&dev, (uint32_t)reg, (uint8_t *)val, 4);
            break;
        default:
            ret = -1;
            break;
    }

    I2C_BusUnlock(&dev);

    return ret;
}

/**
 * @brief i2c写
 *
 * @param [in] attr         i2c参数
 * @param [in] regs         寄存器操作表
 *
 * @return  0 成功；-1 失败
 *
 * @details 通过i2c写相机寄存器
 */
int32_t cm_camera_sensor_i2c_write(const cm_cam_sensor_i2c_attr *attr, cm_cam_sensor_regs regs)
{
    if (!attr || !regs.tab)
    {
        isp_log_err("attr=%x", attr);
        return -1;
    }

    I2C_BusDevice dev;
    dev.addr = attr->addr;

    I2C_BusLock(&dev);

    for (uint32_t i = 0; i < regs.num; i++)
    {
        const cm_cam_regval_tab *tab = &regs.tab[i];
        uint8_t buf[4] = {0};
        uint8_t len = 0;

        switch (attr->reg_len)
        {
            case CAM_I2C_8BIT:
                buf[0] = (uint8_t)tab->reg;
                len = 1;
                break;
            case CAM_I2C_16BIT:
                buf[0] = (uint8_t)(tab->reg >> 8);
                buf[1] = (uint8_t)tab->reg;
                len = 2;
                break;
            case CAM_I2C_32BIT:
                buf[0] = (uint8_t)(tab->reg >> 24);
                buf[1] = (uint8_t)(tab->reg >> 16);
                buf[2] = (uint8_t)(tab->reg >> 8);
                buf[3] = (uint8_t)tab->reg;
                len = 4;
                break;
            default:
                I2C_BusUnlock(&dev);
                isp_log_err("reg_len=%d", attr->reg_len);
                return -1;
        }

        switch (attr->val_len)
        {
            case CAM_I2C_8BIT:
                buf[len] = (uint8_t)tab->val;
                len += 1;
                break;
            case CAM_I2C_16BIT:
                buf[len] = (uint8_t)(tab->val >> 8);
                buf[len + 1] = (uint8_t)tab->val;
                len += 2;
                break;
            case CAM_I2C_32BIT:
                buf[len] = (uint8_t)(tab->val >> 24);
                buf[len + 1] = (uint8_t)(tab->val >> 16);
                buf[len + 2] = (uint8_t)(tab->val >> 8);
                buf[len + 3] = (uint8_t)tab->val;
                len += 4;
                break;
            default:
                I2C_BusUnlock(&dev);
                return -1;
        }

        if (I2C_BusDevice_Write(&dev, buf, len) != 0)
        {
            I2C_BusUnlock(&dev);
            return -1;
        }

        // 延时
        if (tab->mask > 0)
        {
            osThreadMsSleep(tab->mask);
        }
    }

    I2C_BusUnlock(&dev);

    return 0;
}

/**
 * @brief i2c写（单个数据）
 *
 * @param [in] attr         i2c参数
 * @param [in] reg          寄存器地址
 * @param [out] val         写入值
 *
 * @return  0 成功；-1 失败
 *
 * @details 通过i2c写相机寄存器
 */
int32_t cm_camera_sensor_i2c_write_single(const cm_cam_sensor_i2c_attr *attr, uint32_t reg, uint32_t val)
{
    cm_cam_regval_tab tab = {
        .reg = (uint16_t)reg,
        .val = (uint16_t)val,
        .mask = 0
    };

    cm_cam_sensor_regs regs = {
        .tab = &tab,
        .num = 1
    };

    return cm_camera_sensor_i2c_write(attr, regs);
}

/** @}*/

