/**
 * @file url_module.c
 * @brief URL 库的 Lua 模块封装
 *
 * 提供面向对象的 URL 接口，支持解析、编码、解码和构建
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Lua 头文件 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* IoT 核心头文件 */
#include "iot_log.h"
#include "iot_mem.h"
#include "iot_os.h"

/* URL 库头文件 */
#include "url.h"

/*===========================================================
 * 类型定义
 *===========================================================*/

/* URL Lua 上下文 */
typedef struct url_lua_ctx {
    iot_url_t* url;              /* URL 结构体 */
    int is_owner;                /* 是否拥有所有权 */
} url_lua_ctx_t;

/*===========================================================
 * 内部函数
 *===========================================================*/

#define URL_CTX_METATABLE "url.url"

/* 获取 URL 上下文 */
static url_lua_ctx_t* url_get_ctx(lua_State* L, int idx) {
    if (lua_type(L, idx) != LUA_TUSERDATA) {
        LOG("ERR: not userdata");
        return NULL;
    }
    
    url_lua_ctx_t** ctx_ptr = (url_lua_ctx_t**)lua_touserdata(L, idx);
    if (!ctx_ptr || !*ctx_ptr) {
        LOG("ERR: null context");
        return NULL;
    }
    return *ctx_ptr;
}

/* 创建 URL 上下文 */
static url_lua_ctx_t* url_ctx_create(iot_url_t* url, int is_owner) {
    url_lua_ctx_t* ctx = (url_lua_ctx_t*)iot_malloc(sizeof(url_lua_ctx_t));
    if (!ctx) {
        LOG("ERR: malloc url ctx failed");
        if (is_owner && url) {
            iot_url_free(url);
        }
        return NULL;
    }
    ctx->url = url;
    ctx->is_owner = is_owner;
    return ctx;
}

/* 销毁 URL 上下文 */
static void url_ctx_destroy(url_lua_ctx_t* ctx) {
    if (!ctx) return;
    if (ctx->is_owner && ctx->url) {
        iot_url_free(ctx->url);
    }
    iot_free(ctx);
}

/*===========================================================
 * Lua 方法实现
 *===========================================================*/

/**
 * @brief 解析 URL
 * @api url.parse(url_str)
 * @string url_str URL 字符串
 * @return url 对象，失败返回 nil
 * @usage
 * local u = url.parse("https://user:pass@host.com:8080/path?query=1#frag")
 */
static int iot_url_parse(lua_State* L) {
    const char* url_str = luaL_checkstring(L, 1);
    
    iot_url_t* url = iot_url_parse(url_str);
    if (!url) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to parse URL");
        return 2;
    }
    
    url_lua_ctx_t* ctx = url_ctx_create(url, 1);
    if (!ctx) {
        iot_url_free(url);
        lua_pushnil(L);
        lua_pushstring(L, "failed to create context");
        return 2;
    }
    
    url_lua_ctx_t** ctx_ptr = (url_lua_ctx_t**)lua_newuserdata(L, sizeof(url_lua_ctx_t*));
    *ctx_ptr = ctx;
    
    luaL_getmetatable(L, URL_CTX_METATABLE);
    lua_setmetatable(L, -2);
    
    return 1;
}

/**
 * @brief 构建 URL
 * @api url.build(url_obj)
 * @table url_obj URL 组件对象
 * @return URL 字符串，失败返回 nil
 * @usage
 * local str = url.build({
 *     scheme = "https",
 *     host = "host.com",
 *     port = 8080,
 *     path = "/path",
 *     query = "query=1"
 * })
 */
static int iot_url_build(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    
    iot_url_t url;
    memset(&url, 0, sizeof(url));
    
    /* 获取 scheme */
    lua_getfield(L, 1, "scheme");
    if (lua_type(L, -1) == LUA_TSTRING) {
        url.scheme = (char*)lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    /* 获取 host */
    lua_getfield(L, 1, "host");
    if (lua_type(L, -1) == LUA_TSTRING) {
        url.host = (char*)lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    /* 获取 port */
    lua_getfield(L, 1, "port");
    if (lua_type(L, -1) == LUA_TNUMBER) {
        url.port = (int)lua_tointeger(L, -1);
    }
    lua_pop(L, 1);
    
    /* 获取 path */
    lua_getfield(L, 1, "path");
    if (lua_type(L, -1) == LUA_TSTRING) {
        url.path = (char*)lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    /* 获取 query */
    lua_getfield(L, 1, "query");
    if (lua_type(L, -1) == LUA_TSTRING) {
        url.query = (char*)lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    /* 获取 fragment */
    lua_getfield(L, 1, "fragment");
    if (lua_type(L, -1) == LUA_TSTRING) {
        url.fragment = (char*)lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    /* 获取 username */
    lua_getfield(L, 1, "username");
    if (lua_type(L, -1) == LUA_TSTRING) {
        url.username = (char*)lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    /* 获取 password */
    lua_getfield(L, 1, "password");
    if (lua_type(L, -1) == LUA_TSTRING) {
        url.password = (char*)lua_tostring(L, -1);
    }
    lua_pop(L, 1);
    
    char* result = iot_url_build(&url);
    if (!result) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to build URL");
        return 2;
    }
    
    lua_pushstring(L, result);
    iot_free(result);
    
    return 1;
}

/**
 * @brief URL 编码
 * @api url.encode(str, opt)
 * @string str 待编码字符串
 * @bool opt_space_as_plus 空格是否编码为 +，默认 false
 * @return 编码后的字符串
 */
static int iot_url_encode(lua_State* L) {
    const char* str = luaL_checkstring(L, 1);
    int opt_space_as_plus = lua_isnoneornil(L, 2) ? 0 : lua_toboolean(L, 2);
    
    iot_url_encode_opt_t opt = {opt_space_as_plus, 1};
    char* result = iot_url_encode(str, -1, &opt);
    
    if (!result) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to encode");
        return 2;
    }
    
    lua_pushstring(L, result);
    iot_free(result);
    
    return 1;
}

/**
 * @brief URL 解码
 * @api url.decode(str, opt)
 * @string str 待解码字符串
 * @bool opt_space_as_plus + 是否解码为空格，默认 false
 * @return 解码后的字符串
 */
static int iot_url_decode(lua_State* L) {
    const char* str = luaL_checkstring(L, 1);
    int opt_space_as_plus = lua_isnoneornil(L, 2) ? 0 : lua_toboolean(L, 2);
    
    iot_url_encode_opt_t opt = {opt_space_as_plus, 1};
    char* result = iot_url_decode(str, -1, &opt);
    
    if (!result) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to decode");
        return 2;
    }
    
    lua_pushstring(L, result);
    iot_free(result);
    
    return 1;
}

/**
 * @brief 获取 scheme
 * @api url:scheme()
 * @return scheme 字符串
 */
static int iot_url_method_scheme(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (!ctx || !ctx->url) {
        lua_pushnil(L);
        return 1;
    }
    
    if (ctx->url->scheme) {
        lua_pushstring(L, ctx->url->scheme);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取 host
 * @api url:host()
 * @return host 字符串
 */
static int iot_url_method_host(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (!ctx || !ctx->url) {
        lua_pushnil(L);
        return 1;
    }
    
    if (ctx->url->host) {
        lua_pushstring(L, ctx->url->host);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取 port
 * @api url:port()
 * @return port 数值
 */
static int iot_url_method_port(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (!ctx || !ctx->url) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    lua_pushinteger(L, ctx->url->port);
    return 1;
}

/**
 * @brief 获取 path
 * @api url:path()
 * @return path 字符串
 */
static int iot_url_method_path(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (!ctx || !ctx->url) {
        lua_pushnil(L);
        return 1;
    }
    
    if (ctx->url->path) {
        lua_pushstring(L, ctx->url->path);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取 query
 * @api url:query()
 * @return query 字符串
 */
static int iot_url_method_query(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (!ctx || !ctx->url) {
        lua_pushnil(L);
        return 1;
    }
    
    if (ctx->url->query) {
        lua_pushstring(L, ctx->url->query);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取 fragment
 * @api url:fragment()
 * @return fragment 字符串
 */
static int iot_url_method_fragment(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (!ctx || !ctx->url) {
        lua_pushnil(L);
        return 1;
    }
    
    if (ctx->url->fragment) {
        lua_pushstring(L, ctx->url->fragment);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取 username
 * @api url:username()
 * @return username 字符串
 */
static int iot_url_method_username(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (!ctx || !ctx->url) {
        lua_pushnil(L);
        return 1;
    }
    
    if (ctx->url->username) {
        lua_pushstring(L, ctx->url->username);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取 password
 * @api url:password()
 * @return password 字符串
 */
static int iot_url_method_password(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (!ctx || !ctx->url) {
        lua_pushnil(L);
        return 1;
    }
    
    if (ctx->url->password) {
        lua_pushstring(L, ctx->url->password);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 转为字符串
 * @api url:str()
 * @return URL 字符串
 */
static int iot_url_method_str(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (!ctx || !ctx->url) {
        lua_pushstring(L, "");
        return 1;
    }
    
    char* result = iot_url_build(ctx->url);
    if (!result) {
        lua_pushstring(L, "");
        return 1;
    }
    
    lua_pushstring(L, result);
    iot_free(result);
    
    return 1;
}

/**
 * @brief 垃圾回收
 */
static int iot_url_gc(lua_State* L) {
    url_lua_ctx_t* ctx = url_get_ctx(L, 1);
    if (ctx) {
        url_ctx_destroy(ctx);
    }
    return 0;
}

/*===========================================================
 * 模块注册
 *===========================================================*/

/* URL 对象方法表 */
static const luaL_Reg url_methods[] = {
    { "scheme",   iot_url_method_scheme },
    { "host",     iot_url_method_host },
    { "port",     iot_url_method_port },
    { "path",     iot_url_method_path },
    { "query",    iot_url_method_query },
    { "fragment", iot_url_method_fragment },
    { "username", iot_url_method_username },
    { "password", iot_url_method_password },
    { "str",      iot_url_method_str },
    { "__gc",     iot_url_gc },
    { "__tostring", iot_url_method_str },
    { NULL,       NULL }
};

/* url 模块方法表 */
static const luaL_Reg url_module_methods[] = {
    { "parse",  iot_url_parse },
    { "build",  iot_url_build },
    { "encode", iot_url_encode },
    { "decode", iot_url_decode },
    { NULL,     NULL }
};

/* 模块初始化 */
LUAMOD_API int luaopen_url(lua_State* L) {
    /* 创建 url 模块 */
    luaL_newlib(L, url_module_methods);
    
    /* 创建 url 对象元表 */
    luaL_newmetatable(L, URL_CTX_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, url_methods, 0);
    
    return 1;
}
