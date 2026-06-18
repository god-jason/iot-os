/**
 * @file iot_i2s.h
 * @brief ML307N 平台 I2S(集成电路内置音频总线)适配器头文件
 * @details ML307N 平台当前版本暂未提供独立 I2S 驱动接口。
 *          音频相关功能请使用 iot_audio.h 中封装的接口。
 *          本文件保留统一接口占位，如需I2S功能请关注后续平台SDK更新。
 */

#ifndef IOT_I2S_ML307N_H
#define IOT_I2S_ML307N_H

#include "../../iot_types.h"

/** @brief I2S未在该平台实现，相关接口暂不可用 */
#define IOT_I2S_NOT_SUPPORTED   (-1)

#endif /* IOT_I2S_ML307N_H */
