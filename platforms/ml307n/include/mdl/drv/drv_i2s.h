/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        drv_i2s.h
 *
 * @brief       I2S驱动接口.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */

#ifndef _DRV_I2S_H
#define _DRV_I2S_H

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>

/**
 * @addtogroup I2s
 */

/**@{*/

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#define             I2S_FRAME_BUF_NUM        3

/**
 * @brief 回调事件类型
 */
#define             I2S_RX_CB_EVENT         1
#define             I2S_TX_CB_EVENT         2
/**
 * @brief 总线工作状态
 */
#define             I2S_IDLE                0
#define             I2S_BUSY                1
#define             I2S_ERR                 2
/**
 * @brief 总线工作时序
 */
#define             TIMING_I2S              0
#define             TIMING_PCM              1
/**
 * @brief 传输时序
 */
#define             TIMING_I2S_MSB_JUSTIF   0
#define             TIMING_I2S_LSB_JUSTIF   1
#define             TIMING_I2S_STANDARD     2
#define             TIMING_I2S_NORMAL       3
#define             TIMING_PCM_MSB_JUSTIF   4
#define             TIMING_PCM_LSB_JUSTIF   5
#define             TIMING_PCM_STANDARD     6
/**
 * @brief 单个采样点数据宽度
 */
#define             DATA_8_BITS             8
#define             DATA_16_BITS            16
#define             DATA_24_BITS            24
#define             DATA_32_BITS            32
/**
 * @brief 工作模式
 */
#define             I2S_PLAY_MODE           1
#define             I2S_RECORD_MODE         2
/**
 * @brief 通道类型
 */
#define             I2S_RIGHT_CHNL          1
#define             I2S_LEFT_CHNL           2
#define             I2S_DOUBLE_CHNL         3

/**
 * @brief MCLK 与 采样率之间的倍数
 */
#define             I2S_64FS                 64
#define             I2S_128FS               128
#define             I2S_256FS               256
#define             I2S_512FS               512

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

/**
 * @brief PCM 时序配置
 */
typedef struct 
{
    uint8_t slotNum;        /** slot 个数 */
    uint8_t slotCycle;      /** 单个slot 占用的时钟周期 */
    uint8_t dataBits;       /** 数据位大小 */
    uint8_t transTiming;    /** 传输时序*/
}PCM_TimingCfg;

/**
 * @brief I2S 时序配置
 */
typedef struct 
{
    uint8_t chnl;           /** 通道个数 */
    uint8_t dataCycle;      /** 数据 */
    uint8_t dataBits;       /** 数据位大小 */
    uint8_t transTiming;    /** 传输时序*/
}I2S_TimingCfg;

/** 
 * @brief 总线配置
 */
typedef struct 
{
    uint8_t         workMode;       /* 工作模式，播放或录音 */
    uint8_t         timing;         /* 时序 */
    uint16_t        frameSize;      /* 帧缓存大小, 单个接受或发送的帧大小 */
    uint32_t        sampleRate;     /* 采样率 */
    uint16_t        fs;
    void *          timingCfg;      /* 时序详细配置 */
    bool            isSlave;        /* 是否为 slave 模式*/
}I2S_BusCfg;

/**
 * @brief I2S 回调类型
 */
typedef void (*I2SCallBack) (uint32_t event);  

/**
 * @brief 后处理算法回调
 * 
 */
typedef struct 
{
    void (*init)(uint16_t, int);
    void (*deinit)(void);
    void (*rx_proc_notify)(uint32_t);
    void (*tx_proc)(uint8_t *, uint16_t);
}VoiceProcFunc;

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/

/**
 ************************************************************************************
 * @brief           I2S总线初始化
 *
 * @param[in]       cfg                     配置参数
 * @param[in]       cb                      回调函数
 * @param[in]       voiceProc               后处理函数
 * 
 * @retval          0                       成功                           
 *                  <0                      失败
 ************************************************************************************
 */
int8_t I2S_Initialize(I2S_BusCfg *cfg, I2SCallBack cb, VoiceProcFunc *voiceProc);

/**
 ************************************************************************************
 * @brief           I2S总线去初始化
 *
 * @param[in]       void
 * 
 * @return          返回值.
 * @retval          0                       成功                           
 *                  <0                      失败
 ************************************************************************************
 */
void I2S_UnInitialize(void);
/**
 ************************************************************************************
 * @brief           写数据到I2S总线
 *
 * @param[in]       buf                      存放数据buffer
 * @param[in]       size                     写数据个数, 写入大小需要为帧大小
 * @param[in]       timeOut                  超时时间，跟随系统定义
 * 
 * @return          返回值.
 * @retval          0                       成功                           
 *                  <0                      失败
 ************************************************************************************
 */
int32_t I2S_BusWrite(const uint8_t *buf, uint32_t size, uint32_t timeOut);

/**
 ************************************************************************************
 * @brief           从I2S总线读数据
 *
 * @param[in]       buf                      存放数据buffer
 * @param[in]       size                     预读数据个数
*  @param[in]       timeOut                  超时时间，跟随系统定义
 * 
 * @return          返回值.
 * @retval          >0                       成功,实际写入的字节数                           
 *                  <0                       失败
 ************************************************************************************
 */
int32_t I2S_BusRead(uint8_t *buf, uint32_t size, uint32_t timeOut);

/**
 ************************************************************************************
 * @brief           I2S开始播放或录音
 * 
 * @param[in]       mode                      模式：播放或录音
 * 
 * @return          返回值.
 * @retval          >0                      成功                          
 *                  <0                      失败
 ************************************************************************************
 */
int8_t I2S_BusStart(uint8_t mode);

/**
 ************************************************************************************
 * @brief           I2S停止播放或录音
 * 
 * @param[in]       mode                      模式：播放或录音
 * 
 * @return          返回值.
 * @retval          >0                      成功                           
 *                  <0                      失败
 ************************************************************************************
 */
int8_t I2S_BusStop(uint8_t mode);

/**
 ************************************************************************************
 * @brief           获取空闲Buffer个数
 * 
 * @param[in]       mode                      模式：播放或录音
 * 
 * @return          Buffer 个数.
 ************************************************************************************
 */
uint8_t I2S_BusGetAvaliableBufCount(uint8_t mode);

/**
 ************************************************************************************
 * @brief           获取总线状态
 * 
 * @param[in]       mode                      模式：播放或录音
 * 
 * @return          总线状态（I2S_IDLE I2S_BUSY).
 ************************************************************************************
 */
uint8_t I2S_BusGetState(uint8_t mode);

/**
 ************************************************************************************
 * @brief           I2S RX RingBuf 增加, 封装给后处理使用，所有音频帧必须后处理后才允许上层调用
 * 
 * @param[in]      
 * 
 * @return          总线状态（I2S_IDLE I2S_BUSY).
 ************************************************************************************
 */
void I2S_RxRingBufInc(void);
#endif
/** @} */
