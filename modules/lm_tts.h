/**
 * @file iot_tts.h
 * @brief 语音合成
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_TTS_H
#define IOT_TTS_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_tts(lua_State* L);

#endif /* IOT_TTS_H */
