/**
 * @file iot_audio.h
 * @brief 音频播放
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_AUDIO_H
#define IOT_AUDIO_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_audio(lua_State* L);

#endif /* IOT_AUDIO_H */
