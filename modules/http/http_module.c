/**
 * @file http_module.c
 * @brief HTTP 模块 Lua 接口封装
 *
 * 提供 HTTP 客户端和服务器的 Lua 接口。
 *
 * Lua 使用示例：
 *
 *   -- HTTP 请求
 *   local res = http.get("http://example.com/api")
 *   print(res.status_code, res.body)
 *
 *   -- 带选项的请求
 *   local res = http.post("http://example.com/api", {
 *       body = "name=test",
 *       content_type = "application/x-www-form-urlencoded",
 *       headers = {
 *           ["Authorization"] = "Bearer token",
 *           ["X-Custom"] = "value"
 *       },
 *       timeout = 30000
 *   })
 *
 *   -- 文件下载
 *   http.download("http://example.com/file.zip", "file.zip")
 *
 *   -- 创建 HTTP 服务器
 *   local server = http.createServer(8080)
 *   server:on("request", function(req, res)
 *       res:writeHead(200, {["Content-Type"] = "text/plain"})
 *       res:end("Hello World")
 *   end)
 *   server:listen()
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "iot.h"

#include "http_client.h"
#include "http_server.h"

typedef struct {
    http_client_t* client;
    int inited;
} http_lua_ctx_t;

typedef struct {
    http_server_t* server;
    int inited;
    lua_State* L;
    int handler_ref;
} http_server_lua_ctx_t;

#define HTTP_CTX_METATABLE "http.client"
#define HTTP_SERVER_METATABLE "http.server"

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

static http_server_lua_ctx_t* http_get_server_ctx(lua_State* L, int idx) {
    if (lua_type(L, idx) != LUA_TUSERDATA) {
        return NULL;
    }
    
    http_server_lua_ctx_t** ctx_ptr = (http_server_lua_ctx_t**)lua_touserdata(L, idx);
    if (!ctx_ptr || !*ctx_ptr) {
        return NULL;
    }
    return *ctx_ptr;
}

static http_lua_ctx_t* http_ctx_create(const http_client_options_t* options) {
    http_lua_ctx_t* ctx = (http_lua_ctx_t*)iot_malloc(sizeof(http_lua_ctx_t));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(http_lua_ctx_t));
    ctx->client = http_client_create(options);
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

static http_method_t http_string_to_method(const char* method) {
    if (!method) return HTTP_METHOD_GET;
    if (strcmp(method, "POST") == 0) return HTTP_METHOD_POST;
    if (strcmp(method, "PUT") == 0) return HTTP_METHOD_PUT;
    if (strcmp(method, "DELETE") == 0) return HTTP_METHOD_DELETE;
    if (strcmp(method, "HEAD") == 0) return HTTP_METHOD_HEAD;
    if (strcmp(method, "OPTIONS") == 0) return HTTP_METHOD_OPTIONS;
    return HTTP_METHOD_GET;
}

static char* http_build_headers_from_table(lua_State* L, int idx) {
    if (!lua_istable(L, idx)) {
        return NULL;
    }
    
    char* headers = (char*)iot_malloc(1024);
    if (!headers) return NULL;
    
    headers[0] = '\0';
    size_t len = 0;
    
    lua_pushnil(L);
    while (lua_next(L, idx) != 0) {
        const char* key = lua_tostring(L, -2);
        const char* value = lua_tostring(L, -1);
        
        if (key && value) {
            len += snprintf(headers + len, 1024 - len, "%s: %s\r\n", key, value);
        }
        
        lua_pop(L, 1);
    }
    
    if (len == 0) {
        iot_free(headers);
        return NULL;
    }
    
    return headers;
}

static int http_push_response(lua_State* L, http_response_t* response) {
    if (!response) {
        lua_pushnil(L);
        return 1;
    }
    
    lua_newtable(L);
    
    lua_pushinteger(L, response->status_code);
    lua_setfield(L, -2, "status_code");
    
    if (response->body) {
        lua_pushlstring(L, response->body, response->body_len);
    } else {
        lua_pushnil(L);
    }
    lua_setfield(L, -2, "body");
    
    if (response->header) {
        lua_newtable(L);
        
        const char* p = response->header;
        while (*p) {
            const char* line_start = p;
            while (*p && *p != '\r') p++;
            
            if (*p == '\r') {
                p += 2;
                
                const char* colon = strchr(line_start, ':');
                if (colon && colon < p) {
                    size_t key_len = colon - line_start;
                    const char* value_start = colon + 1;
                    while (*value_start == ' ') value_start++;
                    
                    lua_pushlstring(L, line_start, key_len);
                    lua_pushstring(L, value_start);
                    lua_settable(L, -3);
                }
            } else {
                break;
            }
        }
        
        lua_setfield(L, -2, "headers");
    } else {
        lua_newtable(L);
        lua_setfield(L, -2, "headers");
    }
    
    if (response->error) {
        lua_pushstring(L, response->error);
    } else {
        lua_pushnil(L);
    }
    lua_setfield(L, -2, "error");
    
    return 1;
}

static int luaopen_http_request(lua_State* L) {
    const char* method = luaL_checkstring(L, 1);
    const char* url = luaL_checkstring(L, 2);
    
    http_client_options_t options = {
        .url = url,
        .method = http_string_to_method(method),
        .timeout_ms = 30000,
        .enable_gzip = true,
        .auto_decompress = true,
    };
    
    char* headers_str = NULL;
    
    if (lua_istable(L, 3)) {
        lua_getfield(L, 3, "body");
        if (lua_isstring(L, -1)) {
            options.body = lua_tostring(L, -1);
            options.body_len = lua_rawlen(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, 3, "content_type");
        if (lua_isstring(L, -1)) {
            options.content_type = lua_tostring(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, 3, "headers");
        if (lua_istable(L, -1)) {
            headers_str = http_build_headers_from_table(L, -1);
            options.headers = headers_str;
        } else if (lua_isstring(L, -1)) {
            options.headers = lua_tostring(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, 3, "timeout");
        if (lua_isnumber(L, -1)) {
            options.timeout_ms = (int)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, 3, "download_path");
        if (lua_isstring(L, -1)) {
            options.download_path = lua_tostring(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, 3, "enable_gzip");
        if (lua_isboolean(L, -1)) {
            options.enable_gzip = lua_toboolean(L, -1);
        }
        lua_pop(L, 1);
    } else {
        const char* body = luaL_optstring(L, 3, NULL);
        const char* content_type = luaL_optstring(L, 4, "application/x-www-form-urlencoded");
        
        options.body = body;
        options.body_len = body ? strlen(body) : 0;
        options.content_type = content_type;
    }
    
    http_lua_ctx_t* ctx = http_ctx_create(&options);
    if (!ctx) {
        if (headers_str) iot_free(headers_str);
        lua_pushnil(L);
        lua_pushstring(L, "failed to create http client");
        return 2;
    }
    
    if (headers_str) {
        iot_free(headers_str);
    }
    
    int ret = http_client_execute(ctx->client);
    if (ret != 0) {
        http_ctx_destroy(ctx);
        lua_pushnil(L);
        lua_pushstring(L, "http request failed");
        return 2;
    }
    
    http_lua_ctx_t** ctx_ptr = (http_lua_ctx_t**)lua_newuserdata(L, sizeof(http_lua_ctx_t*));
    *ctx_ptr = ctx;
    
    luaL_getmetatable(L, HTTP_CTX_METATABLE);
    lua_setmetatable(L, -2);
    
    return 1;
}

static int luaopen_http_get(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    
    http_client_options_t options = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 30000,
        .enable_gzip = true,
        .auto_decompress = true,
    };
    
    char* headers_str = NULL;
    
    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "headers");
        if (lua_istable(L, -1)) {
            headers_str = http_build_headers_from_table(L, -1);
            options.headers = headers_str;
        } else if (lua_isstring(L, -1)) {
            options.headers = lua_tostring(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, 2, "timeout");
        if (lua_isnumber(L, -1)) {
            options.timeout_ms = (int)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
    }
    
    http_lua_ctx_t* ctx = http_ctx_create(&options);
    if (!ctx) {
        if (headers_str) iot_free(headers_str);
        lua_pushnil(L);
        lua_pushstring(L, "failed to create http client");
        return 2;
    }
    
    if (headers_str) {
        iot_free(headers_str);
    }
    
    int ret = http_client_execute(ctx->client);
    if (ret != 0) {
        http_ctx_destroy(ctx);
        lua_pushnil(L);
        lua_pushstring(L, "http get failed");
        return 2;
    }
    
    http_response_t* response = http_client_get_response(ctx->client);
    int result = http_push_response(L, response);
    
    http_ctx_destroy(ctx);
    
    return result;
}

static int luaopen_http_post(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    const char* body = luaL_checkstring(L, 2);
    const char* content_type = luaL_optstring(L, 3, "application/x-www-form-urlencoded");
    
    http_client_options_t options = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .body = body,
        .body_len = strlen(body),
        .content_type = content_type,
        .timeout_ms = 30000,
        .enable_gzip = true,
        .auto_decompress = true,
    };
    
    char* headers_str = NULL;
    
    if (lua_istable(L, 4)) {
        lua_getfield(L, 4, "headers");
        if (lua_istable(L, -1)) {
            headers_str = http_build_headers_from_table(L, -1);
            options.headers = headers_str;
        } else if (lua_isstring(L, -1)) {
            options.headers = lua_tostring(L, -1);
        }
        lua_pop(L, 1);
        
        lua_getfield(L, 4, "timeout");
        if (lua_isnumber(L, -1)) {
            options.timeout_ms = (int)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
    }
    
    http_lua_ctx_t* ctx = http_ctx_create(&options);
    if (!ctx) {
        if (headers_str) iot_free(headers_str);
        lua_pushnil(L);
        lua_pushstring(L, "failed to create http client");
        return 2;
    }
    
    if (headers_str) {
        iot_free(headers_str);
    }
    
    int ret = http_client_execute(ctx->client);
    if (ret != 0) {
        http_ctx_destroy(ctx);
        lua_pushnil(L);
        lua_pushstring(L, "http post failed");
        return 2;
    }
    
    http_response_t* response = http_client_get_response(ctx->client);
    int result = http_push_response(L, response);
    
    http_ctx_destroy(ctx);
    
    return result;
}

static int luaopen_http_client_close(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (ctx) {
        http_ctx_destroy(ctx);
    }
    return 0;
}

static int luaopen_http_client_tostring(lua_State* L) {
    http_lua_ctx_t* ctx = http_get_ctx(L, 1);
    if (!ctx) {
        lua_pushstring(L, "http.client (invalid)");
    } else {
        lua_pushstring(L, "http.client");
    }
    return 1;
}

static int luaopen_http_download(lua_State* L) {
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
 * HTTP 服务器 Lua 接口
 *===========================================================*/

static http_server_lua_ctx_t* http_server_ctx_create(lua_State* L) {
    http_server_lua_ctx_t* ctx = (http_server_lua_ctx_t*)iot_malloc(sizeof(http_server_lua_ctx_t));
    if (!ctx) return NULL;
    
    memset(ctx, 0, sizeof(http_server_lua_ctx_t));
    ctx->server = http_server_create();
    if (!ctx->server) {
        iot_free(ctx);
        return NULL;
    }
    ctx->inited = 1;
    ctx->L = L;
    ctx->handler_ref = LUA_NOREF;
    
    return ctx;
}

static void http_server_ctx_destroy(http_server_lua_ctx_t* ctx) {
    if (!ctx) return;
    if (ctx->server) {
        http_server_destroy(ctx->server);
    }
    if (ctx->L && ctx->handler_ref != LUA_NOREF) {
        luaL_unref(ctx->L, LUA_REGISTRYINDEX, ctx->handler_ref);
    }
    iot_free(ctx);
}

static int luaopen_http_create_server(lua_State* L) {
    int port = (int)luaL_checkinteger(L, 1);
    
    http_server_lua_ctx_t* ctx = http_server_ctx_create(L);
    if (!ctx) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to create http server");
        return 2;
    }
    
    http_server_lua_ctx_t** ctx_ptr = (http_server_lua_ctx_t**)lua_newuserdata(L, sizeof(http_server_lua_ctx_t*));
    *ctx_ptr = ctx;
    
    luaL_getmetatable(L, HTTP_SERVER_METATABLE);
    lua_setmetatable(L, -2);
    
    lua_pushinteger(L, port);
    lua_setfield(L, -2, "_port");
    
    return 1;
}

static void http_server_request_callback(http_server_t* server, 
                                         const http_server_request_t* req, 
                                         http_server_response_t* resp,
                                         void* user_data) {
    http_server_lua_ctx_t* ctx = (http_server_lua_ctx_t*)user_data;
    if (!ctx || !ctx->L || ctx->handler_ref == LUA_NOREF) return;
    
    lua_rawgeti(ctx->L, LUA_REGISTRYINDEX, ctx->handler_ref);
    if (!lua_isfunction(ctx->L, -1)) {
        lua_pop(ctx->L, 1);
        return;
    }
    
    lua_newtable(ctx->L);
    
    lua_pushstring(ctx->L, req->method ? req->method : "GET");
    lua_setfield(ctx->L, -2, "method");
    
    lua_pushstring(ctx->L, req->path ? req->path : "/");
    lua_setfield(ctx->L, -2, "path");
    
    if (req->query) {
        lua_pushstring(ctx->L, req->query);
        lua_setfield(ctx->L, -2, "query");
    }
    
    if (req->body && req->body_len > 0) {
        lua_pushlstring(ctx->L, req->body, req->body_len);
        lua_setfield(ctx->L, -2, "body");
    }
    
    lua_newtable(ctx->L);
    
    lua_pushstring(ctx->L, "writeHead");
    lua_pushlightuserdata(ctx->L, (void*)resp);
    lua_settable(ctx->L, -3);
    
    lua_pushstring(ctx->L, "end");
    lua_pushlightuserdata(ctx->L, (void*)resp);
    lua_settable(ctx->L, -3);
    
    lua_call(ctx->L, 2, 0);
}

static int luaopen_http_server_on(lua_State* L) {
    http_server_lua_ctx_t* ctx = http_get_server_ctx(L, 1);
    if (!ctx || !ctx->server) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid http server");
        return 2;
    }
    
    const char* event = luaL_checkstring(L, 2);
    
    if (strcmp(event, "request") == 0) {
        luaL_checktype(L, 3, LUA_TFUNCTION);
        
        if (ctx->handler_ref != LUA_NOREF) {
            luaL_unref(L, LUA_REGISTRYINDEX, ctx->handler_ref);
        }
        
        lua_pushvalue(L, 3);
        ctx->handler_ref = luaL_ref(L, LUA_REGISTRYINDEX);
        
        http_server_set_request_callback(ctx->server, http_server_request_callback, ctx);
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_http_server_listen(lua_State* L) {
    http_server_lua_ctx_t* ctx = http_get_server_ctx(L, 1);
    if (!ctx || !ctx->server) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid http server");
        return 2;
    }
    
    lua_getfield(L, 1, "_port");
    int port = (int)lua_tointeger(L, -1);
    lua_pop(L, 1);
    
    int ret = http_server_start(ctx->server, (uint16_t)port);
    if (ret != 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "listen failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_http_server_close(lua_State* L) {
    http_server_lua_ctx_t* ctx = http_get_server_ctx(L, 1);
    if (ctx) {
        http_server_ctx_destroy(ctx);
    }
    return 0;
}

static int luaopen_http_server_tostring(lua_State* L) {
    http_server_lua_ctx_t* ctx = http_get_server_ctx(L, 1);
    if (!ctx) {
        lua_pushstring(L, "http.server (invalid)");
    } else {
        lua_pushstring(L, "http.server");
    }
    return 1;
}

/*===========================================================
 * 模块注册
 *===========================================================*/

static const luaL_Reg http_client_methods[] = {
    { "close",   luaopen_http_client_close },
    { "__gc",    luaopen_http_client_close },
    { "__tostring", luaopen_http_client_tostring },
    { NULL,      NULL }
};

static const luaL_Reg http_server_methods[] = {
    { "on",      luaopen_http_server_on },
    { "listen",  luaopen_http_server_listen },
    { "close",   luaopen_http_server_close },
    { "__gc",    luaopen_http_server_close },
    { "__tostring", luaopen_http_server_tostring },
    { NULL,      NULL }
};

static const luaL_Reg http_module_methods[] = {
    { "request",     luaopen_http_request },
    { "get",         luaopen_http_get },
    { "post",        luaopen_http_post },
    { "download",    luaopen_http_download },
    { "createServer", luaopen_http_create_server },
    { NULL,          NULL }
};

LUAMOD_API int luaopen_http_register(lua_State* L) {
    luaL_newlib(L, http_module_methods);
    
    /* 注册常量 - HTTP 方法 */
    lua_pushinteger(L, HTTP_METHOD_GET);     lua_setfield(L, -2, "GET");
    lua_pushinteger(L, HTTP_METHOD_POST);    lua_setfield(L, -2, "POST");
    lua_pushinteger(L, HTTP_METHOD_PUT);     lua_setfield(L, -2, "PUT");
    lua_pushinteger(L, HTTP_METHOD_DELETE);  lua_setfield(L, -2, "DELETE");
    lua_pushinteger(L, HTTP_METHOD_HEAD);    lua_setfield(L, -2, "HEAD");
    lua_pushinteger(L, HTTP_METHOD_OPTIONS); lua_setfield(L, -2, "OPTIONS");
    
    /* 注册常量 - HTTP 状态码 */
    lua_pushinteger(L, HTTP_STATUS_OK);              lua_setfield(L, -2, "STATUS_OK");
    lua_pushinteger(L, HTTP_STATUS_CREATED);         lua_setfield(L, -2, "STATUS_CREATED");
    lua_pushinteger(L, HTTP_STATUS_NO_CONTENT);      lua_setfield(L, -2, "STATUS_NO_CONTENT");
    lua_pushinteger(L, HTTP_STATUS_REDIRECT);        lua_setfield(L, -2, "STATUS_REDIRECT");
    lua_pushinteger(L, HTTP_STATUS_FOUND);           lua_setfield(L, -2, "STATUS_FOUND");
    lua_pushinteger(L, HTTP_STATUS_NOT_MODIFIED);    lua_setfield(L, -2, "STATUS_NOT_MODIFIED");
    lua_pushinteger(L, HTTP_STATUS_BAD_REQUEST);     lua_setfield(L, -2, "STATUS_BAD_REQUEST");
    lua_pushinteger(L, HTTP_STATUS_UNAUTHORIZED);    lua_setfield(L, -2, "STATUS_UNAUTHORIZED");
    lua_pushinteger(L, HTTP_STATUS_FORBIDDEN);       lua_setfield(L, -2, "STATUS_FORBIDDEN");
    lua_pushinteger(L, HTTP_STATUS_NOT_FOUND);       lua_setfield(L, -2, "STATUS_NOT_FOUND");
    lua_pushinteger(L, HTTP_STATUS_SERVER_ERROR);    lua_setfield(L, -2, "STATUS_SERVER_ERROR");
    lua_pushinteger(L, HTTP_STATUS_SERVICE_UNAVAIL); lua_setfield(L, -2, "STATUS_SERVICE_UNAVAIL");
    
    luaL_newmetatable(L, HTTP_CTX_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, http_client_methods, 0);
    
    luaL_newmetatable(L, HTTP_SERVER_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, http_server_methods, 0);
    
    return 1;
}