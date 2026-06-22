/*
@module  log
@summary 日志输出
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     LOG
*/

/*
log参考示例
-- 输出日志
log.info("tag", "hello world")
log.debug("tag", "debug message")
log.warn("tag", "warning message")
log.error("tag", "error message")
*/

#include "module.h"
#include "yopen_debug.h"

/**
 * @brief 输出info级别日志
 * @api log.info(tag, msg)
 * @string tag  日志标签
 * @string msg  日志消息
 * @return nil
 */
static int luaopen_log_info(lua_State* L) {
    const char* tag = luaL_checkstring(L, 1);
    const char* msg = luaL_checkstring(L, 2);
    iot_log_info(tag, "%s", msg);
    return 0;
}

/**
 * @brief 输出debug级别日志
 * @api log.debug(tag, msg)
 * @string tag  日志标签
 * @string msg  日志消息
 * @return nil
 */
static int luaopen_log_debug(lua_State* L) {
    const char* tag = luaL_checkstring(L, 1);
    const char* msg = luaL_checkstring(L, 2);
    iot_log_debug(tag, "%s", msg);
    return 0;
}

/**
 * @brief 输出warn级别日志
 * @api log.warn(tag, msg)
 * @string tag  日志标签
 * @string msg  日志消息
 * @return nil
 */
static int luaopen_log_warn(lua_State* L) {
    const char* tag = luaL_checkstring(L, 1);
    const char* msg = luaL_checkstring(L, 2);
    iot_log_warn(tag, "%s", msg);
    return 0;
}

/**
 * @brief 输出error级别日志
 * @api log.error(tag, msg)
 * @string tag  日志标签
 * @string msg  日志消息
 * @return nil
 */
static int luaopen_log_error(lua_State* L) {
    const char* tag = luaL_checkstring(L, 1);
    const char* msg = luaL_checkstring(L, 2);
    iot_log_error(tag, "%s", msg);
    return 0;
}

/**
 * @brief 格式化输出日志
 * @api log.printf(tag, fmt, ...)
 * @string tag  日志标签
 * @string fmt  格式化字符串
 * @return nil
 */
static int luaopen_log_printf(lua_State* L) {
    const char* tag = luaL_checkstring(L, 1);
    const char* fmt = luaL_checkstring(L, 2);
    /* 使用可变参数处理 */
    yopen_trace("[%s] %s\r\n", tag, fmt);
    return 0;
}

static const luaL_Reg luaopen_log_funcs[] = {
    {"info",   luaopen_log_info},
    {"debug",  luaopen_log_debug},
    {"warn",   luaopen_log_warn},
    {"error",  luaopen_log_error},
    {"printf", luaopen_log_printf},
    {NULL, NULL}
};

LUAMOD_API int luaopen_log_register(lua_State* L) {
    luaL_newlib(L, luaopen_log_funcs);
    return 1;
}