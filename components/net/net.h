/**
 * @file net.h
 * @brief 基于 lwip 的 socket 异步接口封装
 *
 * 提供类似 BSD socket 的异步接口，底层基于 lwip TCP/IP 协议栈
 */
#ifndef IOT_NET_H
#define IOT_NET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

/* socket 句柄类型 */
typedef int sock_t;
#define INVALID_SOCKET  (-1)
#define SOCKET_ERROR    (-1)

/* socket 类型 */
typedef enum {
    SOCK_TYPE_STREAM = 0,    /* TCP 流式 socket */
    SOCK_TYPE_DGRAM  = 1,     /* UDP 数据报 socket */
} net_sock_type_t;

/* socket 协议 */
typedef enum {
    SOCK_PROTO_TCP = 0,      /* TCP 协议 */
    SOCK_PROTO_UDP = 1,      /* UDP 协议 */
} net_sock_proto_t;

/* socket 事件类型 */
typedef enum {
    NET_EVENT_CONNECTED   = 1 << 0,  /* 连接建立 */
    NET_EVENT_DISCONNECTED = 1 << 1,  /* 连接断开 */
    NET_EVENT_ACCEPT      = 1 << 2,  /* 监听 socket 收到连接 */
    NET_EVENT_RECV        = 1 << 3,  /* 可读事件 */
    NET_EVENT_SEND        = 1 << 4,  /* 可写事件 */
    NET_EVENT_ERROR       = 1 << 5,  /* 错误事件 */
} net_event_type_t;

/* socket 状态 */
typedef enum {
    NET_SOCK_STATE_CLOSED      = 0,   /* 关闭状态 */
    NET_SOCK_STATE_OPENED      = 1,   /* 已打开 */
    NET_SOCK_STATE_LISTENING   = 2,   /* 监听中 */
    NET_SOCK_STATE_CONNECTING  = 3,   /* 连接中 */
    NET_SOCK_STATE_CONNECTED   = 4,   /* 已连接 */
    NET_SOCK_STATE_ERROR       = 5,    /* 错误状态 */
} net_sock_state_t;

/* socket 描述符 */
typedef struct net_socket net_socket_t;

/* socket 回调函数类型 */
typedef void (*net_socket_callback_t)(net_socket_t* sock, net_event_type_t event, void* user_data);

/*===========================================================
 * socket 操作接口
 *===========================================================*/

/**
 * @brief 创建 socket
 * @param type socket 类型 SOCK_TYPE_STREAM 或 SOCK_TYPE_DGRAM
 * @param callback 事件回调函数
 * @param user_data 用户数据
 * @return socket 句柄，失败返回 INVALID_SOCKET
 */
sock_t net_socket_create(net_sock_type_t type, net_socket_callback_t callback, void* user_data);

/**
 * @brief 绑定地址
 * @param sock socket 句柄
 * @param ip IP 地址字符串
 * @param port 端口号
 * @return 0 成功，-1 失败
 */
int net_socket_bind(sock_t sock, const char* ip, uint16_t port);

/**
 * @brief 监听连接
 * @param sock socket 句柄
 * @param backlog 最大监听队列长度
 * @return 0 成功，-1 失败
 */
int net_socket_listen(sock_t sock, int backlog);

/**
 * @brief 连接到服务器（异步）
 * @param sock socket 句柄
 * @param ip 服务器 IP 地址
 * @param port 服务器端口
 * @return 0 成功（异步连接），-1 失败
 */
int net_socket_connect(sock_t sock, const char* ip, uint16_t port);

/**
 * @brief 接受连接
 * @param listen_sock 监听 socket
 * @param callback 客户端 socket 的回调（可选）
 * @param user_data 用户数据
 * @return 客户端 socket 句柄，失败返回 INVALID_SOCKET
 */
sock_t net_socket_accept(sock_t listen_sock, net_socket_callback_t callback, void* user_data);

/**
 * @brief 发送数据（异步）
 * @param sock socket 句柄
 * @param data 数据缓冲区
 * @param len 数据长度
 * @return 发送的字节数（实际可能小于 len），-1 失败
 */
int net_socket_send(sock_t sock, const void* data, size_t len);

/**
 * @brief 接收数据
 * @param sock socket 句柄
 * @param buf 接收缓冲区
 * @param len 缓冲区长度
 * @return 接收的字节数，0 表示连接关闭，-1 失败
 */
int net_socket_recv(sock_t sock, void* buf, size_t len);

/**
 * @brief 关闭 socket
 * @param sock socket 句柄
 */
void net_socket_close(sock_t sock);

/**
 * @brief 获取 socket 状态
 * @param sock socket 句柄
 * @return socket 状态
 */
net_sock_state_t net_socket_get_state(sock_t sock);

/**
 * @brief 设置 socket 选项
 * @param sock socket 句柄
 * @param optname 选项名称
 * @param optval 选项值
 * @param optlen 选项长度
 * @return 0 成功，-1 失败
 */
int net_socket_setopt(sock_t sock, int optname, const void* optval, size_t optlen);

/**
 * @brief 获取本地地址
 * @param sock socket 句柄
 * @param ip 输出 IP 地址缓冲区
 * @param ip_len 缓冲区长度
 * @param port 输出端口
 * @return 0 成功，-1 失败
 */
int net_socket_get_local_addr(sock_t sock, char* ip, size_t ip_len, uint16_t* port);

/**
 * @brief 获取远程地址
 * @param sock socket 句柄
 * @param ip 输出 IP 地址缓冲区
 * @param ip_len 缓冲区长度
 * @param port 输出端口
 * @return 0 成功，-1 失败
 */
int net_socket_get_remote_addr(sock_t sock, char* ip, size_t ip_len, uint16_t* port);

/*===========================================================
 * DNS 解析接口
 *===========================================================*/

/**
 * @brief DNS 解析回调类型
 */
typedef void (*net_dns_callback_t)(const char* name, const char* ip, void* user_data);

/**
 * @brief 异步 DNS 解析
 * @param name 域名
 * @param callback 解析完成回调
 * @param user_data 用户数据
 * @return 0 成功，-1 失败
 */
int net_dns_resolve(const char* name, net_dns_callback_t callback, void* user_data);

/*===========================================================
 * 网络初始化接口
 *===========================================================*/

/**
 * @brief 初始化网络子系统
 * @return 0 成功，-1 失败
 */
int net_init(void);

/**
 * @brief 反初始化网络子系统
 */
void net_deinit(void);

/**
 * @brief 网络轮询（需要在主循环中调用）
 * @param timeout_ms 超时时间（毫秒）
 */
void net_poll(int timeout_ms);

/*===========================================================
 * 辅助接口
 *===========================================================*/

/**
 * @brief 将 IP 地址字符串转换为数字形式
 * @param ip IP 地址字符串
 * @return IP 地址（网络字节序），0 表示失败
 */
uint32_t net_inet_addr(const char* ip);

/**
 * @brief 将数字形式的 IP 地址转换为字符串
 * @param addr IP 地址（网络字节序）
 * @param buf 输出缓冲区
 * @param len 缓冲区长度
 * @return 字符串指针
 */
const char* net_inet_ntoa(uint32_t addr, char* buf, size_t len);

/**
 * @brief 将主机名转换为 IP 地址（同步，只支持 IPv4）
 * @param hostname 主机名
 * @param ip 输出 IP 地址
 * @return 0 成功，-1 失败
 */
int net_gethostbyname(const char* hostname, char* ip);

#ifdef __cplusplus
}
#endif

#endif /* IOT_NET_H */
