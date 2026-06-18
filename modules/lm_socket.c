/*
@module  socket
@summary 网络socket(异步事件驱动,支持SSL)
@version 4.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     SOCKET
*/

/*
Socket参考示例
-- 创建普通socket对象
local sock = socket.create(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)

-- 订阅全部事件
sock:on(function(event, data1, data2)
    if event == "connect" then
        print("连接成功")
        sock:write("hello")
    elseif event == "recv" then
        print("收到数据:", data2)
    elseif event == "error" then
        print("错误:", data1)
    end
end)

-- 或订阅指定事件
sock:on("connect", function(data1, data2)
    print("连接成功")
end)

-- 连接服务器
sock:connect("example.com", 8080)

-- SSL连接示例
local ssl_sock = socket.create(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)

ssl_sock:on(function(event, data1, data2)
    if event == "connect" then
        print("连接成功")
        -- 启动SSL握手
        ssl_sock:ssl_connect({
            verify = socket.SSL_VERIFY_NONE,
        })
    elseif event == "ssl_ready" then
        print("SSL握手成功")
        ssl_sock:write("hello")
    elseif event == "recv" then
        print("收到数据:", data2)
    elseif event == "error" then
        print("错误:", data1)
    end
end)

ssl_sock:connect("example.com", 443)

-- 发送数据
local len = sock:write("hello")

-- 读取数据(在回调中自动处理或主动读取)
local data = sock:read(1024)

-- 设置socket选项
sock:setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)

-- 关闭socket
sock:close()
*/

#include "iot_base.h"
#include "iot_callback.h"
#include "cm_asocket.h"
#include "cm_ssl.h"
#include "lwip/sockets.h"
#include "iot_rtos.h"
#include "mbedtls/ssl.h"

/* SSL验证方式 */
typedef enum {
    SSL_VERIFY_NONE = 0,    /* 不验证 */
    SSL_VERIFY_SERVER = 1,  /* 单向验证 */
    SSL_VERIFY_BOTH = 2,    /* 双向验证 */
} ssl_verify_mode_e;

/* 事件回调结构 */
typedef struct {
    void* callback_ud;      /* Lua回调函数userdata */
    char event[16];         /* 事件名称，空字符串表示订阅全部 */
    int inited;             /* 是否已初始化 */
} socket_event_cb_t;

#define SOCKET_EVENT_CB_MAX 8  /* 每个socket最多支持8个事件回调 */

/* Socket上下文 */
typedef struct socket_ctx {
    int sock;               /* socket描述符 */
    int domain;             /* 协议族 */
    int type;               /* 类型 */
    int protocol;           /* 协议 */
    socket_event_cb_t event_cbs[SOCKET_EVENT_CB_MAX];  /* 事件回调数组 */
    
    /* SSL相关 */
    void* ssl_ctx;          /* SSL上下文 */
    int ssl_id;             /* SSL通道ID */
    int ssl_connected;      /* SSL是否已连接 */
    int ssl_shaking;        /* 是否正在SSL握手 */
    int use_ssl;            /* 是否使用SSL */
    
    /* 链表节点 */
    struct socket_ctx* next;
} socket_ctx_t;

/* Socket全局链表 */
static socket_ctx_t* g_socket_list = NULL;

/* 事件类型映射到Lua字符 */
static const char* socket_event_to_str(cm_asocket_event_e event) {
    switch (event) {
        case CM_ASOCKET_EV_CONNECT_OK:   return "connect";
        case CM_ASOCKET_EV_CONNECT_FAIL: return "connect_fail";
        case CM_ASOCKET_EV_RECV_IND:     return "recv";
        case CM_ASOCKET_EV_SEND_IND:     return "send";
        case CM_ASOCKET_EV_ACCEPT_IND:   return "accept";
        case CM_ASOCKET_EV_ERROR_IND:    return "error";
        case CM_ASOCKET_EV_SEQ_IND:      return "seq";
        case CM_ASOCKET_EV_DROP_IND:     return "drop";
        default: return "unknown";
    }
}

/* 通过socket描述符查找上下文 */
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

/* 创建新的socket上下文 */
static socket_ctx_t* socket_ctx_create(void) {
    socket_ctx_t* ctx = (socket_ctx_t*)iot_malloc(sizeof(socket_ctx_t));
    if (!ctx) {
        LOG("ERR malloc socket ctx failed");
        return NULL;
    }
    memset(ctx, 0, sizeof(socket_ctx_t));
    ctx->sock = -1;
    ctx->ssl_id = -1;
    ctx->next = NULL;
    return ctx;
}

/* 销毁socket上下文 */
static void socket_ctx_destroy(socket_ctx_t* ctx) {
    if (!ctx) return;
    
    /* 清理所有事件回调 */
    for (int i = 0; i < SOCKET_EVENT_CB_MAX; i++) {
        if (ctx->event_cbs[i].inited && ctx->event_cbs[i].callback_ud) {
            iot_callback_free(ctx->event_cbs[i].callback_ud);
        }
    }
    
    /* 关闭SSL连接 */
    if (ctx->ssl_ctx) {
        cm_ssl_close(&ctx->ssl_ctx);
    }
    
    /* 从链表中移除 */
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
    
    iot_free(ctx);
}

/* 查找空闲的事件回调槽 */
static int socket_find_free_event_cb(socket_ctx_t* ctx) {
    if (!ctx) return -1;
    for (int i = 0; i < SOCKET_EVENT_CB_MAX; i++) {
        if (!ctx->event_cbs[i].inited) {
            return i;
        }
    }
    return -1;
}

/* 发送事件到Lua回调 */
static void socket_send_event(socket_ctx_t* ctx, const char* event, int data1, const char* data2, int data2_len) {
    if (!ctx || !event) return;
    
    for (int i = 0; i < SOCKET_EVENT_CB_MAX; i++) {
        socket_event_cb_t* cb = &ctx->event_cbs[i];
        if (!cb->inited || !cb->callback_ud) continue;
        
        /* 检查是否匹配事件：空字符串表示订阅全部 */
        if (cb->event[0] != '\0' && strcmp(cb->event, event) != 0) continue;
        
        params_t* params = params_create(3);
        if (!params) {
            LOG("ERR params_create failed");
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

/* SSL读取数据 */
static void socket_ssl_read_data(socket_ctx_t* ctx) {
    if (!ctx || !ctx->ssl_ctx || !ctx->ssl_connected) return;
    
    /* 先执行一次空读取 */
    cm_ssl_read(ctx->ssl_ctx, NULL, 0);
    
    /* 获取可读数据长度 */
    int avail = cm_ssl_get_bytes_avail(ctx->ssl_ctx);
    if (avail <= 0) {
        /* 检查是否有未读数据 */
        if (cm_ssl_check_pending(ctx->ssl_ctx)) {
            avail = 4096; /* 默认缓冲区大小 */
        } else {
            return;
        }
    }
    
    char* buf = (char*)iot_malloc(avail);
    if (!buf) {
        LOG("ERR ssl malloc failed");
        return;
    }
    
    /* 循环读取所有数据 */
    while (1) {
        int ret = cm_ssl_read(ctx->ssl_ctx, buf, avail);
        if (ret > 0) {
            socket_send_event(ctx, "recv", ret, buf, ret);
        } else {
            break;
        }
        
        if (cm_ssl_check_pending(ctx->ssl_ctx) == 0) {
            break;
        }
    }
    
    iot_free(buf);
}

/* 异步socket事件回调(在eloop线程中执行) */
static void socket_event_cb(int sock, cm_asocket_event_e event, void *ev_param, void *cb_param) {
    socket_ctx_t* ctx = (socket_ctx_t*)cb_param;
    if (!ctx || ctx->sock != sock) {
        LOG("ERR ctx not match sock=%d", sock);
        return;
    }

    int has_callback = 0;
    for (int i = 0; i < SOCKET_EVENT_CB_MAX; i++) {
        if (ctx->event_cbs[i].inited && ctx->event_cbs[i].callback_ud) {
            has_callback = 1;
            break;
        }
    }
    if (!has_callback) {
        LOG("WARN no callback for sock=%d event=%d", sock, event);
        return;
    }

    /* SSL握手期间不处理读取事件 */
    if (ctx->ssl_shaking && event == CM_ASOCKET_EV_RECV_IND) {
        LOG("ssl shaking hands, ignore recv");
        return;
    }

    /* 如果是SSL连接且已握手完成，使用SSL读取 */
    if (ctx->use_ssl && ctx->ssl_connected && event == CM_ASOCKET_EV_RECV_IND) {
        socket_ssl_read_data(ctx);
        return;
    }

    /* 发送事件到所有匹配的回调 */
    const char* event_str = socket_event_to_str(event);
    int data1 = 0;
    char* data2 = NULL;
    int data2_len = 0;

    switch (event) {
        case CM_ASOCKET_EV_CONNECT_OK:
            data1 = 1;
            break;

        case CM_ASOCKET_EV_CONNECT_FAIL:
            data1 = (int)(uintptr_t)ev_param;
            break;

        case CM_ASOCKET_EV_RECV_IND: {
            /* 获取可读数据长度 */
            int recv_avail = 0;
            if (cm_asocket_ioctl(sock, FIONREAD, &recv_avail) == 0 && recv_avail > 0) {
                data2 = (char*)iot_malloc(recv_avail);
                if (data2) {
                    int ret = cm_asocket_recv(sock, data2, recv_avail, 0);
                    if (ret > 0) {
                        data1 = ret;
                        data2_len = ret;
                    } else {
                        data1 = ret;
                        data2_len = 0;
                        iot_free(data2);
                        data2 = NULL;
                    }
                }
            }
            break;
        }

        case CM_ASOCKET_EV_SEND_IND:
            data1 = (int)(uintptr_t)ev_param;
            break;

        case CM_ASOCKET_EV_ACCEPT_IND:
            data1 = (int)(uintptr_t)ev_param;
            break;

        case CM_ASOCKET_EV_ERROR_IND: {
            /* 获取socket错误码 */
            int sock_error = 0;
            socklen_t opt_len = sizeof(sock_error);
            cm_asocket_getsockopt(sock, SOL_SOCKET, SO_ERROR, &sock_error, &opt_len);
            data1 = sock_error;
            break;
        }

        default:
            break;
    }

    socket_send_event(ctx, event_str, data1, data2, data2_len);
    
    if (data2) {
        iot_free(data2);
    }
}

/* 从Lua userdata获取socket上下文 */
static socket_ctx_t* socket_get_ctx_from_userdata(lua_State* L, int idx) {
    socket_ctx_t** ctx_ptr = (socket_ctx_t**)luaL_checkudata(L, idx, "socket");
    if (!ctx_ptr || !*ctx_ptr) {
        LOG("ERR invalid socket userdata");
        lua_pushnil(L);
        lua_pushstring(L, "invalid socket");
        return NULL;
    }
    return *ctx_ptr;
}

/**
 * @brief 创建socket
 * @api socket.create(domain, type, protocol)
 * @int domain 协议族 socket.AF_INET, socket.AF_INET6
 * @int type 类型 socket.SOCK_STREAM, socket.SOCK_DGRAM
 * @int protocol 协议 socket.IPPROTO_TCP, socket.IPPROTO_UDP
 * @return userdata socket对象,失败返回nil和错误信息
 * @usage
 * local sock = socket.create(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
 */
static int iot_socket_create(lua_State* L) {
    int domain = luaL_checkinteger(L, 1);
    int type = luaL_checkinteger(L, 2);
    int protocol = luaL_optinteger(L, 3, 0);

    /* 创建动态上下文 */
    socket_ctx_t* ctx = socket_ctx_create();
    if (!ctx) {
        LOG("ERR create ctx failed");
        lua_pushnil(L);
        lua_pushstring(L, "failed to create socket context");
        return 2;
    }

    /* 创建异步socket */
    int sock = cm_asocket_open(domain, type, protocol, socket_event_cb, (void*)ctx);
    if (sock < 0) {
        LOG("ERR ret=%d", sock);
        socket_ctx_destroy(ctx);
        lua_pushnil(L);
        lua_pushstring(L, "socket create failed");
        return 2;
    }

    /* 初始化上下文 */
    ctx->sock = sock;
    ctx->domain = domain;
    ctx->type = type;
    ctx->protocol = protocol;

    /* 添加到链表 */
    ctx->next = g_socket_list;
    g_socket_list = ctx;

    /* 创建userdata对象 */
    socket_ctx_t** ctx_ptr = (socket_ctx_t**)lua_newuserdata(L, sizeof(socket_ctx_t*));
    *ctx_ptr = ctx;
    
    /* 设置元表 */
    luaL_getmetatable(L, "socket");
    lua_setmetatable(L, -2);

    LOG("OK sock=%d", sock);
    return 1;
}

/**
 * @brief 订阅socket事件
 * @api sock:on([event,] callback)
 * @string event 事件名称(可空)，如"connect"、"recv"、"error"等。不传则订阅全部事件
 * @function callback 事件回调函数
 * @return bool 成功返回true
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
    /* 获取socket上下文 */
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    const char* event = NULL;
    int callback_idx = 2;
    
    /* 判断参数形式：sock + callback 或 sock + event + callback */
    if (lua_type(L, 2) == LUA_TSTRING) {
        event = luaL_checkstring(L, 2);
        callback_idx = 3;
    }
    
    luaL_checktype(L, callback_idx, LUA_TFUNCTION);
    
    /* 查找空闲回调插槽 */
    int cb_slot = socket_find_free_event_cb(ctx);
    if (cb_slot < 0) {
        LOG("ERR no free event cb slot");
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

    LOG("OK sock=%d event=%s cb_slot=%d", ctx->sock, event ? event : "(all)", cb_slot);
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 连接服务器
 * @api sock:connect(host, port)
 * @string host 服务器地址
 * @int port 服务器端口
 * @return bool 成功返回true
 * @usage
 * sock:connect("example.com", 8080)
 */
static int iot_socket_connect(lua_State* L) {
    /* 获取socket上下文 */
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    const char* host = luaL_checkstring(L, 2);
    int port = luaL_checkinteger(L, 3);

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(host);

    int ret = cm_asocket_connect(ctx->sock, (struct sockaddr*)&addr, sizeof(addr));
    if (ret < 0 && errno != EINPROGRESS) {
        LOG("ERR ret=%d errno=%d", ret, errno);
        lua_pushboolean(L, 0);
        lua_pushinteger(L, errno);
        return 2;
    }

    LOG("OK sock=%d host=%s port=%d", ctx->sock, host, port);
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief SSL连接(握手)
 * @api sock:ssl_connect(config)
 * @table config SSL配置表
 *   @int verify 验证方式: socket.SSL_VERIFY_NONE(0), socket.SSL_VERIFY_SERVER(1), socket.SSL_VERIFY_BOTH(2)
 *   @string ca_cert CA证书文件(可空)
 *   @string client_cert 客户端证书文件名(可空)
 *   @string client_key 客户端密钥文件名(可空)
 *   @int timeout 握手超时时间(秒),默认60
 *   @int session 是否开启会话恢复,0/1,默认1
 *   @int sni 是否开启SNI,0/1,默认0
 *   @int version SSL版本,3=TLS1.2,255=全部,默认255
 * @return bool 成功返回true
 * @usage
 * sock:ssl_connect({
 *     verify = socket.SSL_VERIFY_NONE,
 *     timeout = 60,
 * })
 */
static int iot_socket_ssl_connect(lua_State* L) {
    /* 获取socket上下文 */
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    luaL_checktype(L, 2, LUA_TTABLE);
    
    /* 获取SSL通道ID(0-5) - 使用socket描述符模6 */
    int ssl_id = ctx->sock % 6;
    
    /* 获取配置 */
    lua_getfield(L, 2, "verify");
    int verify = luaL_optinteger(L, -1, 0);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "timeout");
    uint16_t negotime = (uint16_t)luaL_optinteger(L, -1, 60);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "session");
    uint8_t session = (uint8_t)luaL_optinteger(L, -1, 1);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "sni");
    uint8_t sni = (uint8_t)luaL_optinteger(L, -1, 0);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "version");
    uint8_t version = (uint8_t)luaL_optinteger(L, -1, 255);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "ca_cert");
    const char* ca_cert = luaL_optstring(L, -1, NULL);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "client_cert");
    const char* client_cert = luaL_optstring(L, -1, NULL);
    lua_pop(L, 1);
    
    lua_getfield(L, 2, "client_key");
    const char* client_key = luaL_optstring(L, -1, NULL);
    lua_pop(L, 1);
    
    /* 设置SSL参数 */
    if (ca_cert) {
        cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CA_CERT_FILENAME, (void*)ca_cert);
    }
    if (client_cert) {
        cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CLI_CERT_FILENAME, (void*)client_cert);
    }
    if (client_key) {
        cm_ssl_setopt(ssl_id, CM_SSL_PARAM_CLI_KEY_FILENAME, (void*)client_key);
    }
    
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERIFY, (void*)(uintptr_t)verify);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_NEGOTIME, (void*)(uintptr_t)negotime);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_SESSION, (void*)(uintptr_t)session);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_SNI, (void*)(uintptr_t)sni);
    cm_ssl_setopt(ssl_id, CM_SSL_PARAM_VERSION, (void*)(uintptr_t)version);
    
    /* 保存SSL配置 */
    ctx->ssl_id = ssl_id;
    ctx->use_ssl = 1;
    
    /* 开始SSL握手(同步接口) */
    ctx->ssl_shaking = 1;
    int ret = cm_ssl_conn(&ctx->ssl_ctx, ssl_id, ctx->sock, 0);
    ctx->ssl_shaking = 0;
    
    if (ret != 0) {
        LOG("ERR ssl conn failed ret=%d", ret);
        ctx->use_ssl = 0;
        ctx->ssl_id = -1;
        lua_pushboolean(L, 0);
        lua_pushinteger(L, ret);
        return 2;
    }
    
    ctx->ssl_connected = 1;
    LOG("OK ssl connected sock=%d ssl_id=%d", ctx->sock, ssl_id);
    
    /* 发送SSL_READY事件 */
    socket_send_event(ctx, "ssl_ready", 1, NULL, 0);
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 发送数据
 * @api sock:write(data)
 * @string data 要发送的数据
 * @return int 发送的字节数，失败返回nil和错误码
 * @usage
 * local len = sock:write("hello")
 */
static int iot_socket_write(lua_State* L) {
    /* 获取socket上下文 */
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    int ret;
    
    /* SSL模式使用SSL连接 */
    if (ctx->use_ssl && ctx->ssl_connected && ctx->ssl_ctx) {
        int send_len = 0;
        while (send_len < (int)len) {
            ret = cm_ssl_write(ctx->ssl_ctx, (void*)(data + send_len), len - send_len);
            if (ret == MBEDTLS_ERR_SSL_WANT_WRITE || ret == MBEDTLS_ERR_SSL_WANT_READ) {
                continue;
            }
            if (ret < 0) {
                LOG("ERR ssl write ret=%d", ret);
                lua_pushnil(L);
                lua_pushinteger(L, ret);
                return 2;
            }
            send_len += ret;
        }
        ret = send_len;
    } else {
        /* 普通模式 */
        ret = cm_asocket_send(ctx->sock, data, len, 0);
        if (ret < 0) {
            LOG("ERR ret=%d errno=%d", ret, errno);
            lua_pushnil(L);
            lua_pushinteger(L, errno);
            return 2;
        }
    }

    LOG("OK sock=%d len=%d", ctx->sock, ret);
    lua_pushinteger(L, ret);
    return 1;
}

/**
 * @brief 读取数据
 * @api sock:read(maxlen)
 * @int maxlen 最大读取长度
 * @return string 读取的数据，失败返回nil和错误码
 * @usage
 * local data = sock:read(1024)
 */
static int iot_socket_read(lua_State* L) {
    /* 获取socket上下文 */
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
        LOG("ERR malloc failed");
        lua_pushnil(L);
        lua_pushstring(L, "malloc failed");
        return 2;
    }

    int ret;
    
    /* SSL模式使用SSL读取 */
    if (ctx->use_ssl && ctx->ssl_connected && ctx->ssl_ctx) {
        ret = cm_ssl_read(ctx->ssl_ctx, buf, maxlen);
        if (ret < 0) {
            LOG("ERR ssl read ret=%d", ret);
            iot_free(buf);
            lua_pushnil(L);
            lua_pushinteger(L, ret);
            return 2;
        }
    } else {
        /* 普通模式 */
        ret = cm_asocket_recv(ctx->sock, buf, maxlen, 0);
        if (ret < 0) {
            LOG("ERR ret=%d errno=%d", ret, errno);
            iot_free(buf);
            lua_pushnil(L);
            lua_pushinteger(L, errno);
            return 2;
        }
    }

    if (ret == 0) {
        iot_free(buf);
        lua_pushstring(L, "");
        return 1;
    }

    LOG("OK sock=%d len=%d", ctx->sock, ret);
    lua_pushlstring(L, buf, ret);
    iot_free(buf);
    return 1;
}

/**
 * @brief 关闭socket
 * @api sock:close()
 * @return bool 成功返回true
 * @usage
 * sock:close()
 */
static int iot_socket_close(lua_State* L) {
    /* 获取socket上下文 */
    socket_ctx_t** ctx_ptr = (socket_ctx_t**)luaL_checkudata(L, 1, "socket");
    if (!ctx_ptr || !*ctx_ptr) {
        LOG("ERR invalid socket userdata");
        lua_pushboolean(L, 0);
        return 1;
    }
    
    socket_ctx_t* ctx = *ctx_ptr;
    int sock = ctx->sock;

    /* 关闭socket */
    int ret = cm_asocket_close(sock);

    /* 销毁上下文(自动清理SSL连接和回调) */
    socket_ctx_destroy(ctx);
    
    /* 设置userdata为NULL表示已关闭 */
    *ctx_ptr = NULL;

    LOG("close sock=%d ret=%d", sock, ret);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 设置socket选项
 * @api sock:setsockopt(level, optname, optval)
 * @int level 选项级别
 * @int optname 选项名称
 * @int optval 选项值
 * @return bool 成功返回true
 * @usage
 * sock:setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)
 */
static int iot_socket_setsockopt(lua_State* L) {
    /* 获取socket上下文 */
    socket_ctx_t* ctx = socket_get_ctx_from_userdata(L, 1);
    if (!ctx) return 2;
    
    int level = luaL_checkinteger(L, 2);
    int optname = luaL_checkinteger(L, 3);
    int optval = luaL_checkinteger(L, 4);

    int ret = cm_asocket_setsockopt(ctx->sock, level, optname, &optval, sizeof(optval));
    if (ret < 0) {
        LOG("ERR ret=%d errno=%d", ret, errno);
        lua_pushboolean(L, 0);
        lua_pushinteger(L, errno);
        return 2;
    }

    lua_pushboolean(L, 1);
    return 1;
}

/* Socket对象方法 */
static const luaL_Reg socket_methods[] = {
    { "on",           iot_socket_on },
    { "connect",      iot_socket_connect },
    { "ssl_connect",  iot_socket_ssl_connect },
    { "write",        iot_socket_write },
    { "read",         iot_socket_read },
    { "close",        iot_socket_close },
    { "setsockopt",   iot_socket_setsockopt },
    { NULL, NULL }
};

/* Socket对象元表的__gc方法 */
static int iot_socket_gc(lua_State* L) {
    socket_ctx_t** ctx_ptr = (socket_ctx_t**)luaL_checkudata(L, 1, "socket");
    if (ctx_ptr && *ctx_ptr) {
        socket_ctx_destroy(*ctx_ptr);
        *ctx_ptr = NULL;
    }
    return 0;
}

/* Socket对象元表 */
static const luaL_Reg socket_metatable[] = {
    { "__gc",         iot_socket_gc },
    { NULL, NULL }
};

/* Socket模块方法表 */
static const luaL_Reg socket_lib[] = {
    { "create",       iot_socket_create },
    { NULL, NULL }
};

/**
 * @brief 注册socket模块到Lua
 */
LUAMOD_API int luaopen_socket(lua_State* L) {
    /* 创建元表 */
    luaL_newmetatable(L, "socket");
    
    /* 设置元表的__index为方法表 */
    luaL_newlibtable(L, socket_methods);
    luaL_setfuncs(L, socket_methods, 0);
    lua_setfield(L, -2, "__index");
    
    /* 设置元表的其他方法 */
    luaL_setfuncs(L, socket_metatable, 0);
    
    /* 创建模块表 */
    luaL_newlibtable(L, socket_lib);
    luaL_setfuncs(L, socket_lib, 0);

    /* 推送协议族常量 */
    lua_pushinteger(L, AF_INET);
    lua_setfield(L, -2, "AF_INET");
    lua_pushinteger(L, AF_INET6);
    lua_setfield(L, -2, "AF_INET6");

    /* 推送类型常量 */
    lua_pushinteger(L, SOCK_STREAM);
    lua_setfield(L, -2, "SOCK_STREAM");
    lua_pushinteger(L, SOCK_DGRAM);
    lua_setfield(L, -2, "SOCK_DGRAM");

    /* 推送协议常量 */
    lua_pushinteger(L, IPPROTO_TCP);
    lua_setfield(L, -2, "IPPROTO_TCP");
    lua_pushinteger(L, IPPROTO_UDP);
    lua_setfield(L, -2, "IPPROTO_UDP");

    /* 推送socket级别常量 */
    lua_pushinteger(L, SOL_SOCKET);
    lua_setfield(L, -2, "SOL_SOCKET");

    /* 推送选项常量 */
    lua_pushinteger(L, SO_KEEPALIVE);
    lua_setfield(L, -2, "SO_KEEPALIVE");
    lua_pushinteger(L, SO_REUSEADDR);
    lua_setfield(L, -2, "SO_REUSEADDR");

    /* 推送SSL验证方式常量 */
    lua_pushinteger(L, SSL_VERIFY_NONE);
    lua_setfield(L, -2, "SSL_VERIFY_NONE");
    lua_pushinteger(L, SSL_VERIFY_SERVER);
    lua_setfield(L, -2, "SSL_VERIFY_SERVER");
    lua_pushinteger(L, SSL_VERIFY_BOTH);
    lua_setfield(L, -2, "SSL_VERIFY_BOTH");

    return 1;
}