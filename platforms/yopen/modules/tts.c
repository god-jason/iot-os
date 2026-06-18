/*
@module  tts
@summary 语音合成
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     TTS
*/

#include "module.h"
#include "yopen_tts.h"

static int luaopen_tts_speak(lua_State* L) {
    const char* text = luaL_checkstring(L, 1);
    lua_pushboolean(L, 0);
    return 1;
}

static int luaopen_tts_stop(lua_State* L) {
    return 0;
}

static const luaL_Reg luaopen_tts_funcs[] = {
    {"speak", luaopen_tts_speak},
    {"stop",  luaopen_tts_stop},
    {NULL, NULL}
};

int luaopen_tts(lua_State* L) {
    luaL_newlib(L, luaopen_tts_funcs);
    return 1;
}