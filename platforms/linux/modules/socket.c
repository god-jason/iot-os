/*
@module  socket
@summary 网络socket(异步事件驱动)
@version 1.0
@date    2026.06.19
@tag     SOCKET
*/

#include "module.h"
#include "iot_callback.h"
#include "iot_params.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

typedef struct {
    void* callback_ud;
    char event[16];
    int inited;
} socket_event_cb_t;

#define SOCKET_EVENT_CB_MAX 8

typedef struct socket_ctx {
    int sock;
    int domain;
    int type;
    int protocol;
    socket_event_cb_t event_cbs[SOCKET_EVENT_CB_MAX];
    struct socket_ctx* next;
} socket_ctx_t;

static socket_ctx_t* g_socket_list = NULL;
static iot_mutex_t g_socket_mutex = NULL;
static int g_poll_thread_running = 0;
static iot_task_t g_poll_thread = NULL;

static const char* socket_event_to_str(int event) {
    switch (event) {
        case 0: return "connect";
        case 1: return "connect_fail";
        case 2: return "recv";
        case 3: return "send";
        case 4: return "error";
        default: return "unknown";
    }
}

static socket_ctx_t* socket_find_ctx_by_sock(int sock) {
    socket_ctx_t* ctx = g_socket_list;
    while (ctx) {
        if (ctx->sock == sock) {
            return ctx;
        }
        ctx = ctx->next;
    }
    return NULL;
}

static socket_ctx_t* socket_ctx_create(void) {
    socket_ctx_t* ctx = (socket_ctx_t*)iot_malloc(sizeof(socket_ctx_t));
    if (!ctx) return NULL;
    memset(ctx, 0, sizeof(socket_ctx_t));
    ctx->sock = -1;
    ctx->next = NULL;
    return ctx;
}

static void socket_ctx_destroy(socket_ctx_t* ctx) {
    if (!ctx) return;
    
    for (int i = 0; i < SOCKET_EVENT_CB_MAX; i++) {
        if (ctx->event_cbs[i].inited && ctx->event_cbs[i].callback_ud) {
            iot_callback_free(ctx->event_cbs[i].callback_ud);
        }
    }
    
    iot_mutex_lock(g_socket_mutex, -1);
    if (g_socket_list == ctx) {
        g_socket_list = ctx->next;
    } else {
        socket_ctx_t* prev = g_socket_list;
        while (prev && prev->next != ctx) {
            prev = prev->next;
        }
        if (prev) {
            prev->next = ctx->next;
        }
    }
    iot_mutex_unlock(g_socket_mutex);
    
    if (ctx->sock >= 0) {
        close(ctx->sock);
    }
    iot_free(ctx);
}

static int socket_find_free_event_cb(socket_ctx_t* ctx) {
    if (!ctx) return -1;
    for (int i = 0; i < SOCKET_EVENT_CB_MAX; i++) {
        if (!ctx->event_cbs[i].inited) {
            return i;
        }
    }
    return -1;
}

static void socket_send_event(socket_ctx_t* ctx, const char* event, int data1, const char* data2, int data2_len) {
    if (!ctx || !event) return;
    
    for (int i = 0; i < SOCKET_EVENT_CB_MAX; i++) {
        socket_event_cb_t* cb = &ctx->event_cbs[i];
        if (!cb->inited || !cb->callback_ud) continue;
        
        if (cb->event[0] != '\0' && strcmp(cb->event, event) != 0) continue;
        
        params_t* params = params_create(3);
        if (!params) continue;
        
        if (cb->event[0] == '\0') {
            params_push_string(params, event, 0);
            params_push_int(params, data1);
            if (data2 && data2_len > 0) {
                params_push_string(params, data2, data2_len);
            } else {
                params_push_nil(params);
            }
        } else {
            params_push_int(params, data1);
            if (data2 && data2_len > 0) {
                params_push_string(params, data2, data2_len);
            } else {
                params_push_nil(params);
            }
        }
        
        iot_callback_call(cb->callback_ud, params);
    }
}

static void* socket_poll_thread(void* arg) {
    fd_set read_set, error_set;
    int max_fd;
    
    while (g_poll_thread_running) {
        iot_mutex_lock(g_socket_mutex, -1);
        
        FD_ZERO(&read_set);
        FD_ZERO(&error_set);
        max_fd = 0;
        
        socket_ctx_t* ctx = g_socket_list;
        while (ctx) {
            if (ctx->sock >= 0) {
                FD_SET(ctx->sock, &read_set);
                FD_SET(ctx->sock, &error_set);
                if (ctx->sock > max_fd) {
                    max_fd = ctx->sock;
                }
            }
            ctx = ctx->next;
        }
        
        iot_mutex_unlock(g_socket_mutex);
        
        if (max_fd == 0) {
            iot_task_delay(10);
            continue;
        }
        
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;
        
        int ret = select(max_fd + 1, &read_set, NULL, &error_set, &timeout);
        if (ret < 0) {
            if (errno != EINTR) {
                LOG("select error: %d", errno);
            }
            continue;
        }
        
        if (ret == 0) continue;
        
        iot_mutex_lock(g_socket_mutex, -1);
        
        ctx = g_socket_list;
        while (ctx) {
            int sock = ctx->sock;
            socket_ctx_t* next = ctx->next;
            
            if (sock >= 0) {
                if (FD_ISSET(sock, &error_set)) {
                    int err = 0;
                    socklen_t len = sizeof(err);
                    getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len);
                    socket_send_event(ctx, "error", err, NULL, 0);
                }
                
                if (FD_ISSET(sock, &read_set)) {
                    char buf[4096];
                    int n = recv(sock, buf, sizeof(buf), 0);
                    if (n > 0) {
                        socket_send_event(ctx, "recv", n, buf, n);
                    } else if (n == 0) {
                        socket_send_event(ctx, "error", 0, "connection closed", 17);
                    }
                }
            }
            
            ctx = next;
        }
        
        iot_mutex_unlock(g_socket_mutex);
    }
    
    return NULL;
}

static void socket_start_poll_thread(void) {
    if (g_poll_thread_running) return;
    
    if (!g_socket_mutex) {
        g_socket_mutex = iot_mutex_create();
    }
    
    g_poll_thread_running = 1;
    g_poll_thread = iot_task_create("socket_poll", socket_poll_thread, NULL, 4096, IOT_OS_PRIO_NORMAL);
}

static socket_ctx_t* socket_get_ctx_from_userdata(lua_State* L, int idx) {
    socket_ctx_t** ctx_ptr = (socket_ctx_t**)luaL_checkudata(L, idx, "socket");
    if (!ctx_ptr || !*ctx_ptr) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid socket");
        return NULL;
    }
    return *ctx_ptr;
}

static int iot_socket_create(lua_State* L) {
    int domain = luaL_checkinteger(L, 1);
    int type = luaL_checkinteger(L, 2);
    int protocol = luaL_optinteger(L, 3, 0);
    
    socket_ctx_t* ctx = socket_ctx_create();
    if (!ctx) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to create socket context");
        return 2;
    }
    
    int sock = socket(domain, type | SOCK_NONBLOCK, protocol);
    if (sock < 0) {
        socket_ctx_destroy(ctx);
        lua_pushnil(L);
        lua_pushstring(L, "socket create failed");
        return 2;
    }
    
    ctx->sock = sock;
    ctx->domain = domain;
    ctx->type = type;
    ctx->protocol = protocol;
    
    iot_mutex_lock(g_socket_mutex, -1);
    ctx->next = g_socket_list;
    g_socket_list = ctx;
    iot_mutex_unlock(g_socket_mutex);
    
    socket_start_poll_thread();
    
    socket_ctx_t** ctx_ptr = (socket_ctx_t**)lua_newuserdata(L, sizeof(socket_ctx_t*));
    *ctx_ptr = ctx;
    
    luaL_getmetatable(L, "socket");
    lua_setmetatable(L, -2);
    
    return 1;
}

static int iot_socket_on(lua_State* L) {
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    const char* event = NULL;
    int callback_idx = 2;
    
    if (lua_type(L, 2) == LUA_TSTRING) {
        event = luaL_checkstring(L, 2);
        callback_idx = 3;
    }
    
    luaL_checktype(L, callback_idx, LUA_TFUNCTION);
    
    int cb_slot = socket_find_free_event_cb(ctx);
    if (cb_slot < 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "no free event callback slot");
        return 2;
    }
    
    void* cb_ud = iot_callback_save(L, callback_idx);
    if (!cb_ud) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "failed to save callback");
        return 2;
    }
    
    socket_event_cb_t* cb = &ctx->event_cbs[cb_slot];
    cb->callback_ud = cb_ud;
    if (event) {
        strncpy(cb->event, event, sizeof(cb->event) - 1);
        cb->event[sizeof(cb->event) - 1] = '\0';
    } else {
        cb->event[0] = '\0';
    }
    cb->inited = 1;
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_socket_connect(lua_State* L) {
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    const char* host = luaL_checkstring(L, 2);
    int port = luaL_checkinteger(L, 3);
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);
    
    int ret = connect(ctx->sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0 && errno != EINPROGRESS) {
        lua_pushboolean(L, 0);
        lua_pushinteger(L, errno);
        return 2;
    }
    
    socket_send_event(ctx, "connect", 1, NULL, 0);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_socket_write(lua_State* L) {
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);
    
    int ret = send(ctx->sock, data, len, 0);
    if (ret < 0) {
        lua_pushnil(L);
        lua_pushinteger(L, errno);
        return 2;
    }
    
    lua_pushinteger(L, ret);
    return 1;
}

static int iot_socket_read(lua_State* L) {
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    int maxlen = luaL_checkinteger(L, 2);
    
    if (maxlen <= 0) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid maxlen");
        return 2;
    }
    
    char* buf = (char*)iot_malloc(maxlen);
    if (!buf) {
        lua_pushnil(L);
        lua_pushstring(L, "malloc failed");
        return 2;
    }
    
    int ret = recv(ctx->sock, buf, maxlen, 0);
    if (ret < 0) {
        iot_free(buf);
        lua_pushnil(L);
        lua_pushinteger(L, errno);
        return 2;
    }
    
    if (ret == 0) {
        iot_free(buf);
        lua_pushstring(L, "");
        return 1;
    }
    
    lua_pushlstring(L, buf, ret);
    iot_free(buf);
    return 1;
}

static int iot_socket_close(lua_State* L) {
    socket_ctx_t** ctx_ptr = (socket_ctx_t**)luaL_checkudata(L, 1, "socket");
    if (!ctx_ptr || !*ctx_ptr) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    socket_ctx_t* ctx = *ctx_ptr;
    socket_ctx_destroy(ctx);
    *ctx_ptr = NULL;
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_socket_setsockopt(lua_State* L) {
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    int level = luaL_checkinteger(L, 2);
    int optname = luaL_checkinteger(L, 3);
    int optval = luaL_checkinteger(L, 4);
    
    int ret = setsockopt(ctx->sock, level, optname, &optval, sizeof(optval));
    if (ret < 0) {
        lua_pushboolean(L, 0);
        lua_pushinteger(L, errno);
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg socket_methods[] = {
    { "on",           iot_socket_on },
    { "connect",      iot_socket_connect },
    { "write",        iot_socket_write },
    { "read",         iot_socket_read },
    { "close",        iot_socket_close },
    { "setsockopt",   iot_socket_setsockopt },
    { NULL, NULL }
};

static int iot_socket_gc(lua_State* L) {
    socket_ctx_t** ctx_ptr = (socket_ctx_t**)luaL_checkudata(L, 1, "socket");
    if (ctx_ptr && *ctx_ptr) {
        socket_ctx_destroy(*ctx_ptr);
        *ctx_ptr = NULL;
    }
    return 0;
}

static const luaL_Reg socket_metatable[] = {
    { "__gc",         iot_socket_gc },
    { NULL, NULL }
};

static const luaL_Reg socket_lib[] = {
    { "create",       iot_socket_create },
    { NULL, NULL }
};

LUAMOD_API int luaopen_socket(lua_State* L) {
    luaL_newmetatable(L, "socket");
    
    luaL_newlibtable(L, socket_methods);
    luaL_setfuncs(L, socket_methods, 0);
    lua_setfield(L, -2, "__index");
    
    luaL_setfuncs(L, socket_metatable, 0);
    
    luaL_newlibtable(L, socket_lib);
    luaL_setfuncs(L, socket_lib, 0);
    
    lua_pushinteger(L, AF_INET);
    lua_setfield(L, -2, "AF_INET");
    lua_pushinteger(L, AF_INET6);
    lua_setfield(L, -2, "AF_INET6");
    
    lua_pushinteger(L, SOCK_STREAM);
    lua_setfield(L, -2, "SOCK_STREAM");
    lua_pushinteger(L, SOCK_DGRAM);
    lua_setfield(L, -2, "SOCK_DGRAM");
    
    lua_pushinteger(L, IPPROTO_TCP);
    lua_setfield(L, -2, "IPPROTO_TCP");
    lua_pushinteger(L, IPPROTO_UDP);
    lua_setfield(L, -2, "IPPROTO_UDP");
    
    lua_pushinteger(L, SOL_SOCKET);
    lua_setfield(L, -2, "SOL_SOCKET");
    
    lua_pushinteger(L, SO_KEEPALIVE);
    lua_setfield(L, -2, "SO_KEEPALIVE");
    lua_pushinteger(L, SO_REUSEADDR);
    lua_setfield(L, -2, "SO_REUSEADDR");
    
    return 1;
}
