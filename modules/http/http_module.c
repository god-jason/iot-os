/**
 * @file http_module.c
 * @brief HTTP 客户端 Lua 模块封装
 *
 * 提供面向对象的 HTTP 客户端接口，支持 GET、POST、文件下载等功能
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

typedef struct {
    http_client_t* client;
    int inited;
} http_lua_ctx_t;

#define HTTP_CTX_METATABLE "http.client"

/*===========================================================
 * 内部函数
 *===========================================================*/

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

static int iot_http_close(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (ctx) {
        http_ctx_destroy(ctx);
    }
    return 0;
}

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

static const luaL_Reg http_client_methods[] = {
    { "get",             iot_http_get },
    { "post",            iot_http_post },
    { "download",        iot_http_download },
    { "set_timeout",     iot_http_set_timeout },
    { "set_max_redirects", iot_http_set_max_redirects },
    { "close",           iot_http_close },
    { "__gc",            iot_http_close },
    { "__tostring",      iot_http_close },
    { NULL,              NULL }
};

static const luaL_Reg http_module_methods[] = {
    { "new",             iot_http_new },
    { "get",             iot_http_simple_get },
    { "post",            iot_http_simple_post },
    { "download",        iot_http_simple_download },
    { NULL,              NULL }
};

static const luaL_Const http_constants[] = {
    { "GET",     HTTP_METHOD_GET },
    { "POST",    HTTP_METHOD_POST },
    { "PUT",     HTTP_METHOD_PUT },
    { "DELETE",  HTTP_METHOD_DELETE },
    { "HEAD",    HTTP_METHOD_HEAD },
    { "OPTIONS", HTTP_METHOD_OPTIONS },
    
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

LUAMOD_API int luaopen_http(lua_State* L) {
    luaL_newlib(L, http_module_methods);
    
    const luaL_Const* constant = http_constants;
    for (; constant->name; constant++) {
        lua_pushinteger(L, constant->value);
        lua_setfield(L, -2, constant->name);
    }
    
    luaL_newmetatable(L, HTTP_CTX_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, http_client_methods, 0);
    
    return 1;
}
