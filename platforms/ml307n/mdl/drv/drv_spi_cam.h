/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_timer.h
 *
 * @brief       Timer驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */
#ifndef __DRIVER_SPI_CAM_H__
#define __DRIVER_SPI_CAM_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include "drv_common.h"
#include <os.h>
#include <drv_dma.h>

/**
 * @addtogroup SpiCam
 */

/**@{*/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

/**
 * @brief 摄像头工作时序类型
 */
#define         CAMERA_MODE_BT656                   0
#define         CAMERA_MODE_MTK                     1

/**
 * @brief 接受纯图像数据或数据包
 */
#define         SAMPLE_IMAGE_DATA                   1
#define         SAMPLE_PACKET_DATA                  0

/**
 * @brief 流模式或者捕获
 */
#define         STREAMING_MODE                      1    
#define         CAPTURE_MODE                        0

/**
 * @brief 数据线个数
 */
#define         SPI_CAM_1_WIRE                      0x00
#define         SPI_CAM_2_WIRE                      0x01
#define         SPI_CAM_4_WIRE                      0x02

/**
 * @brief SPI 工作模式
 */
#define         SPI_TIMING_MODE_0                   0
#define         SPI_TIMING_MODE_1                   1
#define         SPI_TIMING_MODE_2                   2
#define         SPI_TIMING_MODE_3                   3

/**
 * @brief 回调事件类型
 */
#define         SPI_RX_EVENT                        0
#define         SPI_TX_EVENT                        1
/************************************************************************************
 *                                 类型定义
 ************************************************************************************/
typedef void (*SPICAM_SignalEvent) (void* data, uint32_t event);

typedef struct {
    osList_t node;
    uint8_t *data;
    uint32_t len;
    char     name[15];
    DMA_LliDesc  *dmaCfg;
    void *dmaParam;

    uint8_t using;
}SPICAM_ImageBuffer;

typedef struct 
{
    osList_t free;
    osList_t used;
    uint8_t  remain;
    uint8_t  max;
    uint16_t depth;

    SPICAM_ImageBuffer *array;
}SPICAM_Pool;

typedef struct 
{
    DMA_ChHandle *dmaHandlePtr;
    uint8_t userLLiNum;
    uint8_t loopCount;
    DMA_LliDesc *desclist;
}SpiDmaCtrl;

typedef struct 
{
    const struct CAM_Res   *res;
    void  *regs;
    SpiDmaCtrl rxCtrl;
    SPICAM_SignalEvent cbEvent;
    void *userData;
    bool err;
    uint32_t errNumber;
    void *priv;

    uint16_t imageWidth;
    uint16_t imageHeight;
    uint32_t wireNum;

    SPICAM_Pool  pool;

#define SPICAM_INIT         ((uint8_t)0x01)
#define SPICAM_POWER        ((uint8_t)0x02)
#define SPICAM_RUNNING      ((uint8_t)0x10)
    uint8_t                 ctrl;
}SPICAM_Handle;

enum{
    SPICAM_CONFIG_FRAME_WIDTH = 0,
    SPICAM_CONFIG_FREME_HEIGHT,
    SPICAM_CONFIG_FRAME_DEPTH,
    SPICAM_CONFIG_LINE_NUM,
    SPICAM_CONFIG_PACKET_SIZE,
};

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
void SPICAM_Start(SPICAM_Handle *hdl);
void SPICAM_Stop(SPICAM_Handle *hdl);
void SPICAM_ImageFree(SPICAM_Handle *hdl, uint8_t *addr);
uint8_t *SPICAM_ImageCapture(SPICAM_Handle *hdl);
int32_t SPICAM_Control(SPICAM_Handle *hdl, uint32_t control, uint32_t arg);
int32_t SPICAM_PowerControl(SPICAM_Handle *hdl, DRV_POWER_STATE state);
/**
 ************************************************************************************
 * @brief           SPI初始化.
 *
 * @param[in]       cfg         spi 相关配置
 * @param[in]       cb          回调函数
 *
 * @return          返回值.
 * @retval          ==0                     成功
 *                  < 0                     失败
 ************************************************************************************
 */
int32_t SPICAM_Initialize(SPICAM_Handle *hdl);

/**
 ************************************************************************************
 * @brief           SPI 传输数据. 
 *                  sendBuf != NULL, recvBuf == NULL 发送数据
 *                  sendBuf == NULL, recvBuf != NULL 接收数据
 *                  sendBuf != NULL, recvBuf != NULL 发送数据并接收数据
 * 
 * @param[in]       sendBuf         发送数据缓冲
 * @param[in]       sendSize        发送数据大小
 * @param[in]       recvBuf         接受数据缓冲
 * @param[in]       recvSize        接受数据大小
 *
 * @return          返回值.
 * @retval          ==0                     成功
 *                  < 0                     失败
 ************************************************************************************
 */
// int8_t SpiCam_Transfer(uint8_t *sendBuf, uint32_t sendSize, uint8_t *recvBuf, uint32_t *recvSize);

/**
 ************************************************************************************
 * @brief           SPI去初始化.资源释放
 *
 * @return          返回值.
 * @retval          ==0                     成功
 *                  < 0                     失败
 ************************************************************************************
 */
int8_t SPICAM_UnInitialize(SPICAM_Handle *hdl);
#endif


/** @} */