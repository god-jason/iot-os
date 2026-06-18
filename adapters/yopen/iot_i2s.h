/**
 * @file iot_i2s.h
 * @brief YOPEN 平台 I2S 音频接口适配器头文件
 * @details 提供 I2S 数字音频接口功能封装。
 * @note 暂未支持，保留接口占位
 */

#ifndef IOT_I2S_YOPEN_H
#define IOT_I2S_YOPEN_H

#include "../../iot_types.h"

/* ============================================================
 * I2S 类型定义
 * ============================================================ */

/** @brief I2S采样率 */
typedef enum {
    IOT_I2S_SAMPLE_RATE_8K = 8000,       /**< 8kHz */
    IOT_I2S_SAMPLE_RATE_16K = 16000,     /**< 16kHz */
    IOT_I2S_SAMPLE_RATE_44K = 44100,     /**< 44.1kHz */
    IOT_I2S_SAMPLE_RATE_48K = 48000,     /**< 48kHz */
} iot_i2s_sample_rate_t;

/** @brief I2S字长 */
typedef enum {
    IOT_I2S_WORD_LENGTH_16BIT = 16,     /**< 16位 */
    IOT_I2S_WORD_LENGTH_24BIT = 24,     /**< 24位 */
    IOT_I2S_WORD_LENGTH_32BIT = 32,     /**< 32位 */
} iot_i2s_word_length_t;

/** @brief I2S通道 */
typedef enum {
    IOT_I2S_CHANNEL_MONO = 1,           /**< 单声道 */
    IOT_I2S_CHANNEL_STEREO = 2,         /**< 立体声 */
} iot_i2s_channel_t;

/** @brief I2S句柄类型 */
typedef void* iot_i2s_handle_t;

/* ============================================================
 * I2S 接口（暂未支持）
 * ============================================================ */

/**
 * @brief 打开I2S
 * @param[in] sample_rate 采样率
 * @param[in] word_length 字长
 * @param[in] channel 通道数
 * @return I2S句柄，失败返回NULL
 */
iot_i2s_handle_t iot_i2s_open(iot_i2s_sample_rate_t sample_rate,
                                iot_i2s_word_length_t word_length,
                                iot_i2s_channel_t channel);

/**
 * @brief 关闭I2S
 * @param[in] handle I2S句柄
 * @return 0 成功，<0 失败
 */
int32_t iot_i2s_close(iot_i2s_handle_t handle);

/**
 * @brief 发送数据
 * @param[in] handle I2S句柄
 * @param[in] data 数据指针
 * @param[in] len 数据长度(字节)
 * @return >=0 成功发送的字节数，<0 失败
 */
int32_t iot_i2s_send(iot_i2s_handle_t handle, const uint8_t *data, uint32_t len);

/**
 * @brief 接收数据
 * @param[in] handle I2S句柄
 * @param[out] data 数据缓冲区
 * @param[in] len 缓冲区大小
 * @return >=0 成功接收的字节数，<0 失败
 */
int32_t iot_i2s_recv(iot_i2s_handle_t handle, uint8_t *data, uint32_t len);

#endif /* IOT_I2S_YOPEN_H */
