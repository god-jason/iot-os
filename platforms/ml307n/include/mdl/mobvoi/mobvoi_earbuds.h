#ifndef _MOBVOI_EARBUDS_H_
#define _MOBVOI_EARBUDS_H_

#ifdef __cplusplus
extern "C" {
#endif

enum {
  MOBVOI_KWS_MODE     = 0,
  MOBVOI_ASR_MODE     = 1,
  MOBVOI_CALL_MODE_WB = 2, // 16k sample rate output
  MOBVOI_CALL_MODE_NB = 3, // 8k sample rate output
  MOBVOI_MAX_MODE,
};

enum {
  MOBVOI_CMD_GET_AUDIO_QUALITY = 0,
  MOBVOI_CMD_GET_WIND_STATUS   = 1,
};


/**
 * Mobvoi library version get function.
 */
unsigned char* mobvoi_get_lib_version(void);

/**
 * Mobvoi library built time get function.
 */
unsigned char* mobvoi_get_lib_built_datetime(void);

/**
 * Mobvoi dsp license setting function for Studio
 * @param key: license key
 * @param len: length of license key
 */
void mobvoi_dsp_set_license(unsigned char* key, int len);

/**
 * Set a block of pre-alloced memory which will be used by mobvoi dsp.
 * @param mode: dsp work mode.
 * @param memory: The memory header pointer.
 * @param length: The memory length in bytes.
 */

/**
 * Mobvoi dsp check license function for Studio
 * @return  0:fail
 *          1:sucess
 */
int  mobvoi_dsp_check_license(void);


int mobvoi_dsp_set_memory(int mode, void* memory, int length);

/**
 * Get the size of memory needed by mobvoi dsp.
 * @param mode: dsp work mode.
 * @return result: the total memory size needed by dsp.
 */
unsigned int mobvoi_dsp_get_memory_needed(int mode);

/**
 * Mobvoi dsp uplink initialization function.
 * @param mode: dsp work mode.
 * @param sample_rate: support 8K, 16K
 * @param sample_num: the sample number of 1 frame, eg: 120 for 7.5ms, 64 for 4ms.
 * @param nr_level: the noise reduction level, eg:1,2,3.
 * @param drc_gain: the gain of DRC algorithm.
 * @return The dsp processor instance.
 */
void* mobvoi_dsp_uplink_init(int mode,
                             int sample_rate,
							 int sample_num,
							 int nr_level,
							 int drc_gain);

/**
 * Mobvoi dsp uplink process function.
 * @param instance: The dsp processor instance.
 * @param stage_flag:
 *                    0 multi-stage process disabled;
 *                    1 1st stage process;
 *                    2 2nd stage process.
 * @param talk_mic: The talk_mic audio data.
 * @param enc_mic: The enc_mic audio data.
 * @param fb_mic: The fb_mic audio data.
 * @param bone_mic: The bone sensor data.
 * @param spk: The speaker audio data.
 * @param out: The processed audio data.
 * @return The detected command id. 0 means invalid command.
 */
int mobvoi_dsp_uplink_process(void *instance,
                              int stage_flag,
                              short *talk_mic,
                              short *enc_mic,
                              short *fb_mic,
                              short *bone_mic,
                              short *spk,
                              short *out);

/**
 * Mobvoi dsp uplink cleanup function.
 * @param instance: The dsp processor instance.
 */
void mobvoi_dsp_uplink_cleanup(void *instance);

/**
 * Set echo dealy, assuming speaker signal is always earlier.
 * @param instance: The dsp processor instance.
 * @param sample_num: The sample number of echo delay.
 */
void mobvoi_dsp_uplink_set_echo_delay(void* instance, int sample_num);

/**
 * Set echo dealy, assuming speaker signal is always earlier.
 * @param instance: The dsp processor instance.
 * @param cmd: The command index.
 * @param result: The result of the command.
 */
void mobvoi_dsp_uplink_cmd(void* instance, int cmd, void* result);



#ifdef __cplusplus
}
#endif

#endif  // _MOBVOI_EARBUDS_H_

