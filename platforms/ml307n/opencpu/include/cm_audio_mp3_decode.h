#ifndef cm_audio_mp3_decode_h
#define cm_audio_mp3_decode_h
#include "cm_audio_common.h"
#include "mp3dec.h"
#include "vfs.h"
#include "os.h"

#define CM_MP3_MAX_FRAME_SIZE                  (4096 + 1024)
#define CM_AUDIO_READ_MP3_FILE_SIZE            4096

typedef struct
{
    VFS_File* fp;
    uint8_t *buff;
    uint32_t buff_len;
    uint32_t read_pos;
    uint8_t audio_type;
    uint8_t *p_pcm_dec;
    uint32_t pcm_frame_size;
    void * mp3_handle;
    int bfi;
    uint32_t mp3_decsize;
    uint32_t index;
    int is_initialezed;
}cm_mp3_context;

HMP3Decoder cm_audio_mp3_file_init(VFS_File *fd, uint8_t *dataBits, uint32_t *sampleRate, uint8_t *chnls);
HMP3Decoder cm_audio_mp3_buff_init(uint8_t *buffer, uint32_t buffer_size, uint8_t *dataBits, uint32_t *sampleRate, uint8_t *chnls);
HMP3Decoder cm_audio_mp3_stream_init(uint8_t *buffer, uint32_t data_len, uint8_t *dataBits, uint32_t *sampleRate, uint8_t *chnls);

void cm_audio_mp3_deinit(HMP3Decoder *mp3Handle);
uint32_t get_mp3_frame_size(void);
uint32_t mp3_file_read_pcm_data(HMP3Decoder mp3Handle, uint8_t* pBufferOut);
uint32_t mp3_buff_read_pcm_data(HMP3Decoder p_mp3Handle, uint8_t* pBufferOut);
void mp3_buff_reset(uint32_t data_len);
uint32_t mp3_buff_get_data_len();
uint32_t mp3_buff_get_read_pos();

#endif
