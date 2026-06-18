/*
@module  audio
@summary 音频播放/录音模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     AUDIO
*/

/*
Audio参考示例:
local audio = require 'audio'

-- 初始化音频
audio.init()

-- 播放文件
audio.play('/data/test.wav', function(event) end)

-- 播放PCM数据
audio.play_pcm({rate=16000, bits=16, channels=1, data=...})

-- 暂停/恢复
audio.pause()
audio.resume()

-- 停止
audio.stop()

-- 设置音量
audio.set_volume(80)

-- 获取音量
local vol = audio.get_volume()

-- 开始录音
audio.record_start({format='pcm', rate=16000, bits=16, channels=1}, function(event, data) end)

-- 停止录音
audio.record_stop()

-- 反初始化
audio.deinit()
*/


#include "lua.h"
#include "iot_base.h"
#include "cm_audio_player.h"
#include "cm_audio_recorder.h"
#include "iot_rtos.h"

/* 录音回调参数 */
typedef struct {
    void *callback_ud;
} audio_record_ctx_t;

/* 录音上下文 */
static audio_record_ctx_t *g_record_ctx = NULL;

/**
 * @brief 解析文件格式
 */
static cm_audio_play_format_e parse_file_format(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return CM_AUDIO_PLAY_FORMAT_PCM;

    if (strcmp(ext, ".wav") == 0 || strcmp(ext, ".WAV") == 0) {
        return CM_AUDIO_PLAY_FORMAT_WAV;
    } else if (strcmp(ext, ".mp3") == 0 || strcmp(ext, ".MP3") == 0) {
        return CM_AUDIO_PLAY_FORMAT_MP3;
    } else if (strcmp(ext, ".amr") == 0 || strcmp(ext, ".AMR") == 0) {
        return CM_AUDIO_PLAY_FORMAT_AMRNB;
    }
    return CM_AUDIO_PLAY_FORMAT_PCM;
}

/**
 * @brief 播放文件回调
 */
static void audio_play_callback(cm_audio_play_event_e event, void *param) {
    /* 此处可根据需要添加事件通知到 Lua */
}

/**
 * @brief 初始化音频
 * @api audio.init()
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_init(lua_State* L) {
    LOG("init");
    cm_audio_init();
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 反初始化音频
 * @api audio.deinit()
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_deinit(lua_State* L) {
    LOG("deinit");
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 播放音频文件
 * @api audio.play(path, callback, loop)
 * @param path string 音频文件路径
 * @param callback function|nil 播放回调
 * @param loop bool|nil 是否循环播放（暂不支持）
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_play(lua_State* L) {
    size_t len;
    const char *path = lua_tolstring(L, 1, &len);
    if (!path) {
        lua_pushboolean(L, 0);
        return 1;
    }

    LOG("play path=%s", path);
    cm_audio_play_format_e format = parse_file_format(path);
    int32_t ret = cm_audio_play_file(path, format, NULL, audio_play_callback, NULL);
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 播放PCM数据
 * @api audio.play_pcm(data, rate, bits, channels)
 * @param data string PCM数据
 * @param rate number 采样率（默认16000）
 * @param bits number 位深（默认16）
 * @param channels number 声道数（默认1）
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_play_pcm(lua_State* L) {
    size_t len;
    const char *data = lua_tolstring(L, 1, &len);
    if (!data || len == 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int rate = lua_isnumber(L, 2) ? (int)lua_tointeger(L, 2) : 16000;
    int bits = lua_isnumber(L, 3) ? (int)lua_tointeger(L, 3) : 16;
    int channels = lua_isnumber(L, 4) ? (int)lua_tointeger(L, 4) : 1;

    LOG("play_pcm len=%u rate=%d bits=%d channels=%d", len, rate, bits, channels);

    cm_audio_sample_param_t sample_param = {
        .sample_format = (bits == 8) ? CM_AUDIO_SAMPLE_FORMAT_8BIT : CM_AUDIO_SAMPLE_FORMAT_16BIT,
        .rate = (cm_audio_sample_rate_e)rate,
        .num_channels = (cm_audio_sound_channel_e)channels
    };

    int32_t ret = cm_audio_player_stream_open(CM_AUDIO_PLAY_FORMAT_PCM, &sample_param);
    if (ret != 0) {
        LOG("ERR open ret=%d", ret);
        lua_pushboolean(L, 0);
        return 1;
    }

    ret = cm_audio_player_stream_push((uint8_t*)data, (uint32_t)len);
    if (ret < 0) {
        LOG("ERR push ret=%d", ret);
    }
    lua_pushboolean(L, ret >= 0);
    return 1;
}

/**
 * @brief 暂停播放
 * @api audio.pause()
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_pause(lua_State* L) {
    LOG("pause");
    int32_t ret = cm_audio_player_pause();
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 恢复播放
 * @api audio.resume()
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_resume(lua_State* L) {
    LOG("resume");
    int32_t ret = cm_audio_player_resume();
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 停止播放
 * @api audio.stop()
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_stop(lua_State* L) {
    LOG("stop");
    int32_t ret = cm_audio_player_stop();
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 设置音量
 * @api audio.set_volume(volume)
 * @param volume number 音量值（0-100）
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_set_volume(lua_State* L) {
    int vol = (int)lua_tointeger(L, 1);
    if (vol < 0) vol = 0;
    if (vol > 100) vol = 100;

    LOG("set_volume vol=%d", vol);
    int ret = cm_audio_player_set_volume(vol);
    if (ret != 0) {
        LOG("ERR ret=%d", ret);
    }
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 获取音量
 * @api audio.get_volume()
 * @return number 音量值（0-100）
 */
static int iot_audio_get_volume(lua_State* L) {
    int vol = 80;
    cm_audio_player_get_volume(&vol);
    lua_pushinteger(L, vol);
    return 1;
}

/**
 * @brief 录音回调
 */
static void audio_record_callback(cm_audio_record_event_e event, void *param) {
    if (!g_record_ctx || !g_record_ctx->callback_ud) return;

    params_t* params = params_create(2);
    if (!params) {
        LOG("ERR params_create failed");
        return;
    }

    params_push_int(params, event);

    if (event == CM_AUDIO_RECORD_EVENT_DATA) {
        cm_audio_record_data_t *record_data = (cm_audio_record_data_t*)param;
        params_push_string(params, (const char*)record_data->data, record_data->len);
    } else {
        params_push_nil(params);
    }

    iot_rtos_call(g_record_ctx->callback_ud, params);
}

/**
 * @brief 开始录音
 * @api audio.record_start(config, callback)
 * @param config table|nil 录音配置
 * @param callback function|nil 录音回调
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_record_start(lua_State* L) {
    cm_audio_record_format_e format = CM_AUDIO_RECORD_FORMAT_PCM;
    cm_audio_sample_param_t sample_param = {
        .sample_format = CM_AUDIO_SAMPLE_FORMAT_16BIT,
        .rate = CM_AUDIO_SAMPLE_RATE_16000HZ,
        .num_channels = CM_AUDIO_SOUND_MONO
    };

    /* 解析配置参数 */
    if (lua_istable(L, 1)) {
        lua_getfield(L, 1, "format");
        if (lua_isstring(L, -1)) {
            const char *fmt = lua_tostring(L, -1);
            if (strcmp(fmt, "amr") == 0) {
                format = CM_AUDIO_RECORD_FORMAT_AMRNB_475;
            } else if (strcmp(fmt, "wav") == 0) {
                format = CM_AUDIO_RECORD_FORMAT_WAVPCM;
            }
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "rate");
        if (lua_isnumber(L, -1)) {
            sample_param.rate = (cm_audio_sample_rate_e)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "bits");
        if (lua_isnumber(L, -1)) {
            sample_param.sample_format = ((int)lua_tointeger(L, -1) == 8) ?
                CM_AUDIO_SAMPLE_FORMAT_8BIT : CM_AUDIO_SAMPLE_FORMAT_16BIT;
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "channels");
        if (lua_isnumber(L, -1)) {
            sample_param.num_channels = ((int)lua_tointeger(L, -1) == 2) ?
                CM_AUDIO_SOUND_STEREO : CM_AUDIO_SOUND_MONO;
        }
        lua_pop(L, 1);
    }

    LOG("record_start rate=%d", sample_param.rate);

    /* 处理回调 */
    if (lua_isfunction(L, 2)) {
        g_record_ctx = (audio_record_ctx_t*)malloc(sizeof(audio_record_ctx_t));
        if (!g_record_ctx) {
            LOG("ERR malloc failed");
            lua_pushboolean(L, 0);
            return 1;
        }
        
        void* ud = lua_newuserdata(L, 1);
        lua_pushvalue(L, 2);
        lua_setuservalue(L, -2);
        g_record_ctx->callback_ud = ud;

        lua_pushlightuserdata(L, ud);
        lua_pushvalue(L, -2);
        lua_settable(L, LUA_REGISTRYINDEX);

        lua_pop(L, 1);
    }

    int32_t ret = cm_audio_recorder_start(format, &sample_param,
        g_record_ctx ? audio_record_callback : NULL, NULL);

    if (ret != 0) {
        LOG("ERR ret=%d", ret);
        if (g_record_ctx) {
            lua_pushlightuserdata(L, g_record_ctx->callback_ud);
            lua_pushnil(L);
            lua_settable(L, LUA_REGISTRYINDEX);
            free(g_record_ctx);
            g_record_ctx = NULL;
        }
    }

    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 停止录音
 * @api audio.record_stop()
 * @return bool 成功返回true，失败返回false
 */
static int iot_audio_record_stop(lua_State* L) {
    LOG("record_stop");
    cm_audio_recorder_stop();

    if (g_record_ctx) {
        lua_pushlightuserdata(L, g_record_ctx->callback_ud);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
        free(g_record_ctx);
        g_record_ctx = NULL;
    }

    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg audio_lib[] = {
    { "init",             iot_audio_init },
    { "deinit",           iot_audio_deinit },
    { "play",             iot_audio_play },
    { "play_pcm",         iot_audio_play_pcm },
    { "pause",            iot_audio_pause },
    { "resume",           iot_audio_resume },
    { "stop",             iot_audio_stop },
    { "set_volume",       iot_audio_set_volume },
    { "get_volume",       iot_audio_get_volume },
    { "record_start",     iot_audio_record_start },
    { "record_stop",      iot_audio_record_stop },
    {NULL, NULL}
};

LUAMOD_API int luaopen_audio(lua_State* L) {
    luaL_newlibtable(L, audio_lib);
    luaL_setfuncs(L, audio_lib, 0);
    return 1;
}
