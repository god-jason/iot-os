#ifndef cm_audio_wav_decode_h
#define cm_audio_wav_decode_h
#include "cm_audio_common.h"
#include "cm_common_api.h"
#include "wav.h"

int cm_audio_wav_init(VFS_File *fd, uint8_t *dataBits, uint32_t *sampleRate, uint8_t *chnls);

#endif
