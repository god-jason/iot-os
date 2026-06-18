/*
 * Audio module stub when CM_SERVICE_AUDIO_ENABLE is off (e.g. ml307n-dc gateway).
 */
#include "lua.h"
#include "iot_base.h"
#include "iot_audio.h"

static int audio_unavailable(lua_State *L)
{
    (void)L;
    return luaL_error(L, "audio not enabled on this build");
}

static const luaL_Reg audio_lib[] = {
    { "init",         audio_unavailable },
    { "deinit",       audio_unavailable },
    { "play",         audio_unavailable },
    { "play_pcm",     audio_unavailable },
    { "pause",        audio_unavailable },
    { "resume",       audio_unavailable },
    { "stop",         audio_unavailable },
    { "set_volume",   audio_unavailable },
    { "get_volume",   audio_unavailable },
    { "record_start", audio_unavailable },
    { "record_stop",  audio_unavailable },
    { NULL, NULL }
};

LUAMOD_API int luaopen_audio(lua_State *L)
{
    luaL_newlibtable(L, audio_lib);
    luaL_setfuncs(L, audio_lib, 0);
    return 1;
}
