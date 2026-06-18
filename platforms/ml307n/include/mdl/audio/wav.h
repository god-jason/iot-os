#ifndef __WAV_H__
#define __WAV_H__
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
typedef struct 
{
    // RIFF
    uint32_t ChunkID;
    uint32_t ChunkSize;
    uint32_t Format;

    //fmt
    uint32_t SubChunk1ID;
    uint32_t SubChunk1Size;
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    //data
    uint32_t SubChunk2ID;
    uint32_t SubChunk2Size;
}WavFileHeader;

#define     WAV_HEADER_SIZE     sizeof(WavFileHeader)

int8_t WAV_Check(uint8_t *wavFile, uint32_t size);
int8_t WAV_GetBitsOfPerSample(uint8_t *wavFile, uint32_t size, uint8_t *BitsPerSample);
int8_t WAV_GetSampleRate(uint8_t *wavFile, uint32_t size, uint32_t *sampleRate);
int8_t WAV_CreateHeader(uint8_t *wavFile, uint32_t size, uint32_t sampleRate, uint16_t dataBits, uint16_t chnls);
int8_t WAV_GetChnls(uint8_t *wavFile, uint32_t size, uint8_t *chnls);
int32_t WAV_FindDataChunk(uint8_t *data, uint32_t size);
#endif
