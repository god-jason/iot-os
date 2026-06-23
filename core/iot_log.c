/*
@module  log
@summary 日志打印
@version 1.0
@date    2026.06.20
@author  杰神 & TRAE & ChatGPT
*/

/*
LOG参考示例
-- 打印不同级别日志
log.debug("debug message")
log.trace("trace message")
log.info("info message")
log.warn("warn message")
log.error("error message")

-- 多参数打印
log.info("key", "=", "value", 123)
*/

#include "iot_base.h"
#include "iot_log.h"
#include "iot.h"

/* 当前日志级别，默认INFO */
static int s_log_level = LOG_LEVEL_INFO;

/* 日志级别前缀 */
static const char* log_level_prefix[] = {
    "[D]",
    "[T]",
    "[I]",
    "[W]",
    "[E]"
};

/**
 * @brief 设置日志级别
 * @param level 日志级别 (0-4)
 */
void iot_log_set_level(iot_log_level_t level) {
    if (level >= LOG_LEVEL_NONE && level <= LOG_LEVEL_DEBUG) {
        s_log_level = (int)level;
    }
}

/**
 * @brief 获取当前日志级别
 * @return 当前日志级别
 */
iot_log_level_t iot_log_get_level(void) {
    return (iot_log_level_t)s_log_level;
}

/**
 * @brief 内部日志输出函数
 * @param level 日志级别
 * @param tag 模块标签
 * @param fmt 格式字符串
 * @param ... 可变参数
 */
void iot_log_printf(iot_log_level_t level, const char* tag, const char* fmt, ...) {
    if (level > s_log_level) {
        return;
    }
    
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    int len = vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    
    if (len > 0 && len < (int)sizeof(buffer)) {
        iot_puts(buffer);
    }
}

/**
 * @brief 将Lua值转换为字符串并输出
 * @param L Lua状态机
 * @param idx 栈上的索引
 * @param buffer 输出缓冲区
 * @param buffer_size 缓冲区大小
 * @param first 是否是第一个参数
 */
static void value_to_string(lua_State* L, int idx, char* buffer, size_t buffer_size, int* first) {
    if (!buffer || buffer_size == 0) {
        return;
    }
    
    const char* prefix = *first ? "" : " ";
    *first = 0;
    
    if (lua_isstring(L, idx)) {
        size_t len;
        const char* str = lua_tolstring(L, idx, &len);
        if (len > buffer_size - 1) {
            len = buffer_size - 1;
        }
        snprintf(buffer, buffer_size, "%s%.*s", prefix, (int)len, str);
    } else if (lua_isnumber(L, idx)) {
        if (lua_isinteger(L, idx)) {
            int64_t v = lua_tointeger(L, idx);
            snprintf(buffer, buffer_size, "%s%lld", prefix, (long long)v);
        } else {
            double v = lua_tonumber(L, idx);
            snprintf(buffer, buffer_size, "%s%f", prefix, v);
        }
    } else if (lua_isboolean(L, idx)) {
        int v = lua_toboolean(L, idx);
        snprintf(buffer, buffer_size, "%s%s", prefix, v ? "true" : "false");
    } else if (lua_isnil(L, idx)) {
        snprintf(buffer, buffer_size, "%snil", prefix);
    } else if (lua_istable(L, idx)) {
        lua_pushvalue(L, idx);
        snprintf(buffer, buffer_size, "%stable: %p", prefix, lua_topointer(L, -1));
        lua_pop(L, 1);
    } else if (lua_isfunction(L, idx)) {
        lua_pushvalue(L, idx);
        snprintf(buffer, buffer_size, "%sfunction: %p", prefix, lua_topointer(L, -1));
        lua_pop(L, 1);
    } else if (lua_isuserdata(L, idx)) {
        lua_pushvalue(L, idx);
        snprintf(buffer, buffer_size, "%suserdata: %p", prefix, lua_topointer(L, -1));
        lua_pop(L, 1);
    } else if (lua_isthread(L, idx)) {
        lua_pushvalue(L, idx);
        snprintf(buffer, buffer_size, "%sthread: %p", prefix, lua_topointer(L, -1));
        lua_pop(L, 1);
    } else {
        snprintf(buffer, buffer_size, "%s%s", prefix, luaL_typename(L, idx));
    }
}

/**
 * @brief 内部日志打印函数，支持多参数
 */
static void iot_log_output(lua_State* L, int level) {
    if (level > s_log_level) {
        return;
    }
    
    /* 输出日志级别前缀 */
    iot_puts(log_level_prefix[level]);
    iot_puts(" ");
    
    int n = lua_gettop(L);
    int first = 1;
    char buffer[128];
    
    for (int i = 1; i <= n; i++) {
        buffer[0] = '\0';
        value_to_string(L, i, buffer, sizeof(buffer), &first);
        if (buffer[0] != '\0') {
            iot_puts(buffer);
        }
    }
    
    iot_puts("\r\n");
}

/**
 * @brief 打印debug级别日志
 * @api log.debug(...)
 * @param ... 要打印的内容
 * @return nil
 * @usage
log.debug("debug message")
log.debug("key", "=", "value", 123)
*/
static int luaopen_log_debug(lua_State* L) {
    iot_log_output(L, LOG_LEVEL_DEBUG);
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
    iot_log_output(L, LOG_LEVEL_TRACE);
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
    iot_log_output(L, LOG_LEVEL_INFO);
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
    iot_log_output(L, LOG_LEVEL_WARN);
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
    iot_log_output(L, LOG_LEVEL_ERROR);
    return 0;
}

/**
 * @brief 设置日志级别
 * @api log.level(level)
 * @param level 日志级别 0=NONE, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=TRACE
 * @return nil
 * @usage
log.level(log.INFO)
*/
static int luaopen_log_level(lua_State* L) {
    int level = (int)luaL_optinteger(L, 1, LOG_LEVEL_INFO);
    iot_log_set_level((iot_log_level_t)level);
    return 0;
}

/* Lua模块方法列表 */
static const luaL_Reg log_lib[] = {
    { "debug",  luaopen_log_debug },
    { "trace",  luaopen_log_trace },
    { "info",   luaopen_log_info },
    { "warn",   luaopen_log_warn },
    { "error",  luaopen_log_error },
    { "level",  luaopen_log_level },
    { NULL, NULL }
};

/**
 * @brief 日志级别常量
 */
static const luaL_Reg log_level_constants[] = {
    { "NONE",   NULL },
    { "ERROR",  NULL },
    { "WARN",   NULL },
    { "INFO",   NULL },
    { "DEBUG",  NULL },
    { "TRACE",  NULL },
    { NULL, NULL }
};

/**
 * @brief 注册log模块到Lua
 */
LUAMOD_API int luaopen_log_register(lua_State* L) {
    luaL_newlibtable(L, log_lib);
    luaL_setfuncs(L, log_lib, 0);
    
    /* 添加日志级别常量 */
    lua_pushinteger(L, LOG_LEVEL_NONE);
    lua_setfield(L, -2, "NONE");
    
    lua_pushinteger(L, LOG_LEVEL_ERROR);
    lua_setfield(L, -2, "ERROR");
    
    lua_pushinteger(L, LOG_LEVEL_WARN);
    lua_setfield(L, -2, "WARN");
    
    lua_pushinteger(L, LOG_LEVEL_INFO);
    lua_setfield(L, -2, "INFO");
    
    lua_pushinteger(L, LOG_LEVEL_DEBUG);
    lua_setfield(L, -2, "DEBUG");
    
    lua_pushinteger(L, 5);  /* TRACE级别 */
    lua_setfield(L, -2, "TRACE");
    
    return 1;
}
