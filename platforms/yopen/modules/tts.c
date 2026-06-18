/*
@module  tts
@summary 文字转语音
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     AUDIO
*/

/*
TTS参考示例
-- 初始化TTS
tts.init()

-- 播放文字
tts.play("你好，世界")

-- 停止播放
tts.stop()

-- 暂停/恢复
tts.pause()
tts.resume()

-- 设置参数
tts.setVolume(80)
tts.setSpeed(50)
*/

#include "module.h"
#include "yopen_tts.h"

/* TTS状态 */
#define TTS_STATE_IDLE    0
#define TTS_STATE_PLAYING 1
#define TTS_STATE_PAUSED  2

/* TTS句柄 */
static yopen_tts_h g_tts_handle = 0;
static int g_tts_state = TTS_STATE_IDLE;

/**
 * @brief 初始化TTS
 * @api tts.init()
 * @return bool true表示成功
 */
static int luaopen_tts_init(lua_State* L) {
    if (g_tts_handle != 0) {
        lua_pushboolean(L, 1);
        return 1;
    }

    g_tts_handle = yopen_tts_create();
    if (g_tts_handle != 0) {
        yopen_tts_init(g_tts_handle);
        g_tts_state = TTS_STATE_IDLE;
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 播放文字
 * @api tts.play(text)
 * @string text 要播放的文字
 * @return bool true表示成功
 */
static int luaopen_tts_play(lua_State* L) {
    const char* text = luaL_checkstring(L, 1);

    if (!text || g_tts_handle == 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = yopen_tts_play(g_tts_handle, (char*)text);
    if (ret == 0) {
        g_tts_state = TTS_STATE_PLAYING;
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 停止播放
 * @api tts.stop()
 * @return nil
 */
static int luaopen_tts_stop(lua_State* L) {
    if (g_tts_handle != 0) {
        yopen_tts_stop(g_tts_handle);
        g_tts_state = TTS_STATE_IDLE;
    }
    return 0;
}

/**
 * @brief 暂停播放
 * @api tts.pause()
 * @return bool true表示成功
 */
static int luaopen_tts_pause(lua_State* L) {
    if (g_tts_handle != 0 && g_tts_state == TTS_STATE_PLAYING) {
        int ret = yopen_tts_pause(g_tts_handle);
        if (ret == 0) {
            g_tts_state = TTS_STATE_PAUSED;
            lua_pushboolean(L, 1);
        } else {
            lua_pushboolean(L, 0);
        }
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 恢复播放
 * @api tts.resume()
 * @return bool true表示成功
 */
static int luaopen_tts_resume(lua_State* L) {
    if (g_tts_handle != 0 && g_tts_state == TTS_STATE_PAUSED) {
        int ret = yopen_tts_resume(g_tts_handle);
        if (ret == 0) {
            g_tts_state = TTS_STATE_PLAYING;
            lua_pushboolean(L, 1);
        } else {
            lua_pushboolean(L, 0);
        }
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 设置音量
 * @api tts.setVolume(volume)
 * @int volume 音量 (0-100)
 * @return bool true表示成功
 */
static int luaopen_tts_setvolume(lua_State* L) {
    int volume = (int)luaL_checkinteger(L, 1);

    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;

    if (g_tts_handle != 0) {
        yopen_tts_set_volume(g_tts_handle, volume);
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 设置语速
 * @api tts.setSpeed(speed)
 * @int speed 语速 (0-100)
 * @return bool true表示成功
 */
static int luaopen_tts_setspeed(lua_State* L) {
    int speed = (int)luaL_checkinteger(L, 1);

    if (speed < 0) speed = 0;
    if (speed > 100) speed = 100;

    if (g_tts_handle != 0) {
        yopen_tts_set_speed(g_tts_handle, speed);
        lua_pushboolean(L, 1);
    } else {
        lua_pushboolean(L, 0);
    }
    return 1;
}

/**
 * @brief 获取TTS状态
 * @api tts.getState()
 * @return int TTS状态 (0=空闲, 1=播放中, 2=暂停)
 */
static int luaopen_tts_getstate(lua_State* L) {
    lua_pushinteger(L, g_tts_state);
    return 1;
}

static const luaL_Reg luaopen_tts_funcs[] = {
    {"init",      luaopen_tts_init},
    {"play",      luaopen_tts_play},
    {"stop",      luaopen_tts_stop},
    {"pause",     luaopen_tts_pause},
    {"resume",    luaopen_tts_resume},
    {"setVolume", luaopen_tts_setvolume},
    {"setSpeed",  luaopen_tts_setspeed},
    {"getState",  luaopen_tts_getstate},
    {NULL, NULL}
};

int luaopen_tts(lua_State* L) {
    luaL_newlib(L, luaopen_tts_funcs);

    /* TTS状态常量 */
    lua_pushinteger(L, TTS_STATE_IDLE);
    lua_setfield(L, -2, "IDLE");
    lua_pushinteger(L, TTS_STATE_PLAYING);
    lua_setfield(L, -2, "PLAYING");
    lua_pushinteger(L, TTS_STATE_PAUSED);
    lua_setfield(L, -2, "PAUSED");

    return 1;
}