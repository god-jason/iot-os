/*
@module  http
@summary HTTP客户端
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     HTTP
*/

/*
HTTP参考示例
-- 同步GET请求
local code, headers, body = http.request("GET", "http://example.com/api/data")
print("code:", code)
print("body:", body)

-- 同步POST请求
local code, headers, body = http.request("POST", "http://example.com/api/submit", "key=value")
print("code:", code)

-- HTTPS请求
local code, headers, body = http.request("GET", "https://example.com/api/data", nil, {ssl=true, ssl_id=0})
print("code:", code)

-- 异步请求
http.request_async("GET", "http://example.com/api/data", nil, function(code, headers, body)
    print("code:", code)
    print("body:", body)
end)
*/


#include "lua.h"
#include "module.h"
#include "iot_rtos.h"
#include "cm_http.h"

#define HTTP_CTX_MAX 4

/* HTTP上下文 */
typedef struct {
    cm_httpclient_handle_t handle;
    int inited;
    int busy;
} http_ctx_t;

/* HTTP异步请求参数 */
typedef struct {
    void* callback_ud;
    char* method;
    char* url;
    char* data;
    size_t data_len;
    int ssl_enable;
    int ssl_id;
} http_async_param_t;

/* HTTP全局上下文 */
static http_ctx_t g_http_ctx[HTTP_CTX_MAX];
static int g_http_inited = 0;

/* 初始化HTTP上下文 */
static void http_context_init(void) {
    if (!g_http_inited) {
        memset(g_http_ctx, 0, sizeof(g_http_ctx));
        g_http_inited = 1;
    }
}

/* 查找空闲的HTTP上下文 */
static int http_find_free_ctx(void) {
    for (int i = 0; i < HTTP_CTX_MAX; i++) {
        if (!g_http_ctx[i].inited) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief HTTP异步请求线程函数
 */
static void http_async_thread(void* param) {
    http_async_param_t* async_param = (http_async_param_t*)param;
    if (!async_param) {
        osThreadExit();
        return;
    }

    /* 解析方法 */
    cm_httpclient_request_type_e req_method = HTTPCLIENT_REQUEST_GET;
    if (strcmp(async_param->method, "POST") == 0 || strcmp(async_param->method, "post") == 0) {
        req_method = HTTPCLIENT_REQUEST_POST;
    } else if (strcmp(async_param->method, "PUT") == 0 || strcmp(async_param->method, "put") == 0) {
        req_method = HTTPCLIENT_REQUEST_PUT;
    } else if (strcmp(async_param->method, "DELETE") == 0 || strcmp(async_param->method, "delete") == 0) {
        req_method = HTTPCLIENT_REQUEST_DELETE;
    } else if (strcmp(async_param->method, "HEAD") == 0 || strcmp(async_param->method, "head") == 0) {
        req_method = HTTPCLIENT_REQUEST_HEAD;
    }

    cm_httpclient_handle_t handle = NULL;
    cm_httpclient_ret_code_e ret = cm_httpclient_create((const uint8_t*)async_param->url, NULL, &handle);
    
    if (ret != CM_HTTP_RET_CODE_OK) {
        /* 发送错误回调 */
        params_t* params = params_create(3);
        if (params) {
            params_push_int(params, ret);
            params_push_string(params, "create client failed", 20);
            params_push_string(params, "", 0);
            iot_rtos_call(async_param->callback_ud, params);
        }
        free(async_param->method);
        free(async_param->url);
        if (async_param->data) free(async_param->data);
        free(async_param);
        osThreadExit();
        return;
    }

    /* 设置SSL配置 */
    if (async_param->ssl_enable) {
        cm_httpclient_cfg_t cfg;
        memset(&cfg, 0, sizeof(cfg));
        cfg.ssl_enable = 1;
        cfg.ssl_id = async_param->ssl_id;
        cm_httpclient_set_cfg(handle, cfg);
    }

    /* 准备请求参数 */
    cm_httpclient_sync_param_t http_param;
    memset(&http_param, 0, sizeof(http_param));
    http_param.method = req_method;
    http_param.path = (const uint8_t*)"";
    http_param.content = (uint8_t*)async_param->data;
    http_param.content_length = async_param->data_len;

    /* 发送同步请求 */
    cm_httpclient_sync_response_t response;
    memset(&response, 0, sizeof(response));

    ret = cm_httpclient_sync_request(handle, http_param, &response);

    /* 准备回调参数 */
    params_t* params = params_create(3);
    if (params) {
        if (ret == CM_HTTP_RET_CODE_OK) {
            params_push_int(params, response.response_code);
            
            /* headers */
            if (response.response_header && response.response_header_len > 0) {
                params_push_string(params, (const char*)response.response_header, response.response_header_len);
            } else {
                params_push_string(params, "", 0);
            }
            
            /* body */
            if (response.response_content && response.response_content_len > 0) {
                params_push_string(params, (const char*)response.response_content, response.response_content_len);
            } else {
                params_push_string(params, "", 0);
            }
        } else {
            params_push_int(params, ret);
            params_push_string(params, "request failed", 14);
            params_push_string(params, "", 0);
        }
        
        iot_rtos_call(async_param->callback_ud, params);
    }

    /* 释放响应数据 */
    cm_httpclient_sync_free_data(handle);
    cm_httpclient_delete(handle);

    /* 释放参数 */
    free(async_param->method);
    free(async_param->url);
    if (async_param->data) free(async_param->data);
    free(async_param);

    osThreadExit();
}

/**
 * @brief 创建HTTP客户端
 * @api http.create(url)
 * @string url 服务器地址
 * @return userdata HTTP客户端句柄，失败返回nil
 * @usage
local client = http.create("http://example.com")
*/
static int iot_http_create(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);

    http_context_init();

    int slot = http_find_free_ctx();
    if (slot < 0) {
        lua_pushnil(L);
        lua_pushstring(L, "no free http context");
        return 2;
    }

    cm_httpclient_handle_t handle;
    cm_httpclient_ret_code_e ret = cm_httpclient_create((const uint8_t*)url, NULL, &handle);
    if (ret != CM_HTTP_RET_CODE_OK) {
        lua_pushnil(L);
        lua_pushinteger(L, ret);
        return 2;
    }

    g_http_ctx[slot].handle = handle;
    g_http_ctx[slot].inited = 1;
    g_http_ctx[slot].busy = 0;

    lua_pushlightuserdata(L, handle);
    return 1;
}

/**
 * @brief 关闭HTTP客户端
 * @api http.close(client)
 * @userdata client HTTP客户端句柄
 * @return bool 成功返回true,失败返回false
 * @usage
http.close(client)
*/
static int iot_http_close(lua_State* L) {
    cm_httpclient_handle_t handle = (cm_httpclient_handle_t)lua_touserdata(L, 1);
    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    cm_httpclient_delete(handle);

    /* 查找并清理上下文 */
    for (int i = 0; i < HTTP_CTX_MAX; i++) {
        if (g_http_ctx[i].handle == handle) {
            g_http_ctx[i].handle = NULL;
            g_http_ctx[i].inited = 0;
            g_http_ctx[i].busy = 0;
            break;
        }
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 设置HTTP参数
 * @api http.set_config(client, config)
 * @userdata client HTTP客户端句柄
 * @table config 配置参数 {ssl=0/1, ssl_id=0, cid=1, conn_timeout=60, rsp_timeout=60}
 * @return bool 成功返回true,失败返回false
 * @usage
http.set_config(client, {ssl=true, ssl_id=0})
*/
static int iot_http_set_config(lua_State* L) {
    cm_httpclient_handle_t handle = (cm_httpclient_handle_t)lua_touserdata(L, 1);
    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    cm_httpclient_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "ssl");
        cfg.ssl_enable = lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "ssl_id");
        cfg.ssl_id = lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "cid");
        cfg.cid = lua_isnil(L, -1) ? 1 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "conn_timeout");
        cfg.conn_timeout = lua_isnil(L, -1) ? 60 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "rsp_timeout");
        cfg.rsp_timeout = lua_isnil(L, -1) ? 60 : lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    cm_httpclient_ret_code_e ret = cm_httpclient_set_cfg(handle, cfg);
    lua_pushboolean(L, ret == CM_HTTP_RET_CODE_OK);
    return 1;
}

/**
 * @brief 同步HTTP请求
 * @api http.request(method, url, data, config)
 * @string method 请求方法 "GET", "POST", "PUT", "DELETE"
 * @string url 请求URL
 * @string data 请求数据(可空)
 * @table config 配置参数 (可空) {ssl=0/1, ssl_id=0}
 * @return int, string, string 返回 code, headers, body
 * @usage
local code, headers, body = http.request("GET", "http://example.com/api")
local code, headers, body = http.request("POST", "http://example.com/api", "key=value")
*/
static int iot_http_request(lua_State* L) {
    const char* method = luaL_checkstring(L, 1);
    const char* url = luaL_checkstring(L, 2);
    const char* data = luaL_optstring(L, 3, NULL);
    size_t data_len = data ? strlen(data) : 0;

    http_context_init();

    /* 解析方法 */
    cm_httpclient_request_type_e req_method = HTTPCLIENT_REQUEST_GET;
    if (strcmp(method, "POST") == 0 || strcmp(method, "post") == 0) {
        req_method = HTTPCLIENT_REQUEST_POST;
    } else if (strcmp(method, "PUT") == 0 || strcmp(method, "put") == 0) {
        req_method = HTTPCLIENT_REQUEST_PUT;
    } else if (strcmp(method, "DELETE") == 0 || strcmp(method, "delete") == 0) {
        req_method = HTTPCLIENT_REQUEST_DELETE;
    } else if (strcmp(method, "HEAD") == 0 || strcmp(method, "head") == 0) {
        req_method = HTTPCLIENT_REQUEST_HEAD;
    }

    /* 如果提供了配置表，先创建客户端 */
    cm_httpclient_handle_t handle = NULL;
    int need_close = 0;

    if (lua_istable(L, 4)) {
        lua_getfield(L, 4, "ssl");
        int ssl_enable = lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        if (ssl_enable) {
            /* 需要创建客户端并设置SSL */
            cm_httpclient_ret_code_e ret = cm_httpclient_create((const uint8_t*)url, NULL, &handle);
            if (ret != CM_HTTP_RET_CODE_OK) {
                lua_pushinteger(L, -1);
                lua_pushstring(L, "create client failed");
                lua_pushnil(L);
                return 3;
            }

            cm_httpclient_cfg_t cfg;
            memset(&cfg, 0, sizeof(cfg));
            cfg.ssl_enable = 1;
            lua_getfield(L, 4, "ssl_id");
            cfg.ssl_id = lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1);
            lua_pop(L, 1);

            cm_httpclient_set_cfg(handle, cfg);
            need_close = 1;
        }
    }

    if (!handle) {
        cm_httpclient_ret_code_e ret = cm_httpclient_create((const uint8_t*)url, NULL, &handle);
        if (ret != CM_HTTP_RET_CODE_OK) {
            lua_pushinteger(L, -1);
            lua_pushstring(L, "create client failed");
            lua_pushnil(L);
            return 3;
        }
        need_close = 1;
    }

    /* 准备请求参数 */
    cm_httpclient_sync_param_t param;
    memset(&param, 0, sizeof(param));
    param.method = req_method;
    param.path = (const uint8_t*)"";
    param.content = (uint8_t*)data;
    param.content_length = data_len;

    /* 发送同步请求 */
    cm_httpclient_sync_response_t response;
    memset(&response, 0, sizeof(response));

    cm_httpclient_ret_code_e ret = cm_httpclient_sync_request(handle, param, &response);

    if (ret != CM_HTTP_RET_CODE_OK) {
        if (need_close) {
            cm_httpclient_delete(handle);
        }
        lua_pushinteger(L, ret);
        lua_pushstring(L, "request failed");
        lua_pushnil(L);
        return 3;
    }

    /* 返回结果 */
    lua_pushinteger(L, response.response_code);

    /* 返回 headers */
    if (response.response_header && response.response_header_len > 0) {
        lua_pushlstring(L, (const char*)response.response_header, response.response_header_len);
    } else {
        lua_pushstring(L, "");
    }

    /* 返回 body */
    if (response.response_content && response.response_content_len > 0) {
        lua_pushlstring(L, (const char*)response.response_content, response.response_content_len);
    } else {
        lua_pushstring(L, "");
    }

    /* 释放响应数据 */
    cm_httpclient_sync_free_data(handle);

    if (need_close) {
        cm_httpclient_delete(handle);
    }

    return 3;
}

/**
 * @brief 异步HTTP请求
 * @api http.request_async(method, url, data, callback, config)
 * @string method 请求方法 "GET", "POST", "PUT", "DELETE"
 * @string url 请求URL
 * @string data 请求数据(可空)
 * @function callback 回调函数, 参数为 (code, headers, body)
 * @table config 配置参数 (可空) {ssl=0/1, ssl_id=0}
 * @return bool 成功返回true,失败返回false
 * @usage
http.request_async("GET", "http://example.com/api/data", nil, function(code, headers, body)
    print("code:", code)
    print("body:", body)
end)
*/
static int iot_http_request_async(lua_State* L) {
    const char* method = luaL_checkstring(L, 1);
    const char* url = luaL_checkstring(L, 2);
    const char* data = luaL_optstring(L, 3, NULL);
    luaL_checktype(L, 4, LUA_TFUNCTION);

    /* 保存回调函数 */
    void* callback_ud = lua_newuserdata(L, 1);
    lua_pushvalue(L, 4);
    lua_setuservalue(L, -2);

    lua_pushlightuserdata(L, callback_ud);
    lua_pushvalue(L, -2);
    lua_settable(L, LUA_REGISTRYINDEX);

    lua_pop(L, 1);

    /* 解析配置 */
    int ssl_enable = 0;
    int ssl_id = 0;
    if (lua_istable(L, 5)) {
        lua_getfield(L, 5, "ssl");
        ssl_enable = lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 5, "ssl_id");
        ssl_id = lua_isnil(L, -1) ? 0 : lua_tointeger(L, -1);
        lua_pop(L, 1);
    }

    /* 分配异步参数 */
    http_async_param_t* async_param = (http_async_param_t*)malloc(sizeof(http_async_param_t));
    if (!async_param) {
        lua_pushlightuserdata(L, callback_ud);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
        lua_pushboolean(L, 0);
        return 1;
    }

    memset(async_param, 0, sizeof(http_async_param_t));
    async_param->callback_ud = callback_ud;
    async_param->method = strdup(method);
    async_param->url = strdup(url);
    if (data) {
        async_param->data = strdup(data);
        async_param->data_len = strlen(data);
    }
    async_param->ssl_enable = ssl_enable;
    async_param->ssl_id = ssl_id;

    /* 创建线程执行HTTP请求 */
    const osThreadAttr_t http_async_attr = {
        .name = "http_async",
        .stack_size = 4096,
        .priority = osPriorityNormal,
    };
    osThreadId_t tid = osThreadNew(http_async_thread, async_param, &http_async_attr);
    if (!tid) {
        free(async_param->method);
        free(async_param->url);
        if (async_param->data) free(async_param->data);
        free(async_param);
        lua_pushlightuserdata(L, callback_ud);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
        lua_pushboolean(L, 0);
        return 1;
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 检查HTTP客户端是否忙碌
 * @api http.busy(client)
 * @userdata client HTTP客户端句柄
 * @return bool 忙碌返回true,空闲返回false
 * @usage
if http.busy(client) then
    print("client is busy")
end
*/
static int iot_http_busy(lua_State* L) {
    cm_httpclient_handle_t handle = (cm_httpclient_handle_t)lua_touserdata(L, 1);
    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bool busy = cm_httpclient_is_busy(handle);
    lua_pushboolean(L, busy ? 1 : 0);
    return 1;
}

/**
 * @brief 终止HTTP请求
 * @api http.terminate(client)
 * @userdata client HTTP客户端句柄
 * @return nil
 * @usage
http.terminate(client)
*/
static int iot_http_terminate(lua_State* L) {
    cm_httpclient_handle_t handle = (cm_httpclient_handle_t)lua_touserdata(L, 1);
    if (handle) {
        cm_httpclient_terminate(handle);
    }
    return 0;
}

/**
 * @brief URI编码
 * @api http.uri_encode(str)
 * @string str 待编码字符串
 * @return string 编码后的字符串
 * @usage
local encoded = http.uri_encode("key=value&name=test")
*/
static int iot_http_uri_encode(lua_State* L) {
    size_t len = 0;
    const char* str = luaL_checklstring(L, 1, &len);

    uint8_t* encoded = cm_httpclient_uri_encode((const uint8_t*)str, len);
    if (encoded) {
        lua_pushstring(L, (const char*)encoded);
        /* 注意: 这里应该释放 encoded,但API说明是内部分配空间需要自行释放 */
        /* 由于没有提供释放接口,暂时不释放 */
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief URI组件编码
 * @api http.uri_encode_component(str)
 * @string str 待编码字符串
 * @return string 编码后的字符串
 * @usage
local encoded = http.uri_encode_component("key=value")
*/
static int iot_http_uri_encode_component(lua_State* L) {
    size_t len = 0;
    const char* str = luaL_checklstring(L, 1, &len);

    uint8_t* encoded = cm_httpclient_uri_encode_component((const uint8_t*)str, len);
    if (encoded) {
        lua_pushstring(L, (const char*)encoded);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static const luaL_Reg http_lib[] = {
    { "create",              iot_http_create },
    { "close",               iot_http_close },
    { "set_config",         iot_http_set_config },
    { "request",            iot_http_request },
    { "request_async",      iot_http_request_async },
    { "busy",               iot_http_busy },
    { "terminate",          iot_http_terminate },
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
