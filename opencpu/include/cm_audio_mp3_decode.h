#ifndef cm_audio_mp3_decode_h
#define cm_audio_mp3_decode_h
#include "cm_audio_common.h"
#include "cm_common_api.h"
#include "mp3dec.h"

#define CM_MP3_MAX_FRAME_SIZE                  (4096 + 1024)

typedef struct
{
    VFS_File* fp;
    uint32_t file_pos;
    uint8_t audio_type;
    uint8_t *p_pcm_dec;
    uint32_t pcm_frame_size;
    void * mp3_handle;
    int bfi;
    uint32_t mp3_decsize;
    uint32_t index;
    int is_initialezed;
}cm_mp3_context;

HMP3Decoder cm_audio_mp3_init(VFS_File *fd, uint8_t *dataBits, uint32_t *sampleRate, uint8_t *chnls);
void cm_audio_mp2_deinit(HMP3Decoder *mp3Handle);
uint32_t get_mp3_frame_size(void);
uint32_t mp3_read_pcm_data(HMP3Decoder mp3Handle, uint8_t* pBufferOut);

#endif
