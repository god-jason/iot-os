
/**  @file
  demo_tts.c

  @brief
  TODO

*/

#include <stdio.h>
#include <string.h>
#include "yopen_app_main.h"
#include "yopen_debug.h"
#include "yopen_os.h"
#include "mem_map.h"
#include "yopen_tts.h"

extern void demo_tts_init(void);
extern void demo_tts_play(char *test, int len);
extern bool demo_tts_is_running(void);
extern void demo_tts_stop(void);

void tts_demo_thread(void *argv)
{
    char tts_buf1[] = "你好，我是TTS语音播报演示程序";
    char tts_buf2[] = "1234567890";

    yopen_rtos_task_sleep_ms(2000);
    
    demo_tts_init();

    while(1)
    {
      //正常播放
      demo_tts_play(tts_buf1, strlen(tts_buf1));
      while(demo_tts_is_running());
      yopen_rtos_task_sleep_ms(1000);

      //播放200ms后关闭
      demo_tts_play(tts_buf2, strlen(tts_buf2));
      

      while(demo_tts_is_running());

      demo_tts_stop();

      yopen_rtos_task_sleep_ms(1000);
    }
}
