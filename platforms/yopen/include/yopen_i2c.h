

#ifndef _YOPEN_I2C_H_
#define _YOPEN_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "yopen_api_common.h"

/**
 * @defgroup yopen_i2c i2c功能
 * @{
*/

/*===========================================================================
 * Macro Definition
===========================================================================*/
#define YOPEN_I2C_ERRCODE_BASE (YOPEN_COMPONENT_BSP_I2C<<16)

/*========================================================================
*  Enumeration Definition
*========================================================================*/
/** @brief I2C总线编号*/
typedef enum
{
	i2c_0 = 0,	//i2c 通道0
	i2c_1,		  //i2c 通道1
}yopen_i2c_channel_e;

/** @brief I2C工作模式*/
typedef enum
{
	STANDARD_MODE = 0,	//标准模式 (100K)
	FAST_MODE = 1,		  //快速模式 (400K)
} yopen_i2c_mode_e;

/** @brief I2C返回码*/ 
typedef enum
{
    YOPEN_I2C_SUCCESS = YOPEN_SUCCESS,                                    //函数执行成功

    YOPEN_I2C_INIT_ERR                 = 1|YOPEN_I2C_ERRCODE_BASE,        //I2C初始化失败
    YOPEN_I2C_NOT_INIT_ERR,                                            //I2C未初始化
    YOPEN_I2C_INVALID_PARAM_ERR,                                       //参数无效

    YOPEN_I2C_WRITE_ERR                = 5|YOPEN_I2C_ERRCODE_BASE,        //向I2C写入数据失败
    YOPEN_I2C_READ_ERR,                                                //从I2C读取数据失败
    YOPEN_I2C_RELEASE_ERR,                                             //释放I2C总线失败
    YOPEN_I2C_OTHER
}yopen_errcode_i2c_e;










/*===========================================================================
 * Function Definition
*===========================================================================*/
/**
 * @brief 初始化I2C主机
 * @param i2c_no        I2C总线编号
 * @param Mode          I2C工作模式
 * @return ** yopen_errcode_i2c_e 
 */
extern yopen_errcode_i2c_e yopen_I2cInit(yopen_i2c_channel_e i2c_no, yopen_i2c_mode_e Mode);


/**
 * @brief 该函数用于向I2C总线写入数据，从设备的寄存器地址长度为8位
 * @param i2c_no        I2C总线编号
 * @param slave         I2C从设备地址
 * @param addr          I2C从设备的寄存器地址
 * @param data          写入的数据
 * @param length        写入数据的长度
 * @return ** yopen_errcode_i2c_e 
 */
extern yopen_errcode_i2c_e yopen_I2cWrite(yopen_i2c_channel_e i2c_no, uint8_t slave, uint8_t addr, uint8_t *data, uint32_t length);


/**
 * @brief 该函数用于从I2C总线读取数据，从设备的寄存器地址长度为8位
 * @param i2c_no        I2C总线编号
 * @param slave         I2C从设备地址
 * @param addr          I2C从设备的寄存器地址
 * @param buf           读取的数据
 * @param length        读取数据的长度
 * @return ** yopen_errcode_i2c_e 
 */
extern yopen_errcode_i2c_e yopen_I2cRead(yopen_i2c_channel_e i2c_no, uint8_t slave, uint8_t addr, uint8_t *buf, uint32_t length);


/**
 * @brief 该函数用于释放I2C总线
 * @param i2c_no        I2C总线编号
 * @return ** yopen_errcode_i2c_e 
 */
extern yopen_errcode_i2c_e yopen_I2cRelease(yopen_i2c_channel_e i2c_no);


/**
 * @brief 该函数用于向I2C总线写入数据，从设备的寄存器地址长度为16位
 * @param i2c_no        I2C总线编号
 * @param slave         I2C从设备地址
 * @param addr          I2C从设备的寄存器地址
 * @param data          写入的数据
 * @param length        写入数据的长度
 * @return ** yopen_errcode_i2c_e 
 */
extern yopen_errcode_i2c_e yopen_I2cWrite_16bit_addr(yopen_i2c_channel_e i2c_no, uint8_t slave, uint16_t addr, uint8_t *data, uint32_t length);


/**
 * @brief 该函数用于从I2C总线读取数据，从设备的寄存器地址长度为16位
 * @param i2c_no        I2C总线编号
 * @param slave         I2C从设备地址
 * @param addr          I2C从设备的寄存器地址
 * @param buf           读取的数据
 * @param length        读取数据的长度
 * @return ** yopen_errcode_i2c_e 
 */
extern yopen_errcode_i2c_e yopen_I2cRead_16bit_addr(yopen_i2c_channel_e i2c_no, uint8_t slave, uint16_t addr, uint8_t *buf, uint32_t length);


/**
 * @brief 该函数用于向I2C总线写入数据，不需要使用从设备的寄存器
 * @param i2c_no        I2C总线编号
 * @param slave         I2C从设备地址
 * @param data          写入的数据
 * @param length        写入数据的长度
 * @return ** yopen_errcode_i2c_e 
 */
extern yopen_errcode_i2c_e yopen_I2cWrite_Noaddr(yopen_i2c_channel_e i2c_no, uint8_t slave, uint8_t *data, uint32_t length);


/**
 * @brief 该函数用于从I2C总线读取数据，不需要使用从设备的寄存器
 * @param i2c_no        I2C总线编号
 * @param slave         I2C从设备地址
 * @param buf           读取的数据
 * @param length        读取数据的长度
 * @return ** yopen_errcode_i2c_e 
 */
extern yopen_errcode_i2c_e yopen_I2cRead_Noaddr(yopen_i2c_channel_e i2c_no, uint8_t slave, uint8_t *buf, uint32_t length);


/** @}*/

#ifdef __cplusplus
} /*"C" */
#endif

#endif  //_YOPEN_I2C_H_