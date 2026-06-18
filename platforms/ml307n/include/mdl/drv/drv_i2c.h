/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_i2c.h
 *
 * @brief       I2C驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef DRIVER_I2C_H_
#define DRIVER_I2C_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>

/**
 * @addtogroup I2c
 */

/**@{*/

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/


/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/****** I2C Control Codes *****/
#define I2C_OWN_ADDRESS             (0x01UL)    ///< Set Own Slave Address; arg = address 
#define I2C_BUS_SPEED               (0x02UL)    ///< Set Bus Speed; arg = speed
#define I2C_BUS_CLEAR               (0x03UL)    ///< Execute Bus clear: send nine clock pulses
#define I2C_ABORT_TRANSFER          (0x04UL)    ///< Abort Master/Slave Transmit/Receive
#define I2C_LONG_ADDRESS            (0x05UL)

/*----- I2C Bus Speed -----*/
#define I2C_BUS_SPEED_STANDARD      (0x01UL)    ///< Standard Speed (100kHz)
#define I2C_BUS_SPEED_FAST          (0x02UL)    ///< Fast Speed     (400kHz)
#define I2C_BUS_SPEED_FAST_PLUS     (0x03UL)    ///< Fast+ Speed    (  1MHz)
#define I2C_BUS_SPEED_HIGH          (0x04UL)    ///< High Speed     (3.4MHz)


/****** I2C Address Flags *****/

#define I2C_ADDRESS_10BIT           (0x0400UL)  ///< 10-bit address flag
#define I2C_ADDRESS_GC              (0x8000UL)  ///< General Call flag

/****** I2C Event *****/
#define I2C_EVENT_TRANSFER_DONE       (1UL << 0)  ///< Master/Slave Transmit/Receive finished
#define I2C_EVENT_TRANSFER_INCOMPLETE (1UL << 1)  ///< Master/Slave Transmit/Receive incomplete info
#define I2C_EVENT_SLAVE_TRANSMIT      (1UL << 2)  ///< Addressed as Slave Transmitter but transmit operation is not set.
#define I2C_EVENT_SLAVE_RECEIVE       (1UL << 3)  ///< Addressed as Slave Receiver but receive operation is not set.
#define I2C_EVENT_ADDRESS_NACK        (1UL << 4)  ///< Address not acknowledged from Slave
#define I2C_EVENT_GENERAL_CALL        (1UL << 5)  ///< Slave addressed with general call address
#define I2C_EVENT_ARBITRATION_LOST    (1UL << 6)  ///< Master lost arbitration
#define I2C_EVENT_BUS_ERROR           (1UL << 7)  ///< Bus error detected (START/STOP at illegal position)
#define I2C_EVENT_BUS_CLEAR           (1UL << 8)  ///< Bus clear finished


#define I2C_XFER_CTRL_XPENDING          ((uint8_t)0x01)
#define I2C_XFER_CTRL_XACTIVE           ((uint8_t)0x02)
#define I2C_XFER_CTRL_XDONE             ((uint8_t)0x04)
#define I2C_XFER_CTRL_RSTART            ((uint8_t)0x08)
#define I2C_XFER_CTRL_ADDR_DONE         ((uint8_t)0x10)
#define I2C_XFER_CTRL_DMA_DONE          ((uint8_t)0x20)
#define I2C_XFER_CTRL_WAIT_BTF          ((uint8_t)0x40)
#define I2C_XFER_CTRL_CMB_READ          ((uint8_t)0x80)
/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
/**
 * @brief  I2C_TypeDef
 */
typedef struct {
    __IO uint32_t                         version;                          /*!< Offset 0x00000000 */
    __IO uint32_t                         cmd;                              /*!< Offset 0x00000004 */
    __IO uint32_t                         rsvd0[1];                         /*!< Offset 0x00000008 */
    __IO uint32_t                         deviceAddr;                       /*!< Offset 0x0000000c */
    __IO uint32_t                         rsvd1[1];                         /*!< Offset 0x00000010 */
    __IO uint32_t                         clkDiv;                           /*!< Offset 0x00000014 */
    __IO uint32_t                         rsvd2[1];                         /*!< Offset 0x00000018 */
    __IO uint32_t                         txCtrl;                           /*!< Offset 0x0000001c */
    __IO uint32_t                         rxCtrl;                           /*!< Offset 0x00000020 */
    __IO uint32_t                         data;                             /*!< Offset 0x00000024 */
    __IO uint32_t                         status;                           /*!< Offset 0x00000028 */
    __IO uint32_t                         txfStatus;                        /*!< Offset 0x0000002c */
    __IO uint32_t                         rxfStatus;                        /*!< Offset 0x00000030 */
} I2C_TypeDef;

/**
\brief I2C Status
*/
typedef struct _I2C_STATUS {
    uint32_t busy             : 1;        ///< Busy flag
    uint32_t mode             : 1;        ///< Mode: 0=Slave, 1=Master
    uint32_t direction        : 1;        ///< Direction: 0=Transmitter, 1=Receiver
    uint32_t general_call     : 1;        ///< General Call indication (cleared on start of next Slave operation)
    uint32_t arbitration_lost : 1;        ///< Master lost arbitration (cleared on start of next Master operation)
    uint32_t bus_error        : 1;        ///< Bus error detected (cleared on start of next Master/Slave operation)
    uint32_t reserved         : 26;
} I2C_STATUS;

typedef void (*I2C_Callback) (void * i2c, uint32_t event);  ///< Pointer to \ref I2C_SignalEvent : Signal I2C Event.

typedef enum
{
    I2C_SIMPLE_WRITE                     = 0,
    I2C_COMBINE_READ,
    I2C_SIMPLE_READ
}I2C_Mode;

typedef enum
{
    I2C_BUS_0                             = 0,
    I2C_BUS_1                             = 1,
    I2C_BUS_2                             = 2,      
    MAX_I2C_BUS_NUM
}I2C_BusNumber;

typedef struct
{
    uint8_t                              *buf;
    uint16_t                              len;
    uint16_t                              cnt;
}I2C_TRANSFER_INFO;

typedef struct {
    uint8_t                               busNumber;
    uint8_t                               speed;
    uint8_t                               longAddr;
    bool                                  memDev;                 // 特殊性：memory设备寄存器长度为2
}I2C_InitTypedef;

typedef struct{
    I2C_Callback                          cbEvent;
    I2C_STATUS                            status;
    I2C_TRANSFER_INFO                     tx;
    I2C_TRANSFER_INFO                     rx;
    uint16_t                              deviceAddr;
    uint8_t                               ctrl;
    uint8_t                               flags;
} I2C_INFO;

typedef struct
{
    I2C_Res_t          p_I2C_Res;
    I2C_InitTypedef    Init;
    I2C_INFO           info;
    uint8_t            busNumber;
    void              *userData;
} I2C_Handle;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief           初始化I2C控制器
 *
 * @param[in]       I2C            I2C控制器句柄
 *
 * @return          int32_t
 * @retval          DRV_OK          成功   
 ************************************************************************************
*/
int32_t I2C_Initialize(I2C_Handle *i2c, I2C_Callback cbEvent);

/**
 ************************************************************************************
 * @brief           去初始化I2C控制器
 *
 * @param[in]       i2c            I2C控制器句柄
 *
 * @return          int32_t
 * @retval          DRV_OK          成功   
 ************************************************************************************
*/
int32_t I2C_Uninitialize(I2C_Handle *i2c);

/**
 ************************************************************************************
 * @brief           设置I2C控制器的供电
 *
 * @param[in]       I2C             I2C控制器句柄
 * @param[in]       state           设置控制的状态
 *
 * @return          int32_t
 * @retval          DRV_ERR_PARAMETER             错误的参数
 *                  DRV_ERR_UNSUPPORTED           不支持             
 *                  DRV_ERR                       错误
 *                  DRV_OK                        成功   
 ************************************************************************************
*/
int32_t I2C_PowerControl(I2C_Handle *i2c, DRV_POWER_STATE state);

/**
 ************************************************************************************
 * @brief           master执行写传输
 *
 * @param[in]       i2c            I2C控制器句柄
 * @param[in]       addr           设备地址
 * @param[in]      data          发送数据地址
 * @param[in]      num           发送数据长度
 * 
 * @return          int32_t
 *                  DRV_ERR_BUSY        设备忙
 *                  DRV_OK              执行成功
 *                  DRV_ERR_PARAMETER   参数错误 
 ************************************************************************************
*/
int32_t I2C_MasterSend(I2C_Handle *i2c,uint32_t addr, const uint8_t *data, uint32_t num);

/**
 ************************************************************************************
 * @brief           master执行读传输
 *
 * @param[in]       i2c            I2C控制器句柄
 * @param[in]       addr           设备地址
 * @param[in]      data          接收数据地址
 * @param[in]      num           接收数据长度
 * 
 * @return          int32_t
 *                  DRV_ERR_BUSY        设备忙
 *                  DRV_OK              执行成功
 *                  DRV_ERR_PARAMETER   参数错误 
 ************************************************************************************
*/
int32_t I2C_MasterReceive(I2C_Handle *i2c,uint32_t addr, uint8_t *data, uint32_t num);

/**
 ************************************************************************************
 * @brief           slave执行写传输
 *
 * @param[in]       i2c            I2C控制器句柄
 * @param[in]       addr           设备地址
 * @param[in]      data            发送数据地址
 * @param[in]      num             发送数据长度
 * 
 * @return          int32_t
 *                  DRV_ERR_BUSY        设备忙
 *                  DRV_OK              执行成功
 *                  DRV_ERR_PARAMETER   参数错误 
 ************************************************************************************
*/
int32_t I2C_SlaveSend(I2C_Handle *i2c, const uint8_t *data, uint32_t num);

/**
 ************************************************************************************
 * @brief           slave执行读传输
 *
 * @param[in]       i2c            I2C控制器句柄
 * @param[in]      data          接收数据地址
 * @param[in]      num           接收数据长度
 * 
 * @return          int32_t
 *                  DRV_ERR_BUSY        设备忙
 *                  DRV_OK              执行成功
 *                  DRV_ERR_PARAMETER   参数错误 
 ************************************************************************************
*/
int32_t I2C_SlaveReceive(I2C_Handle *i2c, uint8_t *data, uint32_t num);

/**
 ************************************************************************************
 * @brief           master执行组合传输
 *
 * @param[in]       i2c            I2C控制器句柄
 * @param[in]       addr           设备地址
 * @param[in]      txData          发送数据地址
 * @param[in]      txNum           发送数据长度
 * @param[in]      rxData          接收数据地址
 * @param[in]      rxNum           接收数据长度
 * @param[in]      master          控制器模式（master or slave）
 * 
 * @return          int32_t
 *                  DRV_ERR_BUSY        设备忙
 *                  DRV_OK              执行成功
 *                  DRV_ERR_PARAMETER   参数错误 
 ************************************************************************************
*/
int32_t I2C_MasterTransfer(I2C_Handle *i2c, uint32_t addr, uint8_t *txData, uint32_t txNum, uint8_t *rxData, uint32_t rxNum);

/**
 ************************************************************************************
 * @brief           获取I2C控制器收或发数据的数量.
 *
 * @param[in]       i2c             I2C控制器句柄
 * 
 * @return          int32_t         
 * @retval          >=0             收取或者发送的字节数
 ************************************************************************************
 */
int32_t I2C_GetDataCount(I2C_Handle *i2c);

/**
 ************************************************************************************
 * @brief           控制I2C控制器.
 *
 * @param[in]       UART            I2C控制器句柄
 * @param[in]       control         控制命令
 * @param[in]       arg             控制命令参数
 * 
 * @return          执行返回值.
 * @retval          ==DRV_OK        执行成功
 *                  ==DRV_ERR_UNSUPPORTED  不支持的命令
 ************************************************************************************
 */
int32_t I2C_Control(I2C_Handle *i2c, uint32_t control, uint32_t arg);
/**
 ************************************************************************************
 * @brief           获取I2C控制器状态
 *
 * @param[in]       i2c            I2C控制器句柄
 *
 * @return          I2C控制器状态.
 * @retval          I2C_STATUS
 ************************************************************************************
 */
I2C_STATUS I2C_GetStatus(I2C_Handle *i2c);

/**
 ************************************************************************************
 * @brief           软复位I2C控制器
 *
 * @param[in]       i2c            I2C控制器句柄
 *
 * @return          void
 ************************************************************************************
 */
void I2C_SoftReset(I2C_Handle *i2c);

#endif /* DRIVER_I2C_H_ */
/** @} */
