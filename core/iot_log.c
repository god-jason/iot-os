/**
 * @file iot_log.c
 * @brief IoT日志模块实现
 *
 * 本文件实现日志功能，支持多级别日志输出（ERROR/WARN/INFO/DEBUG/TRACE），
 * 支持C函数调用和Lua脚本调用两种方式，日志输出带级别前缀和函数位置信息。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.20
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
    "[-]",
    "[E]",
    "[W]",
    "[I]",
    "[D]",
    "[T]"
};

/* 日志输出缓冲区（4KB） */
#define LOG_BUFFER_SIZE 4096
static char s_log_buffer[LOG_BUFFER_SIZE];

/**
 * @brief 向日志缓冲区追加格式化字符串
 * @param offset 当前写入偏移
 * @param fmt 格式字符串
 * @param ... 可变参数
 * @return 新的写入偏移
 */
static int log_buf_append(int offset, const char* fmt, ...) {
    if (offset >= LOG_BUFFER_SIZE - 1) return LOG_BUFFER_SIZE - 1;
    va_list args;
    va_start(args, fmt);
    int remaining = LOG_BUFFER_SIZE - offset;
    int written = vsnprintf(s_log_buffer + offset, remaining, fmt, args);
    va_end(args);
    if (written < 0) return offset;
    if (written >= remaining) {
        /* 溢出截断 */
        s_log_buffer[LOG_BUFFER_SIZE - 5] = '.';
        s_log_buffer[LOG_BUFFER_SIZE - 4] = '.';
        s_log_buffer[LOG_BUFFER_SIZE - 3] = '.';
        s_log_buffer[LOG_BUFFER_SIZE - 2] = '\0';
        return LOG_BUFFER_SIZE - 1;
    }
    return offset + written;
}

/**
 * @brief 向日志缓冲区追加字符串
 */
static int log_buf_str(int offset, const char* str, int len) {
    if (offset >= LOG_BUFFER_SIZE - 1) return LOG_BUFFER_SIZE - 1;
    int remaining = LOG_BUFFER_SIZE - offset;
    if (len >= remaining) {
        memcpy(s_log_buffer + offset, str, remaining - 1);
        s_log_buffer[LOG_BUFFER_SIZE - 5] = '.';
        s_log_buffer[LOG_BUFFER_SIZE - 4] = '.';
        s_log_buffer[LOG_BUFFER_SIZE - 3] = '.';
        s_log_buffer[LOG_BUFFER_SIZE - 2] = '\0';
        return LOG_BUFFER_SIZE - 1;
    }
    memcpy(s_log_buffer + offset, str, len);
    return offset + len;
}

/**
 * @brief 设置日志级别
 * @param level 日志级别 (0-5)
 */
void iot_log_set_level(iot_log_level_t level) {
    if (level >= LOG_LEVEL_NONE && level <= LOG_LEVEL_TRACE) {
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
 * @param fmt 格式字符串
 * @param ... 可变参数
 */
void iot_log_printf(iot_log_level_t level, const char* fmt, ...) {
    if (level > s_log_level) {
        return; 
    }
    
    const char* prefix = log_level_prefix[level];
    int offset = 0;
    
    /* 添加级别前缀 */
    offset = log_buf_append(offset, "%s ", prefix);
    
    /* 格式化日志内容 */
    va_list args;
    va_start(args, fmt);
    int remaining = LOG_BUFFER_SIZE - offset;
    int written = vsnprintf(s_log_buffer + offset, remaining, fmt, args);
    va_end(args);
    if (written >= 0 && written < remaining) offset += written;
    else if (written >= remaining) offset = LOG_BUFFER_SIZE - 2;
    
    /* 换行 */
    offset = log_buf_append(offset, "\r\n");
    
    /* 一次性输出 */
    iot_puts(s_log_buffer);
}

/**
 * @brief 将Lua值转换为字符串追加到缓冲区
 * @param L Lua状态机
 * @param idx 栈上的索引
 * @param offset 缓冲区写入偏移
 * @param first 是否是第一个参数
 * @return 新的写入偏移
 */
static int value_to_string(lua_State* L, int idx, int offset, int* first) {
    const char* prefix = *first ? "" : " ";
    *first = 0;
    
    if (lua_isstring(L, idx)) {
        size_t len;
        const char* str = lua_tolstring(L, idx, &len);
        offset = log_buf_str(offset, prefix, (int)strlen(prefix));
        /* 追加字符串内容 */
        offset = log_buf_str(offset, str, (int)len);
    } else if (lua_isnumber(L, idx)) {
        if (lua_isinteger(L, idx)) {
            int64_t v = lua_tointeger(L, idx);
            offset = log_buf_append(offset, "%s%lld", prefix, (long long)v);
        } else {
            double v = lua_tonumber(L, idx);
            offset = log_buf_append(offset, "%s%g", prefix, v);
        }
    } else if (lua_isboolean(L, idx)) {
        int v = lua_toboolean(L, idx);
        offset = log_buf_append(offset, "%s%s", prefix, v ? "true" : "false");
    } else if (lua_isnil(L, idx)) {
        offset = log_buf_append(offset, "%snil", prefix);
    } else if (lua_istable(L, idx)) {
        /* 格式化table: table{key1=>val1, key2=>val2, ...} */
        offset = log_buf_append(offset, "%stable{", prefix);
        
        int count = 0;
        int max_entries = 20;
        lua_pushnil(L);
        while (lua_next(L, idx) != LUA_OK) {
            if (count >= max_entries) {
                offset = log_buf_append(offset, "...");
                lua_pop(L, 2); /* 弹出 value 和 key */
                break;
            }
            if (count > 0) {
                offset = log_buf_append(offset, ", ");
            }
            
            /* 处理key */
            if (lua_isstring(L, -2)) {
                const char* kstr = lua_tostring(L, -2);
                offset = log_buf_append(offset, "%s=>", kstr);
            } else if (lua_isnumber(L, -2)) {
                if (lua_isinteger(L, -2)) {
                    offset = log_buf_append(offset, "%lld=>", (long long)lua_tointeger(L, -2));
                } else {
                    offset = log_buf_append(offset, "%g=>", lua_tonumber(L, -2));
                }
            } else {
                offset = log_buf_append(offset, "[%s]=>", luaL_typename(L, -2));
            }
            
            /* 处理value（简化处理，嵌套table仅显示指针） */
            if (lua_isstring(L, -1)) {
                const char* vstr = lua_tostring(L, -1);
                offset = log_buf_append(offset, "%s", vstr);
            } else if (lua_isnumber(L, -1)) {
                if (lua_isinteger(L, -1)) {
                    offset = log_buf_append(offset, "%lld", (long long)lua_tointeger(L, -1));
                } else {
                    offset = log_buf_append(offset, "%g", lua_tonumber(L, -1));
                }
            } else if (lua_isboolean(L, -1)) {
                offset = log_buf_append(offset, "%s", lua_toboolean(L, -1) ? "true" : "false");
            } else if (lua_isnil(L, -1)) {
                offset = log_buf_append(offset, "nil");
            } else if (lua_istable(L, -1)) {
                offset = log_buf_append(offset, "table:%p", lua_topointer(L, -1));
            } else if (lua_isfunction(L, -1)) {
                offset = log_buf_append(offset, "function:%p", lua_topointer(L, -1));
            } else if (lua_isuserdata(L, -1)) {
                offset = log_buf_append(offset, "userdata:%p", lua_topointer(L, -1));
            } else {
                offset = log_buf_append(offset, "%s", luaL_typename(L, -1));
            }
            
            lua_pop(L, 1); /* 弹出 value，保留 key 以便继续遍历 */
            count++;
        }
        offset = log_buf_append(offset, "}");
    } else if (lua_isfunction(L, idx)) {
        offset = log_buf_append(offset, "%sfunction: %p", prefix, lua_topointer(L, idx));
    } else if (lua_isuserdata(L, idx)) {
        offset = log_buf_append(offset, "%suserdata: %p", prefix, lua_topointer(L, idx));
    } else if (lua_isthread(L, idx)) {
        offset = log_buf_append(offset, "%sthread: %p", prefix, lua_topointer(L, idx));
    } else {
        offset = log_buf_append(offset, "%s%s", prefix, luaL_typename(L, idx));
    }
    return offset;
}

/**
 * @brief 内部日志打印函数，支持多参数
 */
static void iot_log_output(lua_State* L, int level) {
    if (level > s_log_level) {
        return;
    }
    
    int offset = 0;
    
    /* 日志级别前缀 + [lua] 标识 */
    offset = log_buf_append(offset, "%s [lua] ", log_level_prefix[level]);
    
    int n = lua_gettop(L);
    int first = 1;
    
    for (int i = 1; i <= n; i++) {
        offset = value_to_string(L, i, offset, &first);
    }
    
    /* 换行 */
    offset = log_buf_append(offset, "\r\n");
    
    /* 一次性输出 */
    iot_puts(s_log_buffer);
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
