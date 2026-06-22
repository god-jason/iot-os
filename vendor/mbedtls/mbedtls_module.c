/**
 * @file lua_module.c
 * @brief mbedTLS 库的 Lua 模块封装
 *
 * 提供 TLS/SSL 连接和加密功能的 Lua 接口
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Lua 头文件 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* mbedTLS 头文件 */
#include "mbedtls/version.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/error.h"

/*===========================================================
 * 版本信息
 *===========================================================*/

/**
 * @brief 获取 mbedTLS 版本号
 * @api mbedtls.version_number()
 * @return 版本号数字
 */
static int luaopen_mbedtls_version_number(lua_State* L) {
    unsigned int version = mbedtls_version_get_number();
    lua_pushinteger(L, version);
    return 1;
}

/**
 * @brief 获取 mbedTLS 版本字符串
 * @api mbedtls.version_string()
 * @return 版本字符串 "x.y.z"
 */
static int luaopen_mbedtls_version_string(lua_State* L) {
    const char* version = mbedtls_version_get_string();
    lua_pushstring(L, version);
    return 1;
}

/**
 * @brief 获取 mbedTLS 完整版本字符串
 * @api mbedtls.version_string_full()
 * @return 完整版本字符串 "Mbed TLS x.y.z"
 */
static int luaopen_mbedtls_version_string_full(lua_State* L) {
    const char* version = mbedtls_version_get_string_full();
    lua_pushstring(L, version);
    return 1;
}

/**
 * @brief 检查功能是否可用
 * @api mbedtls.check_feature(feature)
 * @string feature 功能名称
 * @return true/false
 */
static int luaopen_mbedtls_check_feature(lua_State* L) {
    const char* feature = luaL_checkstring(L, 1);
    int result = mbedtls_version_check_feature(feature);

    lua_pushboolean(L, result == 0);
    return 1;
}

/*===========================================================
 * 错误处理
 *===========================================================*/

/**
 * @brief 获取错误描述
 * @api mbedtls.error_string(error_code)
 * @int error_code 错误码
 * @return 错误描述字符串
 */
static int luaopen_mbedtls_error_string(lua_State* L) {
    int error_code = (int)luaL_checkinteger(L, 1);

    char error_buf[128];
    mbedtls_strerror(error_code, error_buf, sizeof(error_buf));

    lua_pushstring(L, error_buf);
    return 1;
}

/*===========================================================
 * SSL 配置
 *===========================================================*/

/* SSL 配置元表名称 */
#define SSL_CONF_METATABLE "mbedtls.ssl_conf"

/**
 * @brief 创建 SSL 配置
 * @api mbedtls.ssl_config_new()
 * @return SSL 配置对象
 */
static int luaopen_mbedtls_ssl_config_new(lua_State* L) {
    mbedtls_ssl_config* conf = (mbedtls_ssl_config*)lua_newuserdata(L, sizeof(mbedtls_ssl_config));

    mbedtls_ssl_config_init(conf);

    luaL_getmetatable(L, SSL_CONF_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 设置 SSL 配置默认值
 * @api conf:defaults(endpoint)
 * @int endpoint 0=服务器, 1=客户端
 * @return 成功返回 true，失败返回 nil 和错误信息
 */
static int luaopen_mbedtls_ssl_config_defaults(lua_State* L) {
    mbedtls_ssl_config* conf = (mbedtls_ssl_config*)luaL_checkudata(L, 1, SSL_CONF_METATABLE);
    int endpoint = (int)luaL_checkinteger(L, 2);

    int ret = mbedtls_ssl_config_defaults(conf,
        endpoint == 0 ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
        MBEDTLS_SSL_TRANSPORT_STREAM,
        MBEDTLS_SSL_PRESET_DEFAULT);

    if (ret != 0) {
        lua_pushnil(L);
        char error_buf[128];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        lua_pushstring(L, error_buf);
        return 2;
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 设置认证模式
 * @api conf:authmode(mode)
 * @int mode 0=可选, 1=必需, 2=无
 */
static int luaopen_mbedtls_ssl_config_authmode(lua_State* L) {
    mbedtls_ssl_config* conf = (mbedtls_ssl_config*)luaL_checkudata(L, 1, SSL_CONF_METATABLE);
    int mode = (int)luaL_checkinteger(L, 2);

    int authmode;
    switch (mode) {
        case 0: authmode = MBEDTLS_SSL_VERIFY_OPTIONAL; break;
        case 1: authmode = MBEDTLS_SSL_VERIFY_REQUIRED; break;
        case 2: authmode = MBEDTLS_SSL_VERIFY_NONE; break;
        default: authmode = MBEDTLS_SSL_VERIFY_NONE; break;
    }

    mbedtls_ssl_conf_authmode(conf, authmode);

    return 0;
}

/**
 * @brief SSL 配置垃圾回收
 */
static int luaopen_mbedtls_ssl_config_gc(lua_State* L) {
    mbedtls_ssl_config* conf = (mbedtls_ssl_config*)luaL_checkudata(L, 1, SSL_CONF_METATABLE);
    mbedtls_ssl_config_free(conf);
    return 0;
}

/*===========================================================
 * SSL 上下文
 *===========================================================*/

/* SSL 上下文元表名称 */
#define SSL_CTX_METATABLE "mbedtls.ssl_ctx"

/**
 * @brief 创建 SSL 上下文
 * @api mbedtls.ssl_new(conf)
 * @param conf SSL 配置对象
 * @return SSL 上下文对象
 */
static int luaopen_mbedtls_ssl_new(lua_State* L) {
    mbedtls_ssl_config* conf = (mbedtls_ssl_config*)luaL_checkudata(L, 1, SSL_CONF_METATABLE);

    mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)lua_newuserdata(L, sizeof(mbedtls_ssl_context));

    mbedtls_ssl_init(ssl);

    int ret = mbedtls_ssl_setup(ssl, conf);
    if (ret != 0) {
        mbedtls_ssl_free(ssl);
        luaL_error(L, "ssl setup failed");
    }

    luaL_getmetatable(L, SSL_CTX_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 设置主机名（用于 SNI）
 * @api ssl:set_hostname(hostname)
 * @string hostname 主机名
 * @return 成功返回 true
 */
static int luaopen_mbedtls_ssl_set_hostname(lua_State* L) {
    mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)luaL_checkudata(L, 1, SSL_CTX_METATABLE);
    const char* hostname = luaL_checkstring(L, 2);

    int ret = mbedtls_ssl_set_hostname(ssl, hostname);
    if (ret != 0) {
        lua_pushnil(L);
        char error_buf[128];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        lua_pushstring(L, error_buf);
        return 2;
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 获取握手状态
 * @api ssl:handshake()
 * @return 成功返回 true，进行中返回 false，失败返回 nil 和错误信息
 */
static int luaopen_mbedtls_ssl_handshake(lua_State* L) {
    mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)luaL_checkudata(L, 1, SSL_CTX_METATABLE);

    int ret = mbedtls_ssl_handshake(ssl);

    if (ret == 0) {
        lua_pushboolean(L, 1);
        return 1;
    }

    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushnil(L);
    char error_buf[128];
    mbedtls_strerror(ret, error_buf, sizeof(error_buf));
    lua_pushstring(L, error_buf);
    return 2;
}

/**
 * @brief 读取数据
 * @api ssl:read(len)
 * @int len 最大读取长度
 * @return 数据字符串，失败返回 nil 和错误信息
 */
static int luaopen_mbedtls_ssl_read(lua_State* L) {
    mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)luaL_checkudata(L, 1, SSL_CTX_METATABLE);
    int len = (int)luaL_checkinteger(L, 2);

    if (len <= 0 || len > 16384) {
        luaL_error(L, "invalid length");
    }

    unsigned char* buf = (unsigned char*)malloc(len);
    if (!buf) {
        luaL_error(L, "memory allocation failed");
    }

    int ret = mbedtls_ssl_read(ssl, buf, len);

    if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
        free(buf);
        lua_pushnil(L);
        lua_pushstring(L, "want read");
        return 2;
    }

    if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        free(buf);
        lua_pushnil(L);
        lua_pushstring(L, "want write");
        return 2;
    }

    if (ret <= 0) {
        free(buf);
        lua_pushnil(L);
        char error_buf[128];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        lua_pushstring(L, error_buf);
        return 2;
    }

    lua_pushlstring(L, (const char*)buf, ret);
    free(buf);
    return 1;
}

/**
 * @brief 写入数据
 * @api ssl:write(data)
 * @string data 数据
 * @return 写入的字节数，失败返回 nil 和错误信息
 */
static int luaopen_mbedtls_ssl_write(lua_State* L) {
    mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)luaL_checkudata(L, 1, SSL_CTX_METATABLE);
    size_t data_len;
    const unsigned char* data = (const unsigned char*)luaL_checklstring(L, 2, &data_len);

    int ret = mbedtls_ssl_write(ssl, data, data_len);

    if (ret == MBEDTLS_ERR_SSL_WANT_READ) {
        lua_pushnil(L);
        lua_pushstring(L, "want read");
        return 2;
    }

    if (ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        lua_pushnil(L);
        lua_pushstring(L, "want write");
        return 2;
    }

    if (ret <= 0) {
        lua_pushnil(L);
        char error_buf[128];
        mbedtls_strerror(ret, error_buf, sizeof(error_buf));
        lua_pushstring(L, error_buf);
        return 2;
    }

    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 关闭连接
 * @api ssl:close_notify()
 * @return 成功返回 true
 */
static int luaopen_mbedtls_ssl_close_notify(lua_State* L) {
    mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)luaL_checkudata(L, 1, SSL_CTX_METATABLE);

    int ret = mbedtls_ssl_close_notify(ssl);

    lua_pushboolean(L, ret == 0 || ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);
    return 1;
}

/**
 * @brief 获取当前协议版本
 * @api ssl:get_version()
 * @return 协议版本字符串
 */
static int luaopen_mbedtls_ssl_get_version(lua_State* L) {
    mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)luaL_checkudata(L, 1, SSL_CTX_METATABLE);

    const char* version = mbedtls_ssl_get_version(ssl);
    lua_pushstring(L, version);
    return 1;
}

/**
 * @brief 获取当前加密套件
 * @api ssl:get_ciphersuite()
 * @return 加密套件名称
 */
static int luaopen_mbedtls_ssl_get_ciphersuite(lua_State* L) {
    mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)luaL_checkudata(L, 1, SSL_CTX_METATABLE);

    const char* ciphersuite = mbedtls_ssl_get_ciphersuite(ssl);
    lua_pushstring(L, ciphersuite);
    return 1;
}

/**
 * @brief SSL 上下文垃圾回收
 */
static int luaopen_mbedtls_ssl_ctx_gc(lua_State* L) {
    mbedtls_ssl_context* ssl = (mbedtls_ssl_context*)luaL_checkudata(L, 1, SSL_CTX_METATABLE);
    mbedtls_ssl_free(ssl);
    return 0;
}

/*===========================================================
 * 模块注册
 *===========================================================*/

/* SSL 配置方法表 */
static const luaL_Reg luaopen_mbedtls_ssl_conf_methods[] = {
    { "defaults",  luaopen_mbedtls_ssl_config_defaults },
    { "authmode",  luaopen_mbedtls_ssl_config_authmode },
    { "__gc",      luaopen_mbedtls_ssl_config_gc },
    { NULL,        NULL }
};

/* SSL 上下文方法表 */
static const luaL_Reg luaopen_mbedtls_ssl_ctx_methods[] = {
    { "set_hostname",    luaopen_mbedtls_ssl_set_hostname },
    { "handshake",       luaopen_mbedtls_ssl_handshake },
    { "read",            luaopen_mbedtls_ssl_read },
    { "write",           luaopen_mbedtls_ssl_write },
    { "close_notify",    luaopen_mbedtls_ssl_close_notify },
    { "get_version",     luaopen_mbedtls_ssl_get_version },
    { "get_ciphersuite", luaopen_mbedtls_ssl_get_ciphersuite },
    { "__gc",            luaopen_mbedtls_ssl_ctx_gc },
    { NULL,              NULL }
};

/* mbedtls 模块方法表 */
static const luaL_Reg luaopen_mbedtls_module_methods[] = {
    /* 版本信息 */
    { "version_number",     luaopen_mbedtls_version_number },
    { "version_string",     luaopen_mbedtls_version_string },
    { "version_string_full", luaopen_mbedtls_version_string_full },
    { "check_feature",      luaopen_mbedtls_check_feature },

    /* 错误处理 */
    { "error_string",       luaopen_mbedtls_error_string },

    /* SSL 配置 */
    { "ssl_config_new",     luaopen_mbedtls_ssl_config_new },
    { "ssl_new",            luaopen_mbedtls_ssl_new },

    { NULL,                 NULL }
};

/* 模块初始化 */
LUAMOD_API int luaopen_mbedtls_register(lua_State* L) {
    /* 创建模块 */
    luaL_newlib(L, luaopen_mbedtls_module_methods);

    /* 创建 SSL 配置元表 */
    luaL_newmetatable(L, SSL_CONF_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, luaopen_mbedtls_ssl_conf_methods, 0);
    lua_pop(L, 1);

    /* 创建 SSL 上下文元表 */
    luaL_newmetatable(L, SSL_CTX_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, luaopen_mbedtls_ssl_ctx_methods, 0);
    lua_pop(L, 1);

    /* 注册常量 */
    lua_pushinteger(L, MBEDTLS_SSL_IS_CLIENT);
    lua_setfield(L, -2, "SSL_IS_CLIENT");

    lua_pushinteger(L, MBEDTLS_SSL_IS_SERVER);
    lua_setfield(L, -2, "SSL_IS_SERVER");

    lua_pushinteger(L, MBEDTLS_SSL_VERIFY_NONE);
    lua_setfield(L, -2, "SSL_VERIFY_NONE");

    lua_pushinteger(L, MBEDTLS_SSL_VERIFY_OPTIONAL);
    lua_setfield(L, -2, "SSL_VERIFY_OPTIONAL");

    lua_pushinteger(L, MBEDTLS_SSL_VERIFY_REQUIRED);
    lua_setfield(L, -2, "SSL_VERIFY_REQUIRED");

    return 1;
}