/**
 * @file http_module.c
 * @brief HTTP 客户端 Lua 模块封装
 *
 * 提供面向对象的 HTTP 客户端接口，支持 GET、POST、文件下载等功能。
 * Lua 使用示例：
 *
 *   -- 创建客户端并发送 GET 请求
 *   local client = http.new()
 *   local res = client:get("http://httpbin.org/get")
 *   print(res.status_code, res.body)
 *   client:close()
 *
 *   -- 或者使用简单接口
 *   local res = http.get("http://httpbin.org/get")
 *   print(res.status_code, res.body)
 *
 *   -- 文件下载
 *   client:download("http://example.com/file.zip", "file.zip")
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "platform.h"
#include "iot_callback.h"
#include "iot_params.h"

#include "http.h"

/*===========================================================
 * 类型定义
 *===========================================================*/

/**
 * @brief HTTP Lua 上下文
 *
 * 每个 HTTP 客户端实例的 Lua 层上下文，包含底层客户端句柄。
 */
typedef struct {
    http_client_t* client;        /* 底层 HTTP 客户端句柄 */
    int inited;                   /* 初始化标志 */
} http_lua_ctx_t;

#define HTTP_CTX_METATABLE "http.client"  /* Lua 用户数据元表名称 */

/*===========================================================
 * 内部函数
 *===========================================================*/

/**
 * @brief 从 Lua 栈获取 HTTP 上下文
 * @param L Lua 状态机
 * @param idx 栈索引
 * @return HTTP 上下文指针，失败返回 NULL
 */
static http_lua_ctx_t* http_get_ctx(lua_State* L, int idx) {
    if (lua_type(L, idx) != LUA_TUSERDATA) {
        return NULL;
    }
    
    http_lua_ctx_t** ctx_ptr = (http_lua_ctx_t**)lua_touserdata(L, idx);
    if (!ctx_ptr || !*ctx_ptr) {
        return NULL;
    }
    return *ctx_ptr;
}

/**
 * @brief 创建 HTTP Lua 上下文
 * @return 新创建的上下文，失败返回 NULL
 */
static http_lua_ctx_t* http_ctx_create(void) {
    http_lua_ctx_t* ctx = (http_lua_ctx_t*)iot_malloc(sizeof(http_lua_ctx_t));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(http_lua_ctx_t));
    ctx->client = http_client_create();
    if (!ctx->client) {
        iot_free(ctx);
        return NULL;
    }
    ctx->inited = 1;
    
    return ctx;
}

/**
 * @brief 销毁 HTTP Lua 上下文
 * @param ctx HTTP 上下文
 */
static void http_ctx_destroy(http_lua_ctx_t* ctx) {
    if (!ctx) return;
    if (ctx->client) {
        http_client_destroy(ctx->client);
    }
    iot_free(ctx);
}

/*===========================================================
 * Lua 方法实现
 *===========================================================*/

/**
 * @brief 创建 HTTP 客户端实例
 *
 * Lua 调用方式：client = http.new()
 *
 * @param L Lua 状态机
 * @return 1 返回 HTTP 客户端 userdata，失败返回 nil 和错误信息
 */
static int iot_http_new(lua_State* L) {
    http_lua_ctx_t* ctx = http_ctx_create();
    if (!ctx) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to create http client");
        return 2;
    }
    
    http_lua_ctx_t** ctx_ptr = (http_lua_ctx_t**)lua_newuserdata(L, sizeof(http_lua_ctx_t*));
    *ctx_ptr = ctx;
    
    luaL_getmetatable(L, HTTP_CTX_METATABLE);
    lua_setmetatable(L, -2);
    
    return 1;
}

/**
 * @brief 发送 GET 请求
 *
 * Lua 调用方式：res = client:get(url)
 *
 * @param L Lua 状态机
 * @param url 请求 URL
 * @return 返回包含 status_code、header、body 的表，失败返回 nil 和错误信息
 */
static int iot_http_get(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid http client");
        return 2;
    }
    
    const char* url = luaL_checkstring(L, 2);
    
    http_response_t response;
    memset(&response, 0, sizeof(response));
    
    int ret = http_client_get(ctx->client, url, &response);
    if (ret != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "http request failed");
        return 2;
    }
    
    lua_newtable(L);
    
    lua_pushinteger(L, response.status_code);
    lua_setfield(L, -2, "status_code");
    
    if (response.header) {
        lua_pushstring(L, response.header);
        lua_setfield(L, -2, "header");
    } else {
        lua_pushnil(L);
        lua_setfield(L, -2, "header");
    }
    
    if (response.body) {
        lua_pushstring(L, response.body);
        lua_setfield(L, -2, "body");
    } else {
        lua_pushnil(L);
        lua_setfield(L, -2, "body");
    }
    
    http_response_free(&response);
    
    return 1;
}

/**
 * @brief 发送 POST 请求
 *
 * Lua 调用方式：res = client:post(url, body, content_type)
 *
 * @param L Lua 状态机
 * @param url 请求 URL
 * @param body 请求体
 * @param content_type Content-Type（可选，默认为 application/x-www-form-urlencoded）
 * @return 返回包含 status_code、header、body 的表，失败返回 nil 和错误信息
 */
static int iot_http_post(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid http client");
        return 2;
    }
    
    const char* url = luaL_checkstring(L, 2);
    const char* body = luaL_checkstring(L, 3);
    const char* content_type = luaL_optstring(L, 4, "application/x-www-form-urlencoded");
    
    http_response_t response;
    memset(&response, 0, sizeof(response));
    
    int ret = http_client_post(ctx->client, url, body, strlen(body), content_type, &response);
    if (ret != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "http request failed");
        return 2;
    }
    
    lua_newtable(L);
    
    lua_pushinteger(L, response.status_code);
    lua_setfield(L, -2, "status_code");
    
    if (response.header) {
        lua_pushstring(L, response.header);
        lua_setfield(L, -2, "header");
    } else {
        lua_pushnil(L);
        lua_setfield(L, -2, "header");
    }
    
    if (response.body) {
        lua_pushstring(L, response.body);
        lua_setfield(L, -2, "body");
    } else {
        lua_pushnil(L);
        lua_setfield(L, -2, "body");
    }
    
    http_response_free(&response);
    
    return 1;
}

/**
 * @brief 下载文件
 *
 * Lua 调用方式：success = client:download(url, save_path)
 *
 * @param L Lua 状态机
 * @param url 文件 URL
 * @param save_path 保存路径
 * @return true 成功，false 失败
 */
static int iot_http_download(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid http client");
        return 2;
    }
    
    const char* url = luaL_checkstring(L, 2);
    const char* save_path = luaL_checkstring(L, 3);
    
    int ret = http_client_download(ctx->client, url, save_path, NULL, NULL);
    if (ret != 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "download failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 设置请求超时时间
 *
 * Lua 调用方式：client:set_timeout(timeout_ms)
 *
 * @param L Lua 状态机
 * @param timeout_ms 超时时间（毫秒）
 * @return true 成功，false 失败
 */
static int iot_http_set_timeout(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid http client");
        return 2;
    }
    
    int timeout_ms = (int)luaL_checkinteger(L, 2);
    
    http_client_set_timeout(ctx->client, timeout_ms);
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 设置最大重定向次数
 *
 * Lua 调用方式：client:set_max_redirects(max_redirects)
 *
 * @param L Lua 状态机
 * @param max_redirects 最大重定向次数
 * @return true 成功，false 失败
 */
static int iot_http_set_max_redirects(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (!ctx || !ctx->client) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid http client");
        return 2;
    }
    
    int max_redirects = (int)luaL_checkinteger(L, 2);
    
    http_client_set_max_redirects(ctx->client, max_redirects);
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 获取客户端字符串表示
 *
 * @param L Lua 状态机
 * @return 1 返回 "http.client" 字符串
 */
static int iot_http_tostring(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (!ctx) {
        lua_pushstring(L, "http.client (invalid)");
    } else {
        lua_pushstring(L, "http.client");
    }
    return 1;
}

/**
 * @brief 关闭并销毁 HTTP 客户端实例
 *
 * Lua 调用方式：client:close()
 *
 * @param L Lua 状态机
 * @return 0 无返回值
 */
static int iot_http_close(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (ctx) {
        http_ctx_destroy(ctx);
    }
    return 0;
}

/**
 * @brief 简单 GET 请求（创建临时客户端）
 *
 * Lua 调用方式：res = http.get(url)
 *
 * @param L Lua 状态机
 * @param url 请求 URL
 * @return 返回包含 status_code、header、body 的表，失败返回 nil 和错误信息
 */
static int iot_http_simple_get(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    
    http_response_t response;
    memset(&response, 0, sizeof(response));
    
    int ret = http_get(url, &response);
    if (ret != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "http request failed");
        return 2;
    }
    
    lua_newtable(L);
    
    lua_pushinteger(L, response.status_code);
    lua_setfield(L, -2, "status_code");
    
    if (response.header) {
        lua_pushstring(L, response.header);
        lua_setfield(L, -2, "header");
    } else {
        lua_pushnil(L);
        lua_setfield(L, -2, "header");
    }
    
    if (response.body) {
        lua_pushstring(L, response.body);
        lua_setfield(L, -2, "body");
    } else {
        lua_pushnil(L);
        lua_setfield(L, -2, "body");
    }
    
    http_response_free(&response);
    
    return 1;
}

/**
 * @brief 简单 POST 请求（创建临时客户端）
 *
 * Lua 调用方式：res = http.post(url, body, content_type)
 *
 * @param L Lua 状态机
 * @param url 请求 URL
 * @param body 请求体
 * @param content_type Content-Type（可选，默认为 application/x-www-form-urlencoded）
 * @return 返回包含 status_code、header、body 的表，失败返回 nil 和错误信息
 */
static int iot_http_simple_post(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    const char* body = luaL_checkstring(L, 2);
    const char* content_type = luaL_optstring(L, 3, "application/x-www-form-urlencoded");
    
    http_response_t response;
    memset(&response, 0, sizeof(response));
    
    int ret = http_post(url, body, strlen(body), content_type, &response);
    if (ret != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "http request failed");
        return 2;
    }
    
    lua_newtable(L);
    
    lua_pushinteger(L, response.status_code);
    lua_setfield(L, -2, "status_code");
    
    if (response.header) {
        lua_pushstring(L, response.header);
        lua_setfield(L, -2, "header");
    } else {
        lua_pushnil(L);
        lua_setfield(L, -2, "header");
    }
    
    if (response.body) {
        lua_pushstring(L, response.body);
        lua_setfield(L, -2, "body");
    } else {
        lua_pushnil(L);
        lua_setfield(L, -2, "body");
    }
    
    http_response_free(&response);
    
    return 1;
}

/**
 * @brief 简单文件下载（创建临时客户端）
 *
 * Lua 调用方式：success = http.download(url, save_path)
 *
 * @param L Lua 状态机
 * @param url 文件 URL
 * @param save_path 保存路径
 * @return true 成功，false 失败
 */
static int iot_http_simple_download(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    const char* save_path = luaL_checkstring(L, 2);
    
    int ret = http_download(url, save_path);
    if (ret != 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "download failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/*===========================================================
 * 模块注册
 *===========================================================*/

/**
 * @brief HTTP 客户端方法列表
 *
 * 这些方法会注册到客户端 userdata 的元表中，可通过 client:method() 调用。
 */
static const luaL_Reg http_client_methods[] = {
    { "get",               iot_http_get },
    { "post",              iot_http_post },
    { "download",          iot_http_download },
    { "set_timeout",       iot_http_set_timeout },
    { "set_max_redirects", iot_http_set_max_redirects },
    { "close",             iot_http_close },
    { "__gc",              iot_http_close },
    { "__tostring",        iot_http_tostring },
    { NULL,                NULL }
};

/**
 * @brief HTTP 模块方法列表
 *
 * 这些方法注册到 http 模块命名空间，可通过 http.method() 调用。
 */
static const luaL_Reg http_module_methods[] = {
    { "new",       iot_http_new },
    { "get",       iot_http_simple_get },
    { "post",      iot_http_simple_post },
    { "download",  iot_http_simple_download },
    { NULL,        NULL }
};

/**
 * @brief HTTP 模块常量定义
 *
 * 包含 HTTP 方法、状态码等常量。
 */
static const luaL_Const http_constants[] = {
    /* HTTP 方法 */
    { "GET",     HTTP_METHOD_GET },
    { "POST",    HTTP_METHOD_POST },
    { "PUT",     HTTP_METHOD_PUT },
    { "DELETE",  HTTP_METHOD_DELETE },
    { "HEAD",    HTTP_METHOD_HEAD },
    { "OPTIONS", HTTP_METHOD_OPTIONS },
    
    /* HTTP 状态码 */
    { "STATUS_OK",              HTTP_STATUS_OK },
    { "STATUS_CREATED",         HTTP_STATUS_CREATED },
    { "STATUS_NO_CONTENT",      HTTP_STATUS_NO_CONTENT },
    { "STATUS_REDIRECT",        HTTP_STATUS_REDIRECT },
    { "STATUS_FOUND",           HTTP_STATUS_FOUND },
    { "STATUS_NOT_MODIFIED",    HTTP_STATUS_NOT_MODIFIED },
    { "STATUS_BAD_REQUEST",     HTTP_STATUS_BAD_REQUEST },
    { "STATUS_UNAUTHORIZED",    HTTP_STATUS_UNAUTHORIZED },
    { "STATUS_FORBIDDEN",       HTTP_STATUS_FORBIDDEN },
    { "STATUS_NOT_FOUND",       HTTP_STATUS_NOT_FOUND },
    { "STATUS_SERVER_ERROR",    HTTP_STATUS_SERVER_ERROR },
    { "STATUS_SERVICE_UNAVAIL", HTTP_STATUS_SERVICE_UNAVAIL },
    
    { NULL, 0 }
};

/**
 * @brief Lua 模块入口函数
 *
 * 当 require("http") 时调用此函数，注册模块方法、常量和客户端元表。
 *
 * @param L Lua 状态机
 * @return 1 返回 http 模块表
 */
LUAMOD_API int luaopen_http(lua_State* L) {
    /* 创建模块表 */
    luaL_newlib(L, http_module_methods);
    
    /* 向模块表添加常量 */
    const luaL_Const* constant = http_constants;
    for (; constant->name; constant++) {
        lua_pushinteger(L, constant->value);
        lua_setfield(L, -2, constant->name);
    }
    
    /* 创建客户端元表 */
    luaL_newmetatable(L, HTTP_CTX_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, http_client_methods, 0);
    
    return 1;
}
