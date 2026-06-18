/**
 * @file        cm_audio_private.h
 * @brief       Audio 私有接口
 * @copyright   Copyright © 2021 China Mobile IOT. All rights reserved.
 * @author      By WP0201
 * @date        2021/4/12
 *
 * @defgroup audio_private
 * @ingroup audio_private
 * @{
 */

#ifndef __OPENCPU_AUDIO_PRIVATE_H__
#define __OPENCPU_AUDIO_PRIVATE_H__
/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

typedef void (*cm_audio_hw_driver_cb)(void *arg);

int32_t cm_audio_hw_driver_init(cm_audio_sample_param_t *sample_param, cm_audio_hw_driver_cb cb, uint8_t cntx_type);

void cm_audio_hw_driver_unint(void);

int32_t cm_audio_hw_driver_start(uint32_t bufferSize);

void cm_audio_hw_driver_stop(void);

int32_t cm_audio_hw_driver_insertData(const uint8_t *data, uint32_t dataSize);

cm_audio_play_driver_e cm_audio_player_get_driver();

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif
/** @}*/