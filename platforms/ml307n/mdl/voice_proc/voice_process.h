

#ifndef __AUDIO_PROC__
#define __AUDIO_PROC__

#include "stdint.h"
#include "stddef.h"

void VoiceProc_Initialize(uint16_t sampleRate, int nrLevel);

void VoiceProc_UnInitialize(void);

void VoiceProc_TxProc(uint8_t *frameBuf, uint16_t frameSize);

void VoiceProc_RxProc(uint8_t *frameBuf, uint16_t frameSize);

void VoiceProc_SendMsg(uint32_t bufAddr);
#endif
