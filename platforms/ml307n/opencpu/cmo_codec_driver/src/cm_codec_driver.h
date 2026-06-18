/*
 *************************************************************************************
 * @file        cm_codec_driver.h
 *
 * @brief       codec 驱动
 *
 * @revision    0.01
 *
 * 日期           作者              修改内容
 * 2025-4-15   zhouyang           创建
 ************************************************************************************
 */

#ifndef __CM_CODEC_DRIVER_H__
#define __CM_CODEC_DRIVER_H__

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <drv_common.h>
/************************************************************************************
 *                                 宏定义
 ************************************************************************************/

// #define CODEC_DRIVER_DEBUG_LOG_SHELL
#ifdef CODEC_DRIVER_DEBUG_LOG_SHELL

#define CD_LOG(fmt, ...)   osPrintf("[CD]%s %u: " fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define CD_LOG_E(fmt, ...) osPrintf("[CD_E]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define CD_LOG_RAW         osPrintf

#else

#define CD_LOG(fmt, ...)   app_printf("[CD]%s %u: " fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define CD_LOG_E(fmt, ...) app_printf("[CD_E]%s %u:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define CD_LOG_RAW         app_printf

#endif

#define CODEC_PLAY_MODE     1
#define CODEC_RECORD_MODE   2

/************************************************************************************
 *                                 类型定义
 ************************************************************************************/

// 暂未使用
typedef void (*cm_codec_callback)(uint32_t event);

/************************************************************************************
 *                                 函数定义
 ************************************************************************************/
void cm_codec_pinCfg(void *pin);

int8_t cm_codec_init(uint32_t sampleRate, uint8_t dataBits, uint16_t frameSize, cm_codec_callback cb);

int8_t cm_codec_init_with_mobvoi(uint32_t sampleRate, uint8_t dataBits, uint16_t frameSize, cm_codec_callback cb);

void cm_codec_uninit(void);

int8_t cm_codec_start(uint8_t mode);

void cm_codec_stop(uint8_t mode);

int8_t cm_codec_insertData(const uint8_t *data, uint32_t dataSize);

int8_t cm_codec_readData(uint8_t *data, uint32_t dataSize);

uint8_t cm_codec_getAvaliableBufCount(uint8_t mode);

int cm_codec_set_volume(int volume);

int cm_codec_get_volume(int *volume);

int cm_audio_pa_chip_set(uint8_t state);

#if defined(USE_MOBVOI_DSP)
void cm_audio_set_mobvoi(bool on_off);

bool cm_audio_get_mobvoi(void);
#endif
int8_t cm_codec_setMicGain(uint8_t *para, uint8_t len);
void cm_codec_get_param(uint32_t *sampleRate, uint8_t *dataBits, uint16_t *frameSize);

#endif // __CM_CODEC_DRIVER_H__


