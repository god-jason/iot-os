/*
 * TTS module stub when CM_SERVICE_TTS_ENABLE is off.
 */
#include "lua.h"
#include "iot_base.h"
#include "iot_tts.h"

static int tts_unavailable(lua_State *L)
{
    (void)L;
    return luaL_error(L, "tts not enabled on this build");
}

static const luaL_Reg tts_lib[] = {
    { "say",   tts_unavailable },
    { "stop",  tts_unavailable },
    { "state", tts_unavailable },
    { NULL, NULL }
};

LUAMOD_API int luaopen_tts(lua_State *L)
{
    luaL_newlibtable(L, tts_lib);
    luaL_setfuncs(L, tts_lib, 0);
    return 1;
}
