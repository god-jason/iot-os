/*
@module  tts
@summary 文字转语音模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     TTS
*/

/*
TTS参考示例:
local tts = require 'tts'

-- 播放文字
tts.say('你好，世界')

-- 设置参数
tts.say('你好', 7, 8, 0)

-- 停止播放
tts.stop()

-- 获取状态
local state = tts.state()
print('TTS状态:', state)
*/


#include "lua.h"
#include "iot_base.h"
#include "cm_local_tts.h"

/* TTS初始化标志 */
static int g_tts_initialized = 0;

/**
 * @brief 播放文字
 * @api tts.say(text, speed, volume, digit)
 * @param text string 要播放的文字
 * @param speed number|nil 语速（1-9，默认5）
 * @param volume number|nil 音量（1-9，默认5）
 * @param digit number|nil 数字播放方式（0自动，1数字，2数值）
 * @return bool 成功返回true，失败返回false
 */
static int iot_tts_say(lua_State* L) {
    size_t len;
    const char *text = lua_tolstring(L, 1, &len);
    if (!text) {
        lua_pushboolean(L, 0);
        return 1;
    }

    cm_local_tts_cfg_t cfg = {
        .speed = lua_isnumber(L, 2) ? (int32_t)lua_tointeger(L, 2) : 5,
        .volume = lua_isnumber(L, 3) ? (int32_t)lua_tointeger(L, 3) : 5,
        .encode = CM_LOCAL_TTS_ENCODE_TYPE_UTF8,
        .digit = lua_isnumber(L, 4) ?
            ((int32_t)lua_tointeger(L, 4) == 1 ? CM_LOCAL_TTS_DIGIT_AS_NUMBER :
             (int32_t)lua_tointeger(L, 4) == 2 ? CM_LOCAL_TTS_DIGIT_AS_VALUE : CM_LOCAL_TTS_DIGIT_AUTO) :
            CM_LOCAL_TTS_DIGIT_AUTO,
        .tone = CM_LOCAL_TTS_TONE_NORMAL,
        .effect = CM_LOCAL_TTS_EFFECT_NORMAL
    };

    if (!g_tts_initialized) {
        LOG("auto init");
        int32_t ret = cm_local_tts_init(&cfg);
        if (ret != 0) {
            LOG("ERR init ret=%d", ret);
            lua_pushboolean(L, 0);
            return 1;
        }
        g_tts_initialized = 1;
    }

    LOG("say len=%u", len);
    int32_t ret = cm_local_tts_play(text, -1, NULL, NULL);
    if (ret != 0) {
        LOG("ERR play ret=%d", ret);
    }
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 停止TTS播放
 * @api tts.stop()
 * @return bool 成功返回true，失败返回false
 */
static int iot_tts_stop(lua_State* L) {
    LOG("stop");
    int32_t ret = cm_local_tts_play_stop();
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 获取TTS状态
 * @api tts.state()
 * @return number 状态（1空闲，2工作中）
 */
static int iot_tts_state(lua_State* L) {
    cm_local_tts_state_e state = cm_local_tts_get_state();
    lua_pushinteger(L, state);
    return 1;
}

/**
 * @brief 初始化TTS
 * @api tts.init(speed, volume, digit)
 * @param speed number|nil 语速（1-9，默认5）
 * @param volume number|nil 音量（1-9，默认5）
 * @param digit number|nil 数字播放方式（0自动，1数字，2数值）
 * @return bool 成功返回true，失败返回false
 */
static int iot_tts_init(lua_State* L) {
    cm_local_tts_cfg_t cfg = {
        .speed = lua_isnumber(L, 1) ? (int32_t)lua_tointeger(L, 1) : 5,
        .volume = lua_isnumber(L, 2) ? (int32_t)lua_tointeger(L, 2) : 5,
        .encode = CM_LOCAL_TTS_ENCODE_TYPE_UTF8,
        .digit = lua_isnumber(L, 3) ?
            ((int32_t)lua_tointeger(L, 3) == 1 ? CM_LOCAL_TTS_DIGIT_AS_NUMBER :
             (int32_t)lua_tointeger(L, 3) == 2 ? CM_LOCAL_TTS_DIGIT_AS_VALUE : CM_LOCAL_TTS_DIGIT_AUTO) :
            CM_LOCAL_TTS_DIGIT_AUTO,
        .tone = CM_LOCAL_TTS_TONE_NORMAL,
        .effect = CM_LOCAL_TTS_EFFECT_NORMAL
    };

    LOG("init speed=%d volume=%d", cfg.speed, cfg.volume);
    
    if (g_tts_initialized) {
        cm_local_tts_deinit();
    }

    int32_t ret = cm_local_tts_init(&cfg);
    if (ret == 0) {
        g_tts_initialized = 1;
        LOG("OK");
    } else {
        LOG("ERR ret=%d", ret);
    }
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 反初始化TTS
 * @api tts.deinit()
 * @return bool 成功返回true，失败返回false
 */
static int iot_tts_deinit(lua_State* L) {
    LOG("deinit");
    if (g_tts_initialized) {
        cm_local_tts_deinit();
        g_tts_initialized = 0;
    }
    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg tts_lib[] = {
    { "init",    iot_tts_init },
    { "deinit",  iot_tts_deinit },
    { "say",     iot_tts_say },
    { "stop",    iot_tts_stop },
    { "state",   iot_tts_state },
    {NULL, NULL}
};

LUAMOD_API int luaopen_tts(lua_State* L) {
    luaL_newlibtable(L, tts_lib);
    luaL_setfuncs(L, tts_lib, 0);
    return 1;
}
