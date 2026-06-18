/******************************************************************************
 * Shell test
 *****************************************************************************/

#include <nr_micro_shell.h>
#include "cm_local_tts.h"

// static const char *txt = "你好，这里是奥特曼基地";
// static const char *txt = "我梦想有一天，这个国家会站立起来，真正实现其信条的真谛：“我们认为这些真理是不言而喻的;人人生而平等";
static const char *txt = "123456789, 分段3，已标记，59分1秒，1小时1分9秒。你已经跑步5公里，最近1公里耗时6分40秒，平均心率145，平均配速5分30秒。很棒，加油 加油 加油！下一段，放松。下一个动作，原地高抬腿。前方左转。前方到达CP1点。请加速。减脂中，继续保持。你已完成训练，请放松一下吧。准备开始，3 2 1，go!";

/* "123456789，你好，GBK编码"*/
static const char gbk_txt[]={0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0xa3,0xac,
0xc4,0xe3,0xba,0xc3,0xa3,0xac,0x47,0x42,0x4b,0xb1,0xe0,0xc2,0xeb};
/* "你好，这里是奥特曼基地"*/
static const char gbk_txt2[]={0xc4,0xe3,0xba,0xc3,0xa3,0xac,0xd5,0xe2,0xc0,0xef,0xca,0xc7,0xb0,0xc2,0xcc,0xd8,0xc2,0xfc,0xbb,0xf9,0xb5,0xd8,0};

static void tts_play_cb(uint32_t eventID)
{
    app_printf("eventID %d", eventID);

    if (eventID == CM_LOCAL_TTS_EVENT_PLAY_FINISH) {
        app_printf("tts play end");
    }
    if (eventID == CM_LOCAL_TTS_EVENT_PLAY_INTERRUPT) {
        app_printf("tts play interrupt");
    }
    if (eventID == CM_LOCAL_TTS_EVENT_PLAY_FAIL) {
        app_printf("tts play fail");
    }
}

void shell_local_tts_test(char argc, char **argv)
{
    int ret;
    cm_local_tts_cfg_t cfg = {
        .speed = 1,
        .volume = 1,
        .encode = CM_LOCAL_TTS_ENCODE_TYPE_UTF8,
        .digit = CM_LOCAL_TTS_DIGIT_AUTO,
        .tone = 0,
        .effect = 0
    };

    /*play GBK text*/
    os_kprintf("\r\nplay GBK text\r\n");
    cfg.encode = CM_LOCAL_TTS_ENCODE_TYPE_GB2312;
    cm_local_tts_init(&cfg);
    cm_local_tts_play(gbk_txt2, 22, tts_play_cb, NULL);
    while (cm_local_tts_get_state() == CM_LOCAL_TTS_STATE_WORKING) {
        cm_delay(1000);
    }
    cm_local_tts_deinit();
    cfg.encode = CM_LOCAL_TTS_ENCODE_TYPE_UTF8;

    os_kprintf("\r\ntts play utf-8 text top 100bytes\r\n");
    ret = cm_local_tts_init(&cfg);
    if(ret < 0)
    {
        os_kprintf("\r\ntts cm_local_tts_init err: %d\r\n", ret);
        return;
    }
    ret = cm_local_tts_play(txt, 100, NULL, NULL);
    if(ret < 0)
    {
        os_kprintf("\r\ntts cm_local_tts_play err: %d\r\n", ret);
    }

    while (cm_local_tts_get_state() == CM_LOCAL_TTS_STATE_WORKING) {
        cm_delay(1000);
    }

    cm_local_tts_deinit();

    os_kprintf("\r\ntts play utf-8 text all bytes\r\n");
    cfg.speed = 5;
    cfg.volume = 5;
    cfg.digit = CM_LOCAL_TTS_DIGIT_AS_VALUE;
    cm_local_tts_init(&cfg);

    cm_local_tts_play(txt, -1, NULL, NULL);

    while (cm_local_tts_get_state() == CM_LOCAL_TTS_STATE_WORKING) {
        cm_delay(1000);
    }

    cm_local_tts_deinit();

    os_kprintf("\r\ntts play utf-8 text then stop palying.\r\n");
    cfg.speed = 9;
    cfg.volume = 9;
    cfg.digit = CM_LOCAL_TTS_DIGIT_AS_NUMBER;
    cm_local_tts_init(&cfg);
    cm_local_tts_play(txt, -1, tts_play_cb, NULL);
    cm_local_tts_play(txt, -1, tts_play_cb, NULL);

    cm_local_tts_state_e state = cm_local_tts_get_state();
    os_kprintf("\r\ntts state: %d\r\n", state);

    cm_delay(15000);

    ret = cm_local_tts_play_stop();
    if(ret < 0)
    {
        os_kprintf("\r\ntts cm_local_tts_play_stop err: %d\r\n", ret);
    }
    state = cm_local_tts_get_state();
    os_kprintf("\r\ntts state: %d\r\n", state);

    cm_delay(1000);
    state = cm_local_tts_get_state();
    os_kprintf("\r\ntts state: %d\r\n", state);

    cm_local_tts_deinit();

    os_kprintf("\r\nsuccess\r\n");
}

NR_SHELL_CMD_EXPORT(local_tts_test, shell_local_tts_test);

