#ifndef cm_audio_amr_decode_h
#define cm_audio_amr_decode_h
#include "cm_audio_common.h"
#include "cm_common_api.h"

#ifdef AMR_DEBUG_LOG_SHELL
#define AMR_LOG(fmt, ...)    osPrintf("[AMR]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define AMR_LOG_E(fmt, ...)  osPrintf("[AMR_E]%s %u:" fmt "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define AMR_LOG(fmt, ...)    app_printf("[AMR]%s %u:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define AMR_LOG_E(fmt, ...)  app_printf("[AMR_E]%s %u:" fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#endif


typedef struct
{
    VFS_File* fp;
    uint32_t file_pos;
    uint8_t audio_type;
    uint8_t *p_pcm_dec;
    uint32_t pcm_frame_size;
    void * amr_handle;
    int bfi;
    uint32_t amr_decsize;
    uint32_t index;
    int is_initialezed;
}cm_amr_context;

uint32_t amr_read_pcm_frames_s16(void *amrHandle, uint32_t framesToRead, uint8_t* pBufferOut);
int amr_release(void);
void decoder_release(void* state);
int cm_auido_amr_decoder_init(void **decoder, VFS_File *fd, uint8_t *dataBits, uint32_t *sampleRate, uint8_t *chnls);

#endif  /* dr_flac_c */
