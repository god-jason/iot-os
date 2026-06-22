/*
@module  log
@summary 日志模块
@version 1.0
@date    2026.06.20
@tag     LOG
*/

#include "module.h"
#include "esp_log.h"

static int luaopen_log_debug(lua_State* L) {
    const char* tag = luaL_checkstring(L, 1);
    const char* fmt = luaL_checkstring(L, 2);
    ESP_LOGD(tag, fmt, lua_gettop(L) > 2 ? lua_tostring(L, 3) : "");
    return 0;
}

static int luaopen_log_info(lua_State* L) {
    const char* tag = luaL_checkstring(L, 1);
    const char* fmt = luaL_checkstring(L, 2);
    ESP_LOGI(tag, fmt, lua_gettop(L) > 2 ? lua_tostring(L, 3) : "");
    return 0;
}

static int luaopen_log_warn(lua_State* L) {
    const char* tag = luaL_checkstring(L, 1);
    const char* fmt = luaL_checkstring(L, 2);
    ESP_LOGW(tag, fmt, lua_gettop(L) > 2 ? lua_tostring(L, 3) : "");
    return 0;
}

static int luaopen_log_error(lua_State* L) {
    const char* tag = luaL_checkstring(L, 1);
    const char* fmt = luaL_checkstring(L, 2);
    ESP_LOGE(tag, fmt, lua_gettop(L) > 2 ? lua_tostring(L, 3) : "");
    return 0;
}

static int luaopen_log_print(lua_State* L) {
    size_t len = 0;
    const char* str = luaL_checklstring(L, 1, &len);
    printf("%.*s\n", (int)len, str);
    return 0;
}

static const luaL_Reg luaopen_log_lib[] = {
    {"debug",    luaopen_log_debug},
    {"info",     luaopen_log_info},
    {"warn",     luaopen_log_warn},
    {"error",    luaopen_log_error},
    {"print",    luaopen_log_print},
    {NULL, NULL}
};

LUAMOD_API int luaopen_log_register(lua_State* L) {
    luaL_newlib(L, luaopen_log_lib);
    return 1;
}
