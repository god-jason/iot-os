
/**  @file
  yopen_pwmaud.h

  @brief
  TODO
  PWM音频播放接口
*/


/*=================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

WHEN              WHO         WHAT, WHERE, WHY
------------     -------     -------------------------------------------------------------------------------

=================================================================*/
                
#ifndef _YOPEN_PWMAUD_H_
#define _YOPEN_PWMAUD_H_

#include "yopen_osi_def.h"
#include "yopen_type.h"
#include "yopen_api_common.h"
#include "yopen_pwm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup yopen_pwmaud pwmaud功能
 * @{
*/


/*===========================================================================
 * Macro Definition
 ===========================================================================*/

 typedef enum
{
	YOPEN_PWMAUD_SAMPLERATE_8K,               ///< Sample rate 8k
    YOPEN_PWMAUD_SAMPLERATE_16K,              ///< Sample rate 16k
    YOPEN_PWMAUD_SAMPLERATE_22_05K,           ///< Sample rate 22.05k
    YOPEN_PWMAUD_SAMPLERATE_24K,	             ///< Sample rate 24k
    YOPEN_PWMAUD_SAMPLERATE_32K,              ///< Sample rate 32k
    YOPEN_PWMAUD_SAMPLERATE_44_1K,            ///< Sample rate 44.1k
    YOPEN_PWMAUD_SAMPLERATE_48K,              ///< Sample rate 48k
    YOPEN_PWMAUD_SAMPLERATE_96K,              ///< Sample rate 96k
} yopen_pwmaud_sample_rate_e;
 
//音量等级最大5
typedef enum
{
    YOPEN_PWMAUD_VOLUME_0,               ///< 音量等级0
    YOPEN_PWMAUD_VOLUME_1,               ///< 音量等级1
    YOPEN_PWMAUD_VOLUME_2,               ///< 音量等级2
    YOPEN_PWMAUD_VOLUME_3,               ///< 音量等级3
    YOPEN_PWMAUD_VOLUME_4,               ///< 音量等级4
    YOPEN_PWMAUD_VOLUME_5,               ///< 音量等级5
} yopen_pwmaud_volume_e;

/*
 * @brief 打开PWM音频播放
 * 
 * @param pwm_port PWM通道
 * @param sample_rate 采样率
 * @return yopen_errcode_pwm 
*/
int yopen_pwmaud_open(yopen_pwm_sel pwm_port, yopen_pwmaud_sample_rate_e sample_rate);
/*
 * @brief 写入PWM音频数据
 * 
 * @param data PWM音频数据
 * @param len PWM音频数据长度
 * @return yopen_errcode_pwm 
*/
int yopen_pwmaud_write(uint8_t *data, int len);

/*
 * @brief 关闭PWM音频播放
 * 
 * @return yopen_errcode_pwm 
*/
int yopen_pwmaud_close(void);


/**
 * @brief  设置PWM音频音量比例 
 * @param  volume 音量等级
*/
void yopen_pwmaud_setvolume(yopen_pwmaud_volume_e volume);

/**
 * @brief  获取PWM音频音量比例 
 * @return uint16_t: 返回换算后音量的比例
*/
uint16_t yopen_pwmaud_getvolume(void);


/** @}*/


#ifdef __cplusplus
} /*"C" */
#endif

#endif
