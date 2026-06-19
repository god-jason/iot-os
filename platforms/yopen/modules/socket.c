/*
@module  socket
@summary 网络Socket
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     NET
*/

/*
Socket参考示例
-- 创建TCP socket
local sock = socket.tcp()

-- 连接服务器
local ok, err = sock:connect("192.168.1.100", 8080)
if not ok then
    print("connect failed:", err)
    return
end

-- 发送数据
sock:send("hello")

-- 接收数据
local data = sock:recv(1024)
print("recv:", data)

-- 关闭连接
sock:close()

-- 创建UDP socket
local udp = socket.udp()
udp:bind("0.0.0.0", 8888)
*/

#include "module.h"
#include "yopen_sockets.h"
#include <string.h>

/* Socket类型 */
#define SOCKET_TCP  0
#define SOCKET_UDP  1

/* Socket句柄结构 */
typedef struct {
    int sock;              /* socket描述符 */
    int domain;            /* 协议族 AF_INET */
    int type;              /* 类型 SOCK_STREAM/DGRAM */
    int protocol;          /* 协议 IPPROTO_TCP/UDP */
} socket_handle_t;

/**
 * @brief 创建TCP socket
 * @api socket.tcp()
 * @return userdata socket句柄
 */
static int luaopen_socket_tcp(lua_State* L) {
    socket_handle_t* handle = (socket_handle_t*)iot_malloc(sizeof(socket_handle_t));
    if (!handle) {
        lua_pushnil(L);
        return 1;
    }

    handle->domain = AF_INET;
    handle->type = SOCK_STREAM;
    handle->protocol = IPPROTO_TCP;
    handle->sock = -1;

    lua_pushlightuserdata(L, handle);
    return 1;
}

/**
 * @brief 创建UDP socket
 * @api socket.udp()
 * @return userdata socket句柄
 */
static int luaopen_socket_udp(lua_State* L) {
    socket_handle_t* handle = (socket_handle_t*)iot_malloc(sizeof(socket_handle_t));
    if (!handle) {
        lua_pushnil(L);
        return 1;
    }

    handle->domain = AF_INET;
    handle->type = SOCK_DGRAM;
    handle->protocol = IPPROTO_UDP;
    handle->sock = -1;

    lua_pushlightuserdata(L, handle);
    return 1;
}

/**
 * @brief 连接远程服务器
 * @api sock:connect(ip, port)
 * @string ip 服务器IP
 * @int port 端口
 * @return bool true表示成功
 */
static int luaopen_socket_connect(lua_State* L) {
    socket_handle_t* handle = (socket_handle_t*)lua_touserdata(L, 1);
    const char* ip = luaL_checkstring(L, 2);
    int port = (int)luaL_checkinteger(L, 3);

    if (!handle || !ip) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid param");
        return 2;
    }

    /* 创建socket */
    int sock = socket(handle->domain, handle->type, handle->protocol);
    if (sock < 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "socket create failed");
        return 2;
    }

    /* 设置目标地址 */
    struct sockaddr_in dest_addr = {0};
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(ip);

    /* 连接 */
    int ret = connect(sock, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
    if (ret < 0) {
        close(sock);
        lua_pushboolean(L, 0);
        lua_pushstring(L, "connect failed");
        return 2;
    }

    handle->sock = sock;
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 发送数据
 * @api sock:send(data)
 * @string data 要发送的数据
 * @return int 发送的字节数，失败返回nil
 */
static int luaopen_socket_send(lua_State* L) {
    socket_handle_t* handle = (socket_handle_t*)lua_touserdata(L, 1);

    if (!handle || handle->sock < 0) {
        lua_pushnil(L);
        return 1;
    }

    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    int sent = send(handle->sock, data, len, 0);

    if (sent >= 0) {
        lua_pushinteger(L, sent);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 接收数据
 * @api sock:recv(maxlen)
 * @int maxlen 最大接收长度
 * @return string 接收到的数据
 */
static int luaopen_socket_recv(lua_State* L) {
    socket_handle_t* handle = (socket_handle_t*)lua_touserdata(L, 1);
    int maxlen = (int)luaL_checkinteger(L, 2);

    if (!handle || handle->sock < 0) {
        lua_pushstring(L, "");
        return 1;
    }

    if (maxlen <= 0 || maxlen > 4096) {
        maxlen = 1024;
    }

    uint8_t* buf = (uint8_t*)iot_malloc(maxlen);
    if (!buf) {
        lua_pushstring(L, "");
        return 1;
    }

    int len = recv(handle->sock, (char*)buf, maxlen, 0);

    if (len > 0) {
        lua_pushlstring(L, (char*)buf, len);
    } else {
        lua_pushstring(L, "");
    }

    iot_free(buf);
    return 1;
}

/**
 * @brief 绑定端口
 * @api sock:bind(ip, port)
 * @string ip 本地IP (通常为"0.0.0.0")
 * @int port 端口
 * @return bool true表示成功
 */
static int luaopen_socket_bind(lua_State* L) {
    socket_handle_t* handle = (socket_handle_t*)lua_touserdata(L, 1);
    const char* ip = luaL_checkstring(L, 2);
    int port = (int)luaL_checkinteger(L, 3);

    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 创建socket */
    int sock = socket(handle->domain, handle->type, handle->protocol);
    if (sock < 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 设置本地地址 */
    struct sockaddr_in local_addr = {0};
    local_addr.sin_family = AF_INET;
    local_addr.sin_port = htons(port);
    local_addr.sin_addr.s_addr = inet_addr(ip);

    /* 绑定 */
    int ret = bind(sock, (struct sockaddr*)&local_addr, sizeof(local_addr));
    if (ret < 0) {
        close(sock);
        lua_pushboolean(L, 0);
        return 1;
    }

    /* UDP需要listen */
    if (handle->type == SOCK_DGRAM) {
        /* UDP不需要listen */
    } else {
        listen(sock, 5);
    }

    handle->sock = sock;
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 关闭socket
 * @api sock:close()
 * @return nil
 */
static int luaopen_socket_close(lua_State* L) {
    socket_handle_t* handle = (socket_handle_t*)lua_touserdata(L, 1);

    if (handle) {
        if (handle->sock >= 0) {
            close(handle->sock);
            handle->sock = -1;
        }
        iot_free(handle);
    }
    return 0;
}

static const luaL_Reg luaopen_socket_funcs[] = {
    {"tcp", luaopen_socket_tcp},
    {"udp", luaopen_socket_udp},
    {NULL, NULL}
};

static const luaL_Reg luaopen_socket_methods[] = {
    {"connect", luaopen_socket_connect},
    {"send",    luaopen_socket_send},
    {"recv",    luaopen_socket_recv},
    {"bind",    luaopen_socket_bind},
    {"close",   luaopen_socket_close},
    {NULL, NULL}
};

int luaopen_socket(lua_State* L) {
    luaL_newlib(L, luaopen_socket_funcs);

    /* 创建元表 */
    luaL_newmetatable(L, "socket_handle");
    luaL_setfuncs(L, luaopen_socket_methods, 0);
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    return 1;
}