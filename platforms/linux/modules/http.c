/*
@module  http
@summary HTTP客户端(基于mongoose)
@version 1.0
@date    2026.06.19
@tag     HTTP
*/

#include "module.h"
#include "iot_params.h"
#include "mongoose.h"

#define HTTP_CTX_MAX 4

typedef struct {
    struct mg_connection* nc;
    int inited;
    int busy;
    void* callback_ud;
} http_ctx_t;

static http_ctx_t g_http_ctx[HTTP_CTX_MAX];
static struct mg_mgr g_mgr;
static int g_http_inited = 0;
static iot_mutex_t g_http_mutex = NULL;

static void http_context_init(void) {
    if (!g_http_inited) {
        memset(g_http_ctx, 0, sizeof(g_http_ctx));
        mg_mgr_init(&mgr);
        g_http_mutex = iot_mutex_create();
        g_http_inited = 1;
    }
}

static int http_find_free_ctx(void) {
    for (int i = 0; i < HTTP_CTX_MAX; i++) {
        if (!g_http_ctx[i].inited) {
            return i;
        }
    }
    return -1;
}

static void http_event_handler(struct mg_connection* nc, int ev, void* ev_data) {
    int ctx_idx = -1;
    
    iot_mutex_lock(g_http_mutex, -1);
    for (int i = 0; i < HTTP_CTX_MAX; i++) {
        if (g_http_ctx[i].nc == nc) {
            ctx_idx = i;
            break;
        }
    }
    iot_mutex_unlock(g_http_mutex);
    
    if (ctx_idx < 0) return;
    
    http_ctx_t* ctx = &g_http_ctx[ctx_idx];
    
    switch (ev) {
        case MG_EV_CONNECT:
            LOG("HTTP connected to %s:%d", nc->remote_ip, nc->port);
            break;
            
        case MG_EV_SEND:
            LOG("HTTP sent data");
            break;
            
        case MG_EV_RECV:
            LOG("HTTP received %d bytes", nc->recv_len);
            break;
            
        case MG_EV_HTTP_REPLY:
            LOG("HTTP response: code=%d", nc->http_msg.resp_code);
            if (ctx->callback_ud) {
                params_t* params = params_create(3);
                if (params) {
                    params_push_int(params, nc->http_msg.resp_code);
                    
                    if (nc->http_msg.headers) {
                        /* 提取 headers */
                        char* headers_end = strstr(nc->http_msg.headers, "\r\n\r\n");
                        if (headers_end) {
                            size_t headers_len = headers_end - nc->http_msg.headers;
                            params_push_string(params, nc->http_msg.headers, headers_len);
                        } else {
                            params_push_string(params, "", 0);
                        }
                    } else {
                        params_push_string(params, "", 0);
                    }
                    
                    if (nc->http_msg.body && nc->http_msg.body_size > 0) {
                        params_push_string(params, nc->http_msg.body, nc->http_msg.body_size);
                    } else {
                        params_push_string(params, "", 0);
                    }
                    
                    /* 使用 Lua 回调 */
                    lua_State* L = (lua_State*)ctx->callback_ud;
                    iot_callback_call(ctx->callback_ud, params);
                }
            }
            mg_close(nc);
            ctx->nc = NULL;
            ctx->busy = 0;
            ctx->callback_ud = NULL;
            break;
            
        case MG_EV_CLOSE:
            LOG("HTTP connection closed");
            if (ctx->busy && ctx->callback_ud) {
                /* 连接异常关闭，发送错误回调 */
                params_t* params = params_create(3);
                if (params) {
                    params_push_int(params, -1);
                    params_push_string(params, "connection closed", 18);
                    params_push_string(params, "", 0);
                    iot_callback_call(ctx->callback_ud, params);
                }
            }
            ctx->nc = NULL;
            ctx->busy = 0;
            ctx->callback_ud = NULL;
            break;
    }
}

static int iot_http_request_sync(lua_State* L) {
    const char* method = luaL_checkstring(L, 1);
    const char* url = luaL_checkstring(L, 2);
    const char* data = luaL_optstring(L, 3, NULL);
    size_t data_len = data ? strlen(data) : 0;
    
    http_context_init();
    
    /* 创建临时连接 */
    struct mg_connection* nc = mg_connect(&mgr, url, http_event_handler, NULL);
    if (!nc) {
        lua_pushinteger(L, -1);
        lua_pushstring(L, "connect failed");
        lua_pushnil(L);
        return 3;
    }
    
    /* 发送请求 */
    if (strcmp(method, "GET") == 0 || strcmp(method, "get") == 0) {
        mg_http_get(nc, "/");
    } else if (strcmp(method, "POST") == 0 || strcmp(method, "post") == 0) {
        if (data && data_len > 0) {
            mg_http_post(nc, "/", data, data_len);
        } else {
            mg_http_request_simple(nc, "POST", "/", NULL, 0);
        }
    } else if (strcmp(method, "PUT") == 0 || strcmp(method, "put") == 0) {
        mg_http_request_simple(nc, "PUT", "/", NULL, 0);
    } else if (strcmp(method, "DELETE") == 0 || strcmp(method, "delete") == 0) {
        mg_http_request_simple(nc, "DELETE", "/", NULL, 0);
    } else {
        mg_http_request_simple(nc, method, "/", NULL, 0);
    }
    
    /* 等待响应（简单轮询） */
    int timeout = 5000;  /* 5秒超时 */
    int64_t start = iot_get_tick_ms();
    int resp_received = 0;
    
    while (!resp_received && (iot_get_tick_ms() - start) < timeout) {
        mg_mgr_poll(&mgr, 100);
        
        /* 检查是否收到响应 */
        if (nc->http_msg.resp_code > 0) {
            resp_received = 1;
            break;
        }
        
        /* 检查是否已关闭 */
        if (nc == NULL) {
            break;
        }
    }
    
    if (!resp_received) {
        mg_close(nc);
        lua_pushinteger(L, -1);
        lua_pushstring(L, "timeout");
        lua_pushnil(L);
        return 3;
    }
    
    /* 返回响应 */
    lua_pushinteger(L, nc->http_msg.resp_code);
    
    if (nc->http_msg.body && nc->http_msg.body_size > 0) {
        lua_pushlstring(L, nc->http_msg.body, nc->http_msg.body_size);
    } else {
        lua_pushstring(L, "");
    }
    
    if (nc) {
        mg_close(nc);
    }
    
    return 2;
}

static int iot_http_request_async(lua_State* L) {
    const char* method = luaL_checkstring(L, 1);
    const char* url = luaL_checkstring(L, 2);
    const char* data = luaL_optstring(L, 3, NULL);
    
    if (!lua_isfunction(L, 4)) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    size_t data_len = data ? strlen(data) : 0;
    
    http_context_init();
    
    int slot = http_find_free_ctx();
    if (slot < 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "no free http context");
        return 2;
    }
    
    http_ctx_t* ctx = &g_http_ctx[slot];
    
    /* 保存回调 */
    ctx->callback_ud = iot_callback_save(L, 4);
    if (!ctx->callback_ud) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "failed to save callback");
        return 2;
    }
    
    /* 创建连接 */
    ctx->nc = mg_connect(&mgr, url, http_event_handler, ctx);
    if (!ctx->nc) {
        iot_callback_free(ctx->callback_ud);
        ctx->callback_ud = NULL;
        lua_pushboolean(L, 0);
        lua_pushstring(L, "connect failed");
        return 2;
    }
    
    ctx->inited = 1;
    ctx->busy = 1;
    
    /* 发送请求 */
    if (strcmp(method, "GET") == 0 || strcmp(method, "get") == 0) {
        mg_http_get(ctx->nc, "/");
    } else if (strcmp(method, "POST") == 0 || strcmp(method, "post") == 0) {
        if (data && data_len > 0) {
            mg_http_post(ctx->nc, "/", data, data_len);
        } else {
            mg_http_request_simple(ctx->nc, "POST", "/", NULL, 0);
        }
    } else {
        mg_http_request_simple(ctx->nc, method, "/", NULL, 0);
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_http_request(lua_State* L) {
    /* 检查是否有回调函数参数 */
    if (lua_gettop(L) >= 4 && lua_isfunction(L, 4)) {
        return iot_http_request_async(L);
    } else {
        return iot_http_request_sync(L);
    }
}

static int iot_http_uri_encode(lua_State* L) {
    size_t len = 0;
    const char* str = luaL_checklstring(L, 1, &len);
    
    char* encoded = (char*)iot_malloc(len * 3 + 1);
    if (!encoded) {
        lua_pushnil(L);
        return 1;
    }
    
    int pos = 0;
    for (size_t i = 0; i < len; i++) {
        unsigned char c = str[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded[pos++] = c;
        } else {
            encoded[pos++] = '%';
            encoded[pos++] = "0123456789ABCDEF"[c >> 4];
            encoded[pos++] = "0123456789ABCDEF"[c & 0x0F];
        }
    }
    encoded[pos] = '\0';
    
    lua_pushstring(L, encoded);
    iot_free(encoded);
    
    return 1;
}

static int iot_http_uri_encode_component(lua_State* L) {
    return iot_http_uri_encode(L);
}

static int iot_http_poll(lua_State* L) {
    http_context_init();
    
    int timeout_ms = (int)luaL_optinteger(L, 1, 100);
    mg_mgr_poll(&mgr, timeout_ms);
    
    return 0;
}

static const luaL_Reg http_lib[] = {
    { "request",            iot_http_request },
    { "poll",               iot_http_poll },
    { "uri_encode",         iot_http_uri_encode },
    { "uri_encode_component", iot_http_uri_encode_component },
    {NULL, NULL}
};

LUAMOD_API int luaopen_http(lua_State* L) {
    http_context_init();
    luaL_newlibtable(L, http_lib);
    luaL_setfuncs(L, http_lib, 0);
    return 1;
}
