/*
@module  http
@summary HTTP客户端模块
@version 1.0
@date    2026.06.20
@tag     HTTP
*/

#include "module.h"
#include "esp_http_client.h"

typedef struct http_ctx {
    esp_http_client_handle_t client;
} http_ctx_t;

static http_ctx_t* http_get_ctx_from_userdata(lua_State* L, int idx) {
    http_ctx_t** ctx_ptr = (http_ctx_t**)luaL_checkudata(L, idx, "http");
    if (!ctx_ptr || !*ctx_ptr) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid http client");
        return NULL;
    }
    return *ctx_ptr;
}

static int iot_http_new(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    
    http_ctx_t* ctx = (http_ctx_t*)iot_malloc(sizeof(http_ctx_t));
    if (!ctx) {
        lua_pushnil(L);
        lua_pushstring(L, "malloc failed");
        return 2;
    }
    memset(ctx, 0, sizeof(http_ctx_t));
    
    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 10000,
    };
    
    ctx->client = esp_http_client_init(&config);
    if (!ctx->client) {
        iot_free(ctx);
        lua_pushnil(L);
        lua_pushstring(L, "http client init failed");
        return 2;
    }
    
    http_ctx_t** ctx_ptr = (http_ctx_t**)lua_newuserdata(L, sizeof(http_ctx_t*));
    *ctx_ptr = ctx;
    
    luaL_getmetatable(L, "http");
    lua_setmetatable(L, -2);
    
    return 1;
}

static int iot_http_set_url(lua_State* L) {
    http_ctx_t* ctx = http_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    const char* url = luaL_checkstring(L, 2);
    esp_http_client_set_url(ctx->client, url);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_http_set_method(lua_State* L) {
    http_ctx_t* ctx = http_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    const char* method = luaL_checkstring(L, 2);
    esp_http_client_method_t http_method = HTTP_METHOD_GET;
    
    if (strcmp(method, "POST") == 0) http_method = HTTP_METHOD_POST;
    else if (strcmp(method, "PUT") == 0) http_method = HTTP_METHOD_PUT;
    else if (strcmp(method, "DELETE") == 0) http_method = HTTP_METHOD_DELETE;
    else if (strcmp(method, "HEAD") == 0) http_method = HTTP_METHOD_HEAD;
    
    esp_http_client_set_method(ctx->client, http_method);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_http_set_header(lua_State* L) {
    http_ctx_t* ctx = http_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    const char* key = luaL_checkstring(L, 2);
    const char* value = luaL_checkstring(L, 3);
    
    esp_http_client_set_header(ctx->client, key, value);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_http_set_post_field(lua_State* L) {
    http_ctx_t* ctx = http_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);
    
    esp_http_client_set_post_field(ctx->client, data, len);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_http_perform(lua_State* L) {
    http_ctx_t* ctx = http_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    esp_err_t err = esp_http_client_perform(ctx->client);
    if (err != ESP_OK) {
        lua_pushnil(L);
        lua_pushstring(L, "http perform failed");
        return 2;
    }
    
    int status_code = esp_http_client_get_status_code(ctx->client);
    lua_pushinteger(L, status_code);
    
    return 1;
}

static int iot_http_get_status_code(lua_State* L) {
    http_ctx_t* ctx = http_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    int status_code = esp_http_client_get_status_code(ctx->client);
    lua_pushinteger(L, status_code);
    
    return 1;
}

static int iot_http_read(lua_State* L) {
    http_ctx_t* ctx = http_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    int maxlen = luaL_optinteger(L, 2, 4096);
    
    char* buf = (char*)iot_malloc(maxlen);
    if (!buf) {
        lua_pushnil(L);
        lua_pushstring(L, "malloc failed");
        return 2;
    }
    
    int ret = esp_http_client_read(ctx->client, buf, maxlen);
    if (ret < 0) {
        iot_free(buf);
        lua_pushnil(L);
        lua_pushstring(L, "read failed");
        return 2;
    }
    
    lua_pushlstring(L, buf, ret);
    iot_free(buf);
    
    return 1;
}

static int iot_http_get_header(lua_State* L) {
    http_ctx_t* ctx = http_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    const char* key = luaL_checkstring(L, 2);
    const char* value = esp_http_client_get_header(ctx->client, key);
    
    if (value) {
        lua_pushstring(L, value);
    } else {
        lua_pushnil(L);
    }
    
    return 1;
}

static int iot_http_close(lua_State* L) {
    http_ctx_t** ctx_ptr = (http_ctx_t**)luaL_checkudata(L, 1, "http");
    if (!ctx_ptr || !*ctx_ptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    http_ctx_t* ctx = *ctx_ptr;
    esp_http_client_cleanup(ctx->client);
    iot_free(ctx);
    *ctx_ptr = NULL;
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_http_gc(lua_State* L) {
    http_ctx_t** ctx_ptr = (http_ctx_t**)luaL_checkudata(L, 1, "http");
    if (ctx_ptr && *ctx_ptr) {
        esp_http_client_cleanup((*ctx_ptr)->client);
        iot_free(*ctx_ptr);
        *ctx_ptr = NULL;
    }
    return 0;
}

static const luaL_Reg http_methods[] = {
    { "set_url",       iot_http_set_url },
    { "set_method",    iot_http_set_method },
    { "set_header",    iot_http_set_header },
    { "set_post_field", iot_http_set_post_field },
    { "perform",       iot_http_perform },
    { "get_status_code", iot_http_get_status_code },
    { "read",          iot_http_read },
    { "get_header",    iot_http_get_header },
    { "close",         iot_http_close },
    { NULL, NULL }
};

static const luaL_Reg http_metatable[] = {
    { "__gc",          iot_http_gc },
    { NULL, NULL }
};

static const luaL_Reg http_lib[] = {
    { "new",           iot_http_new },
    { NULL, NULL }
};

LUAMOD_API int luaopen_http(lua_State* L) {
    luaL_newmetatable(L, "http");
    
    luaL_newlibtable(L, http_methods);
    luaL_setfuncs(L, http_methods, 0);
    lua_setfield(L, -2, "__index");
    
    luaL_setfuncs(L, http_metatable, 0);
    
    luaL_newlibtable(L, http_lib);
    luaL_setfuncs(L, http_lib, 0);
    
    return 1;
}
