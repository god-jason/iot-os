/*
@module  log
@summary 日志打印
@version 1.0
@date    2026.06.19
@tag     LOG
*/

#include "module.h"

#define LOG_LEVEL_DEBUG  0
#define LOG_LEVEL_TRACE  1
#define LOG_LEVEL_INFO   2
#define LOG_LEVEL_WARN   3
#define LOG_LEVEL_ERROR  4

static const char* log_level_prefix[] = {
    "[D]",
    "[T]",
    "[I]",
    "[W]",
    "[E]"
};

static void iot_log_output(lua_State* L, int level) {
    printf("%s ", log_level_prefix[level]);

    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            size_t len;
            const char* str = lua_tolstring(L, i, &len);
            printf("%s", str);
        } else if (lua_isnumber(L, i)) {
            if (lua_isinteger(L, i)) {
                int64_t v = lua_tointeger(L, i);
                printf("%lld", (long long)v);
            } else {
                double v = lua_tonumber(L, i);
                printf("%f", v);
            }
        } else if (lua_isboolean(L, i)) {
            int v = lua_toboolean(L, i);
            printf("%s", v ? "true" : "false");
        } else if (lua_isnil(L, i)) {
            printf("%s", "nil");
        } else if (lua_istable(L, i)) {
            lua_pushvalue(L, i);
            lua_pushfstring(L, "table: %p", lua_topointer(L, -1));
            const char* str = lua_tostring(L, -1);
            printf("%s", str);
            lua_pop(L, 1);
        } else {
            lua_pushfstring(L, "%s: %p", luaL_typename(L, i), lua_topointer(L, i));
            const char* str = lua_tostring(L, -1);
            printf("%s", str);
            lua_pop(L, 1);
        }
        if (i < n) {
            printf("%s", " ");
        }
    }
    printf("%s", "\n");
}

static int iot_log_debug(lua_State* L) {
    iot_log_output(L, LOG_LEVEL_DEBUG);
    return 0;
}

static int iot_log_trace(lua_State* L) {
    iot_log_output(L, LOG_LEVEL_TRACE);
    return 0;
}

static int iot_log_info(lua_State* L) {
    iot_log_output(L, LOG_LEVEL_INFO);
    return 0;
}

static int iot_log_warn(lua_State* L) {
    iot_log_output(L, LOG_LEVEL_WARN);
    return 0;
}

static int iot_log_error(lua_State* L) {
    iot_log_output(L, LOG_LEVEL_ERROR);
    return 0;
}

static const luaL_Reg log_lib[] = {
    { "trace",  iot_log_trace },
    { "info",   iot_log_info },
    { "warn",   iot_log_warn },
    { "error",  iot_log_error },
    { NULL, NULL }
};

LUAMOD_API int luaopen_log(lua_State* L) {
    luaL_newlibtable(L, log_lib);
    luaL_setfuncs(L, log_lib, 0);
    return 1;
}
