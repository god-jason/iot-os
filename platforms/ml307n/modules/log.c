/*
@module  log
@summary 日志打印
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     LOG
*/

/*
LOG参考示例
-- 打印不同级别日志
log.trace("trace message")
log.info("info message")
log.warn("warn message")
log.error("error message")
*/


#include "module.h"

/* 日志级别 */
#define LOG_LEVEL_DEBUG  0
#define LOG_LEVEL_TRACE  1
#define LOG_LEVEL_INFO   2
#define LOG_LEVEL_WARN   3
#define LOG_LEVEL_ERROR  4

/* 日志级别前缀 */
static const char* log_level_prefix[] = {
    "[D]",
    "[T]",
    "[I]",
    "[W]",
    "[E]"
};

/**
 * @brief 内部日志打印函数
 */
static void luaopen_log_output(lua_State* L, int level) {
    cm_log_printf(0, "%s ", log_level_prefix[level]);

    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        if (lua_isstring(L, i)) {
            size_t len;
            const char* str = lua_tolstring(L, i, &len);
            cm_log_printf(0, "%s", str);
        } else if (lua_isnumber(L, i)) {
            if (lua_isinteger(L, i)) {
                int64_t v = lua_tointeger(L, i);
                cm_log_printf(0, "%lld", (long long)v);
            } else {
                double v = lua_tonumber(L, i);
                cm_log_printf(0, "%f", v);
            }
        } else if (lua_isboolean(L, i)) {
            int v = lua_toboolean(L, i);
            cm_log_printf(0, "%s", v ? "true" : "false");
        } else if (lua_isnil(L, i)) {
            cm_log_printf(0, "%s", "nil");
        } else if (lua_istable(L, i)) {
            lua_pushvalue(L, i);
            lua_pushfstring(L, "table: %p", lua_topointer(L, -1));
            const char* str = lua_tostring(L, -1);
            cm_log_printf(0, "%s", str);
            lua_pop(L, 1);
        } else {
            lua_pushfstring(L, "%s: %p", luaL_typename(L, i), lua_topointer(L, i));
            const char* str = lua_tostring(L, -1);
            cm_log_printf(0, "%s", str);
            lua_pop(L, 1);
        }
        if (i < n) {
            cm_log_printf(0, "%s", " ");
        }
    }
    cm_log_printf(0, "%s", "\r\n");
}

/**
 * @brief 打印debug级别日志
 * @api log.debug(...)
 * @param ... 要打印的内容
 * @return nil
 * @usage
log.debug("debug message")
*/
static int luaopen_log_debug(lua_State* L) {
    luaopen_log_output(L, LOG_LEVEL_DEBUG);
    return 0;
}

/**
 * @brief 打印trace级别日志
 * @api log.trace(...)
 * @param ... 要打印的内容
 * @return nil
 * @usage
log.trace("trace message")
*/
static int luaopen_log_trace(lua_State* L) {
    luaopen_log_output(L, LOG_LEVEL_TRACE);
    return 0;
}

/**
 * @brief 打印info级别日志
 * @api log.info(...)
 * @param ... 要打印的内容
 * @return nil
 * @usage
log.info("info message")
*/
static int luaopen_log_info(lua_State* L) {
    luaopen_log_output(L, LOG_LEVEL_INFO);
    return 0;
}

/**
 * @brief 打印warn级别日志
 * @api log.warn(...)
 * @param ... 要打印的内容
 * @return nil
 * @usage
log.warn("warn message")
*/
static int luaopen_log_warn(lua_State* L) {
    luaopen_log_output(L, LOG_LEVEL_WARN);
    return 0;
}

/**
 * @brief 打印error级别日志
 * @api log.error(...)
 * @param ... 要打印的内容
 * @return nil
 * @usage
log.error("error message")
*/
static int luaopen_log_error(lua_State* L) {
    luaopen_log_output(L, LOG_LEVEL_ERROR);
    return 0;
}

static const luaL_Reg luaopen_log_lib[] = {
    { "trace",  luaopen_log_trace },
    { "info",   luaopen_log_info },
    { "warn",   luaopen_log_warn },
    { "error",  luaopen_log_error },
    { NULL, NULL }
};

LUAMOD_API int luaopen_log_register(lua_State* L) {
    luaL_newlibtable(L, luaopen_log_lib);
    luaL_setfuncs(L, luaopen_log_lib, 0);
    return 1;
}