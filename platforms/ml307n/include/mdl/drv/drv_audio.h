#ifndef __DRV_AUDIO_H_
#define __DRV_AUDIO_H_


typedef enum
{
    AUDIO_SAMPLE_RATE_8000 = 8000,
    AUDIO_SAMPLE_RATE_16000 = 16000,
    AUDIO_SAMPLE_RATE_44100 = 44100,
}AudioSampleRate;

typedef enum
{
    AUDIO_SAMPLE_BITS_8 = 8,
    AUDIO_SAMPLE_BITS_12 = 12,
    AUDIO_SAMPLE_BITS_16 = 16,
    AUDIO_SAMPLE_BITS_24 = 24,
    AUDIO_SAMPLE_BITS_32 = 32,
}AudioSampleBits;

typedef enum
{
    AUDIO_DC_OFFSET_16BIT = 32768,
    AUDIO_DC_OFFSET_8BIT = 128,
}AudioDcOffset;


#endif