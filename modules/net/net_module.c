/**
 * @file lua_module.c
 * @brief 基于 net 的 Lua 面向对象封装
 *
 * 提供面向对象的 socket 接口，支持事件回调
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Lua 头文件 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* IoT 核心头文件 */
#include "platform.h"
#include "iot_callback.h"
#include "iot_params.h"

/* net 组件头文件 */
#include "net.h"

/*===========================================================
 * 类型定义
 *===========================================================*/

/* 事件回调结构 */
typedef struct {
    void* callback_ud;      /* Lua回调函数userdata */
    char event[32];          /* 事件名称，空字符串表示订阅全部 */
    int inited;              /* 是否已初始化 */
} socket_event_cb_t;

/* Socket Lua 上下文 */
typedef struct socket_lua_ctx {
    net_socket_t* sock;               /* socket 句柄 */
    int domain;                      /* 协议族 */
    int type;                        /* 类型 */
    int protocol;                    /* 协议 */
    socket_event_cb_t event_cbs[8];  /* 事件回调数组 */
    
    /* 链表节点 */
    struct socket_lua_ctx* next;
} socket_lua_ctx_t;

/*===========================================================
 * 全局变量
 *===========================================================*/

static socket_lua_ctx_t* g_socket_list = NULL;
static iot_mutex_t g_socket_mutex = NULL;
static int g_lua_module_inited = 0;

/*===========================================================
 * 内部函数
 *===========================================================*/

/* 初始化模块 */
static void lua_module_init(void) {
    if (!g_lua_module_inited) {
        g_socket_mutex = iot_mutex_create();
        g_lua_module_inited = 1;
    }
}

/* 获取 socket 上下文 */
static socket_lua_ctx_t* socket_get_ctx_from_userdata(lua_State* L, int idx) {
    if (lua_type(L, idx) != LUA_TUSERDATA) {
        LOG("ERR: not userdata");
        return NULL;
    }
    
    socket_lua_ctx_t** ctx_ptr = (socket_lua_ctx_t**)lua_touserdata(L, idx);
    if (!ctx_ptr || !*ctx_ptr) {
        LOG("ERR: null context");
        return NULL;
    }
    return *ctx_ptr;
}

/* 创建 socket 上下文 */
static socket_lua_ctx_t* socket_ctx_create(void) {
    socket_lua_ctx_t* ctx = (socket_lua_ctx_t*)iot_malloc(sizeof(socket_lua_ctx_t));
    if (!ctx) {
        LOG("ERR: malloc socket ctx failed");
        return NULL;
    }
    memset(ctx, 0, sizeof(socket_lua_ctx_t));
    ctx->sock = NULL;
    return ctx;
}

/* 销毁 socket 上下文 */
static void socket_ctx_destroy(socket_lua_ctx_t* ctx) {
    if (!ctx) return;
    
    /* 清理所有事件回调 */
    for (int i = 0; i < 8; i++) {
        if (ctx->event_cbs[i].inited && ctx->event_cbs[i].callback_ud) {
            iot_callback_free(ctx->event_cbs[i].callback_ud);
        }
    }
    
    /* 关闭 socket */
    if (ctx->sock) {
        net_socket_close((sock_t)ctx->sock);
    }
    
    iot_free(ctx);
}

/* 从链表中查找空闲的回调槽 */
static int socket_find_free_event_cb(socket_lua_ctx_t* ctx) {
    if (!ctx) return -1;
    for (int i = 0; i < 8; i++) {
        if (!ctx->event_cbs[i].inited) {
            return i;
        }
    }
    return -1;
}

/* 发送事件到 Lua 回调 */
static void socket_send_event(socket_lua_ctx_t* ctx, const char* event, int data1, const char* data2, size_t data2_len) {
    if (!ctx || !event) return;
    
    for (int i = 0; i < 8; i++) {
        socket_event_cb_t* cb = &ctx->event_cbs[i];
        if (!cb->inited || !cb->callback_ud) continue;
        
        /* 检查是否匹配事件：空字符串表示订阅全部 */
        if (cb->event[0] != '\0' && strcmp(cb->event, event) != 0) continue;
        
        params_t* params = params_create(3);
        if (!params) {
            LOG("ERR: params_create failed");
            continue;
        }
        
        /* 如果是订阅全部，第一个参数是事件名称 */
        if (cb->event[0] == '\0') {
            params_push_string(params, event, 0);
            params_push_int(params, data1);
            if (data2 && data2_len > 0) {
                params_push_string(params, data2, data2_len);
            } else {
                params_push_nil(params);
            }
        } else {
            /* 订阅指定事件，只传递数据 */
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

/* socket 事件回调 */
static void socket_event_callback(net_socket_t* sock, net_event_type_t event, void* user_data) {
    socket_lua_ctx_t* ctx = (socket_lua_ctx_t*)user_data;
    if (!ctx) return;
    
    const char* event_name = NULL;
    int data1 = 0;
    const char* data2 = NULL;
    size_t data2_len = 0;
    
    switch (event) {
        case NET_EVENT_CONNECTED:
            event_name = "connect";
            break;
            
        case NET_EVENT_DISCONNECTED:
            event_name = "disconnect";
            break;
            
        case NET_EVENT_ACCEPT:
            event_name = "accept";
            break;
            
        case NET_EVENT_RECV: {
            event_name = "recv";
            /* 读取所有可用数据 */
            char buf[4096];
            int len;
            while ((len = net_socket_recv((sock_t)sock, buf, sizeof(buf) - 1)) > 0) {
                buf[len] = '\0';
                socket_send_event(ctx, event_name, len, buf, len);
            }
            return;  /* 事件已处理 */
        }
            
        case NET_EVENT_SEND:
            event_name = "send";
            break;
            
        case NET_EVENT_ERROR:
            event_name = "error";
            data1 = -1;
            break;
            
        default:
            return;
    }
    
    if (event_name) {
        socket_send_event(ctx, event_name, data1, data2, data2_len);
    }
}

/* 添加到全局链表 */
static void socket_add_to_list(socket_lua_ctx_t* ctx) {
    iot_mutex_lock(g_socket_mutex, -1);
    ctx->next = g_socket_list;
    g_socket_list = ctx;
    iot_mutex_unlock(g_socket_mutex);
}

/* 从全局链表移除 */
static void socket_remove_from_list(socket_lua_ctx_t* ctx) {
    iot_mutex_lock(g_socket_mutex, -1);
    
    if (g_socket_list == ctx) {
        g_socket_list = ctx->next;
    } else {
        socket_lua_ctx_t* prev = g_socket_list;
        while (prev && prev->next != ctx) {
            prev = prev->next;
        }
        if (prev) {
            prev->next = ctx->next;
        }
    }
    
    iot_mutex_unlock(g_socket_mutex);
}

/*===========================================================
 * Lua 方法实现
 *===========================================================*/

/**
 * @brief 创建 socket
 * @api net.socket(domain, type, protocol)
 * @int domain 协议族，默认 net.AF_INET
 * @int type 类型，默认 net.SOCK_STREAM
 * @int protocol 协议，默认 net.IPPROTO_TCP
 * @return socket 对象，失败返回 nil
 * @usage
 * local sock = net.socket()
 * local tcp_sock = net.socket(net.AF_INET, net.SOCK_STREAM, net.IPPROTO_TCP)
 * local udp_sock = net.socket(net.AF_INET, net.SOCK_DGRAM, net.IPPROTO_UDP)
 */
static int iot_net_socket_create(lua_State* L) {
    lua_module_init();
    
    int domain = (int)luaL_optinteger(L, 1, 2);      /* 默认 AF_INET = 2 */
    int type = (int)luaL_optinteger(L, 2, 1);        /* 默认 SOCK_STREAM = 1 */
    int protocol = (int)luaL_optinteger(L, 3, 6);   /* 默认 IPPROTO_TCP = 6 */
    
    /* 创建上下文 */
    socket_lua_ctx_t* ctx = socket_ctx_create();
    if (!ctx) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to create socket context");
        return 2;
    }
    
    /* 创建 socket */
    net_sock_type_t sock_type = (type == 1) ? SOCK_TYPE_STREAM : SOCK_TYPE_DGRAM;
    ctx->sock = (net_socket_t*)net_socket_create(sock_type, socket_event_callback, ctx);
    if (!ctx->sock) {
        iot_free(ctx);
        lua_pushnil(L);
        lua_pushstring(L, "failed to create socket");
        return 2;
    }
    
    ctx->domain = domain;
    ctx->type = type;
    ctx->protocol = protocol;
    
    /* 添加到链表 */
    socket_add_to_list(ctx);
    
    /* 创建 userdata 对象 */
    socket_lua_ctx_t** ctx_ptr = (socket_lua_ctx_t**)lua_newuserdata(L, sizeof(socket_lua_ctx_t*));
    *ctx_ptr = ctx;
    
    /* 设置元表 */
    luaL_getmetatable(L, "net.socket");
    lua_setmetatable(L, -2);
    
    return 1;
}

/**
 * @brief 订阅 socket 事件
 * @api sock:on([event,] callback)
 * @string event 事件名称(可空)，如"connect"、"recv"、"error"等。不传则订阅全部事件
 * @function callback 事件回调函数
 * @return bool 成功返回 true
 * @usage
 * -- 订阅全部事件
 * sock:on(function(event, data1, data2)
 *     print("event:", event, data1, data2)
 * end)
 * 
 * -- 订阅指定事件
 * sock:on("connect", function(data1, data2)
 *     print("connected")
 * end)
 */
static int iot_socket_on(lua_State* L) {
    /* 获取 socket 上下文（self） */
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid socket");
        return 2;
    }
    
    const char* event = NULL;
    int callback_idx = 2;
    
    /* 判断参数形式：sock + callback 或 sock + event + callback */
    if (lua_type(L, 2) == LUA_TSTRING) {
        event = luaL_checkstring(L, 2);
        callback_idx = 3;
    }
    
    if (lua_type(L, callback_idx) != LUA_TFUNCTION) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "callback must be a function");
        return 2;
    }
    
    /* 查找空闲回调插槽 */
    int cb_slot = socket_find_free_event_cb(ctx);
    if (cb_slot < 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "no free event callback slot");
        return 2;
    }
    
    /* 保存回调 */
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

/**
 * @brief 连接服务器
 * @api sock:connect(host, port)
 * @string host 服务器地址
 * @int port 服务器端口
 * @return bool 成功返回 true
 * @usage
 * sock:connect("example.com", 8080)
 */
static int iot_socket_connect(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->sock) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid socket");
        return 2;
    }
    
    const char* host = luaL_checkstring(L, 2);
    int port = (int)luaL_checkinteger(L, 3);
    
    int ret = net_socket_connect((sock_t)ctx->sock, host, port);
    if (ret < 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "connect failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 发送数据
 * @api sock:send(data)
 * @string data 要发送的数据
 * @return int 发送的字节数
 * @usage
 * sock:send("hello world")
 */
static int iot_socket_send(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->sock) {
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
 * @brief 接收数据
 * @api sock:recv(buf_len)
 * @int buf_len 缓冲区长度
 * @return string 接收到的数据，超时返回 nil
 * @usage
 * local data = sock:recv(1024)
 */
static int iot_socket_recv(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->sock) {
        lua_pushnil(L);
        return 1;
    }
    
    int buf_len = (int)luaL_optinteger(L, 2, 1024);
    
    char* buf = (char*)iot_malloc(buf_len);
    if (!buf) {
        lua_pushnil(L);
        return 1;
    }
    
    int len = net_socket_recv((sock_t)ctx->sock, buf, buf_len);
    if (len > 0) {
        lua_pushlstring(L, buf, len);
    } else {
        lua_pushnil(L);
    }
    
    iot_free(buf);
    return 1;
}

/**
 * @brief 发送文本数据（Lua 字符串直接发送）
 * @api sock:write(data)
 * @string data 要发送的数据
 * @return int 发送的字节数
 * @usage
 * sock:write("hello")
 */
static int iot_socket_write(lua_State* L) {
    return iot_socket_send(L);
}

/**
 * @brief 读取文本数据
 * @api sock:read(max_len)
 * @int max_len 最大读取长度
 * @return string 读取到的数据
 * @usage
 * local data = sock:read(1024)
 */
static int iot_socket_read(lua_State* L) {
    return iot_socket_recv(L);
}

/**
 * @brief 绑定地址
 * @api sock:bind(host, port)
 * @string host IP 地址
 * @int port 端口号
 * @return bool 成功返回 true
 * @usage
 * server:bind("0.0.0.0", 8080)
 */
static int iot_socket_bind(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->sock) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid socket");
        return 2;
    }
    
    const char* host = luaL_optstring(L, 2, NULL);
    int port = (int)luaL_optinteger(L, 3, 0);
    
    int ret = net_socket_bind((sock_t)ctx->sock, host, port);
    if (ret < 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "bind failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 监听连接
 * @api sock:listen(backlog)
 * @int backlog 最大监听队列长度
 * @return bool 成功返回 true
 * @usage
 * server:listen(5)
 */
static int iot_socket_listen(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->sock) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid socket");
        return 2;
    }
    
    int backlog = (int)luaL_optinteger(L, 2, 5);
    
    int ret = net_socket_listen((sock_t)ctx->sock, backlog);
    if (ret < 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "listen failed");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 接受连接
 * @api sock:accept(callback)
 * @function callback 接受到连接时的回调函数 function(client_sock)
 * @return bool 成功返回 true
 * @usage
 * server:accept(function(client_sock)
 *     client_sock:on("recv", function(len, data)
 *         print("received:", data)
 *     end)
 * end)
 */
static int iot_socket_accept(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->sock) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid socket");
        return 2;
    }
    
    if (lua_type(L, 2) != LUA_TFUNCTION) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "callback must be a function");
        return 2;
    }
    
    /* 创建客户端 socket */
    socket_lua_ctx_t* client_ctx = socket_ctx_create();
    if (!client_ctx) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "failed to create client socket");
        return 2;
    }
    
    client_ctx->sock = (net_socket_t*)net_socket_accept((sock_t)ctx->sock, socket_event_callback, client_ctx);
    if (!client_ctx->sock) {
        iot_free(client_ctx);
        lua_pushboolean(L, 0);
        lua_pushstring(L, "accept failed");
        return 2;
    }
    
    /* 保存回调 */
    void* cb_ud = iot_callback_save(L, 2);
    if (!cb_ud) {
        net_socket_close((sock_t)client_ctx->sock);
        iot_free(client_ctx);
        lua_pushboolean(L, 0);
        lua_pushstring(L, "failed to save callback");
        return 2;
    }
    
    client_ctx->event_cbs[0].callback_ud = cb_ud;
    client_ctx->event_cbs[0].event[0] = '\0';
    client_ctx->event_cbs[0].inited = 1;
    
    /* 添加到链表 */
    socket_add_to_list(client_ctx);
    
    /* 创建 userdata */
    socket_lua_ctx_t** ctx_ptr = (socket_lua_ctx_t**)lua_newuserdata(L, sizeof(socket_lua_ctx_t*));
    *ctx_ptr = client_ctx;
    luaL_getmetatable(L, "net.socket");
    lua_setmetatable(L, -2);
    
    /* 调用回调 */
    lua_pushvalue(L, -1);
    iot_callback_call(cb_ud, NULL);
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 关闭 socket
 * @api sock:close()
 * @usage
 * sock:close()
 */
static int iot_socket_close(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) {
        return 0;
    }
    
    /* 从链表中移除 */
    socket_remove_from_list(ctx);
    
    /* 销毁上下文 */
    socket_ctx_destroy(ctx);
    
    return 0;
}

/**
 * @brief 获取 socket 状态
 * @api sock:state()
 * @return int socket 状态
 * @usage
 * local state = sock:state()
 */
static int iot_socket_state(lua_State* L) {
    socket_lua_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx || !ctx->sock) {
        lua_pushinteger(L, NET_SOCK_STATE_CLOSED);
        return 1;
    }
    
    net_sock_state_t state = net_socket_get_state((sock_t)ctx->sock);
    lua_pushinteger(L, state);
    return 1;
}

/**
 * @brief DNS 解析
 * @api net.lookup(host, callback)
 * @string host 主机名
 * @function callback 解析完成回调 function(ip)
 * @return bool 成功返回 true
 * @usage
 * net.lookup("example.com", function(ip)
 *     print("IP:", ip)
 * end)
 */
static int iot_net_lookup(lua_State* L) {
    const char* host = luaL_checkstring(L, 1);

    if (lua_type(L, 2) != LUA_TFUNCTION) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "callback must be a function");
        return 2;
    }

    char ip[32] = {0};
    int ret = net_gethostbyname(host, ip);
    if (ret == 0) {
        lua_pushboolean(L, 1);
        /* 直接调用回调 */
        lua_pushvalue(L, 2);
        lua_pushstring(L, ip);
        lua_call(L, 1, 0);
    } else {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "lookup failed");
        return 2;
    }

    return 1;
}

/*===========================================================
 * Lua 模块定义
 *===========================================================*/

/* socket 方法表 */
static const luaL_Reg socket_methods[] = {
    { "on",       iot_socket_on },      /* 订阅事件 */
    { "connect",  iot_socket_connect }, /* 连接 */
    { "send",     iot_socket_send },    /* 发送 */
    { "recv",     iot_socket_recv },    /* 接收 */
    { "write",    iot_socket_write },   /* 写 */
    { "read",     iot_socket_read },    /* 读 */
    { "bind",     iot_socket_bind },    /* 绑定 */
    { "listen",   iot_socket_listen },  /* 监听 */
    { "accept",   iot_socket_accept },  /* 接受 */
    { "close",    iot_socket_close },   /* 关闭 */
    { "state",    iot_socket_state },   /* 状态 */
    { "__gc",     iot_socket_close },   /* 垃圾回收 */
    { "__tostring", iot_socket_close }, /* 字符串转换 */
    { NULL,       NULL }
};

/* net 模块方法表 */
static const luaL_Reg net_methods[] = {
    { "socket", iot_net_socket_create }, /* 创建 socket */
    { "lookup", iot_net_lookup },        /* DNS 解析 */
    { NULL,     NULL }
};

/* 常量定义 */
static const luaL_Const net_constants[] = {
    { "AF_INET",      2 },           /* IPv4 */
    { "AF_INET6",     10 },          /* IPv6 */
    { "SOCK_STREAM",  1 },           /* TCP */
    { "SOCK_DGRAM",   2 },           /* UDP */
    { "IPPROTO_TCP",  6 },           /* TCP 协议 */
    { "IPPROTO_UDP",  17 },          /* UDP 协议 */
    
    /* socket 状态 */
    { "CLOSED",       NET_SOCK_STATE_CLOSED },
    { "OPENED",       NET_SOCK_STATE_OPENED },
    { "LISTENING",    NET_SOCK_STATE_LISTENING },
    { "CONNECTING",   NET_SOCK_STATE_CONNECTING },
    { "CONNECTED",    NET_SOCK_STATE_CONNECTED },
    { "ERROR",        NET_SOCK_STATE_ERROR },
    
    /* 事件类型 */
    { "EVENT_CONNECTED",   NET_EVENT_CONNECTED },
    { "EVENT_DISCONNECTED", NET_EVENT_DISCONNECTED },
    { "EVENT_ACCEPT",      NET_EVENT_ACCEPT },
    { "EVENT_RECV",        NET_EVENT_RECV },
    { "EVENT_SEND",        NET_EVENT_SEND },
    { "EVENT_ERROR",       NET_EVENT_ERROR },
    
    { NULL, 0 }
};

/* 模块初始化 */
LUAMOD_API int luaopen_net(lua_State* L) {
    lua_module_init();
    
    /* 创建 net 模块 */
    luaL_newlib(L, net_methods);
    
    /* 注册常量 */
    const luaL_Const* constant = net_constants;
    for (; constant->name; constant++) {
        lua_pushinteger(L, constant->value);
        lua_setfield(L, -2, constant->name);
    }
    
    /* 创建 socket 元表 */
    luaL_newmetatable(L, "net.socket");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    
    /* 注册 socket 方法 */
    luaL_setfuncs(L, socket_methods, 0);
    
    return 1;
}
