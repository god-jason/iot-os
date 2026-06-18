/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_soft_i2c.h
 *
 * @brief       I2C软件模拟驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef DRIVER_SOFT_I2C_H_
#define DRIVER_SOFT_I2C_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <stdint.h>
#include <drv_common.h>

/**
 * @addtogroup I2c
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define I2C_SOFT_NAME_LEN           (10)
#define I2C_SOFT_LIST               (1)

#ifndef DRV_OK
#define DRV_OK                      (0)
#endif

#ifndef DRV_ERR_TIMEOUT
#define DRV_ERR_TIMEOUT             (-1)
#endif


/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef enum {
    I2C_SOFT_GPIO_INPUT_MODE = 0,
    I2C_SOFT_GPIO_OUTPUT_MODE,
}I2C_SOFT_GPIO_DIR;

typedef struct {
    void (*init)(void *param);
    void (*scl_write)(void *param, uint8_t value);
    void (*sda_write)(void *param, uint8_t value);
    void (*sda_dir)(void *param, I2C_SOFT_GPIO_DIR dir);
    uint8_t (*sda_read)(void *param);
} I2C_SOFT_Ops;

typedef struct I2C_SOFT_Handle {
    void *userData;
    I2C_SOFT_Ops *ops;
    uint32_t speed;
    uint32_t speed_us;
#if (I2C_SOFT_LIST)
    char name[I2C_SOFT_NAME_LEN];
    struct I2C_SOFT_Handle *next;
#endif
} I2C_SOFT_Handle;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/

#if (I2C_SOFT_LIST)
I2C_SOFT_Handle* I2C_SOFT_Find(const char* name);
#endif

int32_t I2C_SOFT_Initialize(I2C_SOFT_Handle *hdl, const char *name, I2C_SOFT_Ops *ops);
int32_t I2C_SOFT_WriteByte(I2C_SOFT_Handle *hdl, uint8_t byte);
uint8_t I2C_SOFT_ReadByte(I2C_SOFT_Handle *hdl, uint8_t ack);
int32_t I2C_SOFT_WriteBytes(I2C_SOFT_Handle *hdl, uint8_t slave_addr, void *pbuf, uint16_t length);
int32_t I2C_SOFT_WriteRegBytes(I2C_SOFT_Handle *hdl, uint8_t slave_addr, uint8_t reg_addr, void *pbuf, uint16_t length);
void I2C_SOFT_ReadBytes(I2C_SOFT_Handle *hdl, uint8_t slave_addr, uint8_t reg_addr, void *pbuf, uint16_t length);
int32_t I2C_SOFT_WriteBytes_16bit(I2C_SOFT_Handle *hdl, uint8_t slave_addr, uint16_t reg_addr, void *pbuf, uint16_t length);
void I2C_SOFT_ReadBytes_16bit(I2C_SOFT_Handle *hdl, uint8_t slave_addr, uint16_t reg_addr, void *pbuf, uint16_t length);

#endif /* __SF_I2C_H */
/** @} */