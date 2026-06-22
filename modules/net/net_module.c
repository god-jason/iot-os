#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "platform.h"
#include "iot_params.h"
#include "iot_rtos.h"

#include "net.h"

typedef struct socket_lua_ctx {
    net_socket_t* sock;
    void* userdata;
} socket_lua_ctx_t;

static socket_lua_ctx_t* socket_get_ctx_from_userdata(lua_State* L, int idx) {
    if (lua_type(L, idx) != LUA_TUSERDATA) {
        return NULL;
    }
    socket_lua_ctx_t** ctx_ptr = (socket_lua_ctx_t**)lua_touserdata(L, idx);
    return ctx_ptr && *ctx_ptr ? *ctx_ptr : NULL;
}

static socket_lua_ctx_t* socket_ctx_create(void) {
    socket_lua_ctx_t* ctx = (socket_lua_ctx_t*)iot_malloc(sizeof(socket_lua_ctx_t));
    if (ctx) {
        memset(ctx, 0, sizeof(socket_lua_ctx_t));
    }
    return ctx;
}

static void socket_ctx_destroy(socket_lua_ctx_t* ctx) {
    if (!ctx) return;
    if (ctx->sock) {
        net_socket_close((sock_t)ctx->sock);
    }
    iot_free(ctx);
}

static void socket_event_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    socket_lua_ctx_t* ctx = (socket_lua_ctx_t*)user_data;
    if (!ctx || !ctx->userdata) return;

    params_t* params = params_create(2);
    if (!params) return;

    params_push_int(params, event);

    if (event == NET_EVENT_RECV) {
        const char* buf = net_socket_get_recv_buf((sock_t)sock);
        size_t len = net_socket_get_recv_len((sock_t)sock);
        params_push_string(params, buf, len);
        net_socket_clear_recv_buf((sock_t)sock);
    } else {
        params_push_nil(params);
    }

    iot_rtos_call(ctx->userdata, params);
    params_destroy(params);
}

static int iot_net_socket_create(lua_State* L) {
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

static int iot_net_ssl_socket_create(lua_State* L) {
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

static int iot_socket_set_callback(lua_State* L) {
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

static int iot_socket_connect(lua_State* L) {
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

static int iot_socket_send(lua_State* L) {
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

static int iot_socket_recv(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushnil(L);
        return 1;
    }

    const char* buf = net_socket_get_recv_buf((sock_t)ctx->sock);
    size_t len = net_socket_get_recv_len((sock_t)ctx->sock);

    if (len > 0) {
        lua_pushlstring(L, buf, len);
        net_socket_clear_recv_buf((sock_t)ctx->sock);
    } else {
        lua_pushnil(L);
    }

    return 1;
}

static int iot_socket_bind(lua_State* L) {
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

static int iot_socket_listen(lua_State* L) {
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

static int iot_socket_accept(lua_State* L) {
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

static int iot_socket_close(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        return 0;
    }

    socket_ctx_destroy(ctx);
    return 0;
}

static int iot_socket_state(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushinteger(L, NET_SOCK_STATE_CLOSED);
        return 1;
    }

    net_sock_state_t state = net_socket_get_state((sock_t)ctx->sock);
    lua_pushinteger(L, state);
    return 1;
}

static int iot_socket_is_ssl(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bool is_ssl = net_socket_is_ssl((sock_t)ctx->sock);
    lua_pushboolean(L, is_ssl);
    return 1;
}

static int iot_socket_ssl_handshake_done(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushboolean(L, 0);
        return 1;
    }

    bool done = net_socket_ssl_handshake_done((sock_t)ctx->sock);
    lua_pushboolean(L, done);
    return 1;
}

static int iot_socket_ssl_get_error(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushstring(L, "");
        return 1;
    }

    const char* err = net_socket_ssl_get_error((sock_t)ctx->sock);
    lua_pushstring(L, err ? err : "");
    return 1;
}

static int iot_net_dns_resolve(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);

    if (lua_type(L, 2) != LUA_TUSERDATA) {
        lua_pushboolean(L, 0);
        return 1;
    }

    void* userdata = lua_touserdata(L, 2);

    int ret = net_dns_resolve(name, NULL, userdata);
    lua_pushboolean(L, ret >= 0);
    return 1;
}

static int iot_net_init(lua_State* L) {
    int ret = net_init();
    lua_pushboolean(L, ret >= 0);
    return 1;
}

static int iot_net_deinit(lua_State* L) {
    net_deinit();
    return 0;
}

static const luaL_Reg socket_methods[] = {
    { "set_callback",          iot_socket_set_callback },
    { "connect",               iot_socket_connect },
    { "send",                  iot_socket_send },
    { "recv",                  iot_socket_recv },
    { "bind",                  iot_socket_bind },
    { "listen",                iot_socket_listen },
    { "accept",                iot_socket_accept },
    { "close",                 iot_socket_close },
    { "state",                 iot_socket_state },
    { "is_ssl",                iot_socket_is_ssl },
    { "ssl_handshake_done",    iot_socket_ssl_handshake_done },
    { "ssl_get_error",         iot_socket_ssl_get_error },
    { "__gc",                  iot_socket_close },
    { NULL,                    NULL }
};

static const luaL_Reg net_methods[] = {
    { "socket",                iot_net_socket_create },
    { "ssl_socket",            iot_net_ssl_socket_create },
    { "dns_resolve",           iot_net_dns_resolve },
    { "init",                  iot_net_init },
    { "deinit",                iot_net_deinit },
    { NULL,                    NULL }
};

static const luaL_Const net_constants[] = {
    { "AF_INET",               2 },
    { "SOCK_STREAM",           1 },
    { "SOCK_DGRAM",            2 },
    { "IPPROTO_TCP",           6 },
    { "IPPROTO_UDP",           17 },

    { "CLOSED",                NET_SOCK_STATE_CLOSED },
    { "OPENED",                NET_SOCK_STATE_OPENED },
    { "LISTENING",             NET_SOCK_STATE_LISTENING },
    { "CONNECTING",            NET_SOCK_STATE_CONNECTING },
    { "CONNECTED",             NET_SOCK_STATE_CONNECTED },
    { "SSL_HANDSHAKE",         NET_SOCK_STATE_SSL_HANDSHAKE },
    { "ERROR",                 NET_SOCK_STATE_ERROR },

    { "EVENT_CONNECTED",       NET_EVENT_CONNECTED },
    { "EVENT_DISCONNECTED",    NET_EVENT_DISCONNECTED },
    { "EVENT_ACCEPT",          NET_EVENT_ACCEPT },
    { "EVENT_RECV",            NET_EVENT_RECV },
    { "EVENT_SEND",            NET_EVENT_SEND },
    { "EVENT_ERROR",           NET_EVENT_ERROR },

    { "SSL_PROTOCOL_TLS12",    NET_SSL_PROTOCOL_TLS12 },
    { "SSL_PROTOCOL_TLS13",    NET_SSL_PROTOCOL_TLS13 },
    { "SSL_PROTOCOL_TLCP",     NET_SSL_PROTOCOL_TLCP },
    { "SSL_PROTOCOL_AUTO",     NET_SSL_PROTOCOL_AUTO },

    { "SSL_VERIFY_NONE",       NET_SSL_VERIFY_NONE },
    { "SSL_VERIFY_OPTIONAL",   NET_SSL_VERIFY_OPTIONAL },
    { "SSL_VERIFY_REQUIRED",   NET_SSL_VERIFY_REQUIRED },

    { NULL, 0 }
};

LUAMOD_API int luaopen_net(lua_State* L) {
    luaL_newlib(L, net_methods);

    const luaL_Const* constant = net_constants;
    for (; constant->name; constant++) {
        lua_pushinteger(L, constant->value);
        lua_setfield(L, -2, constant->name);
    }

    luaL_newmetatable(L, "net.socket");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, socket_methods, 0);

    return 1;
}