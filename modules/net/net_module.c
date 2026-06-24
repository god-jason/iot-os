/**
 * @file net_module.c
 * @brief 网络模块 Lua 绑定实现
 *
 * 实现网络模块到 Lua 的接口封装，提供异步网络操作能力
 * 事件通过 iot_rtos_call 回传到 Lua VM 中
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "iot.h"
#include "iot_params.h"
#include "iot_rtos.h"

#include "net.h"

/**
 * @brief socket Lua 上下文结构
 */
typedef struct socket_lua_ctx {
    net_socket_t* sock;     /* 底层 socket 句柄 */
    void* userdata;         /* Lua 回调 userdata */
} socket_lua_ctx_t;

/**
 * @brief 从 Lua userdata 获取 socket 上下文
 * @param L Lua 状态机
 * @param idx 参数索引
 * @return socket 上下文指针，失败返回 NULL
 */
static socket_lua_ctx_t* socket_get_ctx_from_userdata(lua_State* L, int idx) {
    if (lua_type(L, idx) != LUA_TUSERDATA) {
        return NULL;
    }
    socket_lua_ctx_t** ctx_ptr = (socket_lua_ctx_t**)lua_touserdata(L, idx);
    return ctx_ptr && *ctx_ptr ? *ctx_ptr : NULL;
}

/**
 * @brief 创建 socket Lua 上下文
 * @return 上下文指针，失败返回 NULL
 */
static socket_lua_ctx_t* socket_ctx_create(void) {
    socket_lua_ctx_t* ctx = (socket_lua_ctx_t*)iot_malloc(sizeof(socket_lua_ctx_t));
    if (ctx) {
        memset(ctx, 0, sizeof(socket_lua_ctx_t));
    }
    return ctx;
}

/**
 * @brief 销毁 socket Lua 上下文
 * @param ctx 上下文指针
 */
static void socket_ctx_destroy(socket_lua_ctx_t* ctx) {
    if (!ctx) return;
    if (ctx->sock) {
        net_socket_close((sock_t)ctx->sock);
    }
    iot_free(ctx);
}

/**
 * @brief socket 事件回调函数
 * @param sock socket 句柄
 * @param event 事件类型
 * @param user_data 用户数据
 */
static void socket_event_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    socket_lua_ctx_t* ctx = (socket_lua_ctx_t*)user_data;
    if (!ctx || !ctx->userdata) return;

    params_t* params = params_create(2);
    if (!params) return;

    params_push_int(params, event);

    if (event == NET_EVENT_RECV) {
        const char* buf = net_socket_get_recv_buf((sock_t)sock);
        size_t len = net_socket_get_recv_len((sock_t)sock);
        if (buf && len > 0) {
            params_push_string(params, buf, len);
            net_socket_clear_recv_buf((sock_t)sock);
        } else {
            params_push_nil(params);
        }
    } else {
        params_push_nil(params);
    }

    iot_rtos_call(ctx->userdata, params);
}

/**
 * @brief 创建普通 socket（Lua 接口）
 * @param L Lua 状态机
 * @return 返回新创建的 socket userdata
 */
static int luaopen_net_socket_create(lua_State* L) {
    int type = (int)luaL_optinteger(L, 1, 1);

    socket_lua_ctx_t* ctx = socket_ctx_create();
    if (!ctx) {
        lua_pushnil(L);
        return 1;
    }

    net_sock_type_t sock_type = (type == 1) ? SOCK_TYPE_STREAM : SOCK_TYPE_DGRAM;
    ctx->sock = (net_socket_t*)net_socket_create(sock_type, NULL, socket_event_callback, ctx);
    if (!ctx->sock) {
        iot_free(ctx);
        lua_pushnil(L);
        return 1;
    }

    socket_lua_ctx_t** ctx_ptr = (socket_lua_ctx_t**)lua_newuserdata(L, sizeof(socket_lua_ctx_t*));
    *ctx_ptr = ctx;

    luaL_getmetatable(L, "net.socket");
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 创建 SSL socket（Lua 接口）
 * @param L Lua 状态机
 * @return 返回新创建的 SSL socket userdata
 */
static int luaopen_net_ssl_socket_create(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_ctx_create();
    if (!ctx) {
        lua_pushnil(L);
        return 1;
    }

    net_ssl_config_t ssl_config;
    net_ssl_config_init(&ssl_config);

    if (lua_istable(L, 1)) {
        lua_getfield(L, 1, "protocol");
        if (lua_isnumber(L, -1)) {
            ssl_config.protocol = (net_ssl_protocol_t)(int)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "verify_mode");
        if (lua_isnumber(L, -1)) {
            ssl_config.verify_mode = (net_ssl_verify_mode_t)(int)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "hostname");
        if (lua_isstring(L, -1)) {
            ssl_config.hostname = lua_tostring(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "ca_file");
        if (lua_isstring(L, -1)) {
            ssl_config.ca_file = lua_tostring(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "cert_file");
        if (lua_isstring(L, -1)) {
            ssl_config.cert_file = lua_tostring(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "key_file");
        if (lua_isstring(L, -1)) {
            ssl_config.key_file = lua_tostring(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "key_password");
        if (lua_isstring(L, -1)) {
            ssl_config.key_password = lua_tostring(L, -1);
        }
        lua_pop(L, 1);

        lua_getfield(L, 1, "handshake_timeout_ms");
        if (lua_isnumber(L, -1)) {
            ssl_config.handshake_timeout_ms = (int)lua_tointeger(L, -1);
        }
        lua_pop(L, 1);
    }

    ctx->sock = (net_socket_t*)net_socket_create(SOCK_TYPE_STREAM, &ssl_config, socket_event_callback, ctx);
    if (!ctx->sock) {
        iot_free(ctx);
        lua_pushnil(L);
        return 1;
    }

    socket_lua_ctx_t** ctx_ptr = (socket_lua_ctx_t**)lua_newuserdata(L, sizeof(socket_lua_ctx_t*));
    *ctx_ptr = ctx;

    luaL_getmetatable(L, "net.socket");
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 设置 socket 回调（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 true 成功，false 失败
 */
static int luaopen_net_socket_set_callback(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushboolean(L, 0);
        return 1;
    }

    if (lua_type(L, 2) != LUA_TUSERDATA) {
        lua_pushboolean(L, 0);
        return 1;
    }

    ctx->userdata = lua_touserdata(L, 2);
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 连接到远程服务器（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 true 成功（异步连接中），false 失败
 */
static int luaopen_net_socket_connect(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* host = luaL_checkstring(L, 2);
    int port = (int)luaL_checkinteger(L, 3);

    int ret = net_socket_connect((sock_t)ctx->sock, host, port);
    lua_pushboolean(L, ret >= 0);
    return 1;
}

/**
 * @brief 发送数据（Lua 接口）
 * @param L Lua 状态机
 * @return 返回发送的字节数，-1 失败
 */
static int luaopen_net_socket_send(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushinteger(L, -1);
        return 1;
    }

    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    int sent = net_socket_send((sock_t)ctx->sock, data, len);
    lua_pushinteger(L, sent);
    return 1;
}

/**
 * @brief 接收数据（Lua 接口）
 * @param L Lua 状态机
 * @return 返回接收到的数据字符串，nil 表示无数据
 */
static int luaopen_net_socket_recv(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushnil(L);
        return 1;
    }

    const char* buf = net_socket_get_recv_buf((sock_t)ctx->sock);
    size_t len = net_socket_get_recv_len((sock_t)ctx->sock);

    if (len > 0 && buf) {
        lua_pushlstring(L, buf, len);
        net_socket_clear_recv_buf((sock_t)ctx->sock);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

/**
 * @brief 绑定 socket 到指定地址（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 true 成功，false 失败
 */
static int luaopen_net_socket_bind(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushboolean(L, 0);
        return 1;
    }

    const char* host = luaL_optstring(L, 2, NULL);
    int port = (int)luaL_optinteger(L, 3, 0);

    int ret = net_socket_bind((sock_t)ctx->sock, host, port);
    lua_pushboolean(L, ret >= 0);
    return 1;
}

/**
 * @brief 开始监听 socket（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 true 成功，false 失败
 */
static int luaopen_net_socket_listen(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int backlog = (int)luaL_optinteger(L, 2, 5);

    int ret = net_socket_listen((sock_t)ctx->sock, backlog);
    lua_pushboolean(L, ret >= 0);
    return 1;
}

/**
 * @brief 接受新连接（Lua 接口）
 * @param L Lua 状态机
 * @return 返回新连接的 socket userdata，失败返回 nil
 */
static int luaopen_net_socket_accept(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushnil(L);
        return 1;
    }

    socket_lua_ctx_t* client_ctx = socket_ctx_create();
    if (!client_ctx) {
        lua_pushnil(L);
        return 1;
    }

    client_ctx->sock = (net_socket_t*)net_socket_accept((sock_t)ctx->sock, socket_event_callback, client_ctx);
    if (!client_ctx->sock) {
        iot_free(client_ctx);
        lua_pushnil(L);
        return 1;
    }

    socket_lua_ctx_t** ctx_ptr = (socket_lua_ctx_t**)lua_newuserdata(L, sizeof(socket_lua_ctx_t*));
    *ctx_ptr = client_ctx;

    luaL_getmetatable(L, "net.socket");
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 关闭 socket（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 0
 */
static int luaopen_net_socket_close(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        return 0;
    }

    socket_ctx_destroy(ctx);
    return 0;
}

/**
 * @brief 获取 socket 状态（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 socket 状态值
 */
static int luaopen_net_socket_state(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushinteger(L, NET_SOCK_STATE_CLOSED);
        return 1;
    }

    net_sock_state_t state = net_socket_get_state((sock_t)ctx->sock);
    lua_pushinteger(L, state);
    return 1;
}

/**
 * @brief 检查是否为 SSL socket（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 true 是 SSL socket，false 不是
 */
static int luaopen_net_socket_is_ssl(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bool is_ssl = net_socket_is_ssl((sock_t)ctx->sock);
    lua_pushboolean(L, is_ssl);
    return 1;
}

/**
 * @brief 检查 SSL 握手是否完成（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 true 握手完成，false 未完成
 */
static int luaopen_net_socket_ssl_handshake_done(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bool done = net_socket_ssl_handshake_done((sock_t)ctx->sock);
    lua_pushboolean(L, done);
    return 1;
}

/**
 * @brief 获取 SSL 错误信息（Lua 接口）
 * @param L Lua 状态机
 * @return 返回错误信息字符串
 */
static int luaopen_net_socket_ssl_get_error(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushstring(L, "");
        return 1;
    }

    const char* err = net_socket_ssl_get_error((sock_t)ctx->sock);
    lua_pushstring(L, err ? err : "");
    return 1;
}

/**
 * @brief DNS 解析完成回调函数
 * @param name 主机名
 * @param ip 解析后的 IP 地址（NULL 表示解析失败）
 * @param user_data 用户数据
 */
static void dns_resolve_callback(const char* name, const char* ip, void* user_data) {
    void* userdata = (void*)user_data;
    if (!userdata) return;

    params_t* params = params_create(2);
    if (!params) return;

    params_push_string(params, name, strlen(name));
    if (ip) {
        params_push_string(params, ip, strlen(ip));
    } else {
        params_push_nil(params);
    }

    iot_rtos_call(userdata, params);
}

/**
 * @brief 异步 DNS 解析（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 true 成功（异步进行中），false 失败
 */
static int luaopen_net_dns_resolve(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);

    if (lua_type(L, 2) != LUA_TUSERDATA) {
        lua_pushboolean(L, 0);
        return 1;
    }

    void* userdata = lua_touserdata(L, 2);

    int ret = net_dns_resolve(name, dns_resolve_callback, userdata);
    lua_pushboolean(L, ret >= 0);
    return 1;
}

/**
 * @brief 初始化网络模块（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 true 成功，false 失败
 */
static int luaopen_net_init(lua_State* L) {
    int ret = net_init();
    lua_pushboolean(L, ret >= 0);
    return 1;
}

/**
 * @brief 反初始化网络模块（Lua 接口）
 * @param L Lua 状态机
 * @return 返回 0
 */
static int luaopen_net_deinit(lua_State* L) {
    net_deinit();
    return 0;
}

/**
 * @brief socket 对象方法表
 */
static const luaL_Reg socket_methods[] = {
    { "set_callback",          luaopen_net_socket_set_callback },
    { "connect",               luaopen_net_socket_connect },
    { "send",                  luaopen_net_socket_send },
    { "recv",                  luaopen_net_socket_recv },
    { "bind",                  luaopen_net_socket_bind },
    { "listen",                luaopen_net_socket_listen },
    { "accept",                luaopen_net_socket_accept },
    { "close",                 luaopen_net_socket_close },
    { "state",                 luaopen_net_socket_state },
    { "is_ssl",                luaopen_net_socket_is_ssl },
    { "ssl_handshake_done",    luaopen_net_socket_ssl_handshake_done },
    { "ssl_get_error",         luaopen_net_socket_ssl_get_error },
    { "__gc",                  luaopen_net_socket_close },
    { NULL,                    NULL }
};

/**
 * @brief net 模块方法表
 */
static const luaL_Reg net_methods[] = {
    { "socket",                luaopen_net_socket_create },
    { "ssl_socket",            luaopen_net_ssl_socket_create },
    { "dns_resolve",           luaopen_net_dns_resolve },
    { "init",                  luaopen_net_init },
    { "deinit",                luaopen_net_deinit },
    { NULL,                    NULL }
};

/**
 * @brief Lua 模块加载函数
 * @param L Lua 状态机
 * @return 返回模块表
 */
LUAMOD_API int luaopen_net_register(lua_State* L) {
    luaL_newlib(L, net_methods);

    lua_pushinteger(L, 2);                        lua_setfield(L, -2, "AF_INET");
    lua_pushinteger(L, 1);                        lua_setfield(L, -2, "SOCK_STREAM");
    lua_pushinteger(L, 2);                        lua_setfield(L, -2, "SOCK_DGRAM");
    lua_pushinteger(L, 6);                        lua_setfield(L, -2, "IPPROTO_TCP");
    lua_pushinteger(L, 17);                       lua_setfield(L, -2, "IPPROTO_UDP");

    lua_pushinteger(L, NET_SOCK_STATE_CLOSED);     lua_setfield(L, -2, "CLOSED");
    lua_pushinteger(L, NET_SOCK_STATE_OPENED);     lua_setfield(L, -2, "OPENED");
    lua_pushinteger(L, NET_SOCK_STATE_LISTENING);  lua_setfield(L, -2, "LISTENING");
    lua_pushinteger(L, NET_SOCK_STATE_CONNECTING); lua_setfield(L, -2, "CONNECTING");
    lua_pushinteger(L, NET_SOCK_STATE_CONNECTED);  lua_setfield(L, -2, "CONNECTED");
    lua_pushinteger(L, NET_SOCK_STATE_SSL_HANDSHAKE); lua_setfield(L, -2, "SSL_HANDSHAKE");
    lua_pushinteger(L, NET_SOCK_STATE_ERROR);       lua_setfield(L, -2, "ERROR");

    lua_pushinteger(L, NET_EVENT_CONNECTED);    lua_setfield(L, -2, "EVENT_CONNECTED");
    lua_pushinteger(L, NET_EVENT_DISCONNECTED); lua_setfield(L, -2, "EVENT_DISCONNECTED");
    lua_pushinteger(L, NET_EVENT_ACCEPT);       lua_setfield(L, -2, "EVENT_ACCEPT");
    lua_pushinteger(L, NET_EVENT_RECV);         lua_setfield(L, -2, "EVENT_RECV");
    lua_pushinteger(L, NET_EVENT_SEND);        lua_setfield(L, -2, "EVENT_SEND");
    lua_pushinteger(L, NET_EVENT_ERROR);        lua_setfield(L, -2, "EVENT_ERROR");

    lua_pushinteger(L, NET_SSL_PROTOCOL_TLS12); lua_setfield(L, -2, "SSL_PROTOCOL_TLS12");
    lua_pushinteger(L, NET_SSL_PROTOCOL_TLS13); lua_setfield(L, -2, "SSL_PROTOCOL_TLS13");
    lua_pushinteger(L, NET_SSL_PROTOCOL_TLCP);  lua_setfield(L, -2, "SSL_PROTOCOL_TLCP");
    lua_pushinteger(L, NET_SSL_PROTOCOL_AUTO);  lua_setfield(L, -2, "SSL_PROTOCOL_AUTO");

    lua_pushinteger(L, NET_SSL_VERIFY_NONE);     lua_setfield(L, -2, "SSL_VERIFY_NONE");
    lua_pushinteger(L, NET_SSL_VERIFY_OPTIONAL); lua_setfield(L, -2, "SSL_VERIFY_OPTIONAL");
    lua_pushinteger(L, NET_SSL_VERIFY_REQUIRED); lua_setfield(L, -2, "SSL_VERIFY_REQUIRED");

    luaL_newmetatable(L, "net.socket");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, socket_methods, 0);
    lua_pop(L, 1);

    return 1;
}