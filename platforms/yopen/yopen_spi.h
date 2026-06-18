
/**  @file
  ycom_spi.h

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
                
#ifndef YOPEN_SPI_H
#define YOPEN_SPI_H
#include "yopen_api_common.h"


#ifdef __cplusplus
extern "C" {
#endif
#include "yopen_gpio.h"

/**
 * @defgroup yopen_spi spi功能
 * @{
*/

/*========================================================================
*  Enumeration Definition
*========================================================================*/
/**
 * @brief spi接口错误码
 * 
 */
typedef enum
{
	YOPEN_SPI_SET_CB_ERR               =  -1,
	YOPEN_SPI_SUCCESS                  =   0,
    
    YOPEN_SPI_ERROR                    =   1 | (YOPEN_COMPONENT_BSP_SPI << 16),   //SPI总线其他错误
    YOPEN_SPI_PARAM_TYPE_ERROR,                        //参数类型错误
    YOPEN_SPI_PARAM_DATA_ERROR,                        //参数数据错误
    YOPEN_SPI_PARAM_ACQUIRE_ERROR,                     //参数无法获取
    YOPEN_SPI_PARAM_NULL_ERROR,                        //参数NULL错误
    YOPEN_SPI_DEV_NOT_ACQUIRE_ERROR,                   //无法获取SPI总线
    YOPEN_SPI_PARAM_LENGTH_ERROR,                      //参数长度错误
    YOPEN_SPI_MALLOC_MEM_ERROR,                        //申请内存错误
    YOPEN_SPI_ADDR_ALIGNED_ERROR,                      //地址不是4字节对齐
    YOPEN_SPI_MUTEX_CREATE_ERROR,                      //互斥锁创建失败报错
    YOPEN_SPI_MUTEX_LOCK_ERROR,                        //互斥锁上锁超时报错
    YOPEN_SPI_UNKNOWN_ERROR,
}yopen_errcode_spi_e;

/**
 * @brief spi片选
 * 
 */
typedef enum
{
    YOPEN_SPI_CS0 = 0,                                 //选择cs0为SPI片选CS引脚
    YOPEN_SPI_CS1,                                     //选择cs1为SPI片选CS引脚
    YOPEN_SPI_CS2,                                     //选择cs2为SPI片选CS引脚,not use now
    YOPEN_SPI_CS3,                                     //选择cs3为SPI片选CS引脚,not use now
}yopen_spi_cs_sel_e;

/**
 * @brief spi端口号
 * 
 */
typedef enum
{
    YOPEN_SPI_PORT0,                                   //SPI0总线
#ifdef TYPE_EC718M
    YOPEN_SPI_PORT1,                                   //SPI1总线
#endif
    YOPEN_SPI_PORT_MAX,                                //SPI总线最大数量
}yopen_spi_port_e;

/**
 * @brief spi时钟极性
 * 
 */
typedef enum
{
    YOPEN_SPI_CPOL_LOW = 0,                            //SPI未使能时，CLK线为低电平，第一个边沿是上升沿
    YOPEN_SPI_CPOL_HIGH,                               //SPI未使能时，CLK线为高电平，第一个边沿是下降沿
} yopen_spi_cpol_pol_e;

/**
 * @brief spi时钟相位
 * 
 */
typedef enum
{
    YOPEN_SPI_CPHA_1Edge,                              //MOSI延时一个边沿,CLK和MISO延时两个边沿，即发送的数据先准备好，才有CLK
    YOPEN_SPI_CPHA_2Edge,                              //MOSI延时两个边沿，CLK延时2个边沿，MISO延时3个边沿。数据和CLK同时准备好
}yopen_spi_cpha_pol_e;

/**
 * @brief spi传输模式
 * 
 */
typedef enum
{
    YOPEN_SPI_DIRECT_POLLING = 0,                      //MASTER  FIFO读写，轮询等待。
    YOPEN_SPI_DIRECT_IRQ,                              //MASTER  FIFO读写，中断通知,not use now
    YOPEN_SPI_DMA_IRQ,                                 //MASTER  DMA读写， 中断通知
    YOPEN_SPI_SLAVE_DIRECT_POLLING,                    //SLAVE   FIFO读写，轮询等待。
    YOPEN_SPI_SLAVE_DIRECT_IRQ,                        //SLAVE   FIFO读写，中断通知,not use now
    YOPEN_SPI_SLAVE_DMA_IRQ,                           //SLAVE   DMA读写， 中断通知
}yopen_spi_transfer_mode_e;

/**
 * @brief spi时钟频率
 * 
 */
typedef enum
{	
	YOPEN_SPI_CLK_INVALID=-1,                      //无效时钟选择
	YOPEN_SPI_CLK_812_5KHZ = 812500,           	//时钟：812.5K
	YOPEN_SPI_CLK_1_625MHZ = 1625000,              //时钟：1.625M
	YOPEN_SPI_CLK_3_25MHZ = 3250000,               //时钟：3.125M
	YOPEN_SPI_CLK_6_5MHZ = 6500000,                //时钟：6.5M
	YOPEN_SPI_CLK_13MHZ = 13000000,                //时钟：13M
}yopen_spi_clk_e;

/**
 * @brief spi触发中断阈值
 * 
 */
typedef enum
{
    YOPEN_SPI_TRIGGER_1_DATA,                      //FIFO有1个字节，触发中断
    YOPEN_SPI_TRIGGER_4_DATA,                      //FIFO有4个字节，触发中断
    YOPEN_SPI_TRIGGER_8_DATA,                      //FIFO有8个字节，触发中断
    YOPEN_SPI_TRIGGER_12_DATA,                     //FIFO有12个字节，触发中断
}yopen_spi_threshold_e;

/**
 * @brief spi配置结构体
 * 
 */
typedef struct
{
    yopen_spi_port_e port;
    unsigned int framesize;
    yopen_spi_clk_e spiclk;
    yopen_spi_cpol_pol_e cpol;
    yopen_spi_cpha_pol_e cpha;
    yopen_spi_transfer_mode_e transmode;   
} yopen_spi_config_s;


/**
 * @brief spi中断结构体
 * 
 */
typedef struct
{
    unsigned int rx_ovf : 1;
    unsigned int tx_th : 1;
    unsigned int tx_dma_done : 1;
    unsigned int rx_th : 1;
    unsigned int rx_dma_done : 1;
    yopen_spi_threshold_e tx_threshold;
    yopen_spi_threshold_e rx_threshold;
}yopen_spi_irq_s;

/*========================================================================
 *	Function Definition
 *========================================================================*/
/**
 * @brief spi初始化
 * 
 * @param spi_config spi配置
 * @return ** yopen_errcode_spi_e 
 */
extern yopen_errcode_spi_e yopen_spi_init(yopen_spi_config_s spi_config);


/**
 * @brief spi传输
 * 
 * @param port spi端口号
 * @param inbuf spi接收缓冲区
 * @param outbuf spi发送缓冲区
 * @param len spi传输长度
 * @return ** yopen_errcode_spi_e 
 */
extern yopen_errcode_spi_e yopen_spi_write_read(yopen_spi_port_e port, unsigned char *inbuf, unsigned char *outbuf, unsigned int len);


/**
 * @brief spi接收
 * 
 * @param port spi端口号
 * @param buf 接收缓冲区
 * @param len 接收数据长度
 * @return ** yopen_errcode_spi_e 
 */
extern yopen_errcode_spi_e yopen_spi_read(yopen_spi_port_e port, unsigned char *buf, unsigned int len);


/**
 * @brief spi发送
 * 
 * @param port spi端口号
 * @param buf 发送缓冲区
 * @param len 发送数据长度
 * @return ** yopen_errcode_spi_e 
 */
extern yopen_errcode_spi_e yopen_spi_write(yopen_spi_port_e port, unsigned char *buf, unsigned int len);


/**
 * @brief spi去初始化
 * 
 * @param port spi端口号
 * @return ** yopen_errcode_spi_e 
 */
extern yopen_errcode_spi_e yopen_spi_release(yopen_spi_port_e port);


typedef void (*yopen_spi_callback)(int event);
extern yopen_errcode_spi_e yopen_spi_callback_set(yopen_spi_port_e port, yopen_spi_callback cb);

/**
 * @brief spi从模式获取读数据长度
 * @param port spi端口号
 * @return ** int 返回长度
 */
int yopen_spi_slaver_get_data_count(yopen_spi_port_e port);
/** @}*/

#ifdef __cplusplus
} /*"C" */
#endif

#endif /* YOPEN_SPI_H */


