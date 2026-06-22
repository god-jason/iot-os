/**
 * @file net.h
 * @brief 网络模块接口定义
 *
 * 封装 lwip 非阻塞 socket，提供异步网络接口
 * 支持 SSL/TLS（基于 GmSSL），支持 TLS 1.2/1.3 和 TLCP 协议
 */

#ifndef IOT_NET_H
#define IOT_NET_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* sock_t;
#define INVALID_SOCKET  NULL

/**
 * @brief socket 类型
 */
typedef enum {
    SOCK_TYPE_STREAM = 0,  /* 流式 socket（TCP） */
    SOCK_TYPE_DGRAM  = 1,  /* 数据报 socket（UDP） */
} net_sock_type_t;

/**
 * @brief 网络事件类型
 */
typedef enum {
    NET_EVENT_CONNECTED    = 0,  /* 连接成功 */
    NET_EVENT_DISCONNECTED = 1,  /* 连接断开 */
    NET_EVENT_ACCEPT       = 2,  /* 有新连接 */
    NET_EVENT_RECV         = 3,  /* 收到数据 */
    NET_EVENT_SEND         = 4,  /* 数据发送完成 */
    NET_EVENT_ERROR        = 5,  /* 发生错误 */
} net_event_type_t;

/**
 * @brief socket 状态
 */
typedef enum {
    NET_SOCK_STATE_CLOSED      = 0,  /* 已关闭 */
    NET_SOCK_STATE_OPENED      = 1,  /* 已创建 */
    NET_SOCK_STATE_LISTENING   = 2,  /* 监听中 */
    NET_SOCK_STATE_CONNECTING  = 3,  /* 连接中 */
    NET_SOCK_STATE_CONNECTED   = 4,  /* 已连接 */
    NET_SOCK_STATE_SSL_HANDSHAKE = 5, /* SSL 握手中 */
    NET_SOCK_STATE_ERROR       = 6,  /* 错误状态 */
} net_sock_state_t;

typedef struct net_socket net_socket_t;

/**
 * @brief 网络事件回调函数
 * @param sock socket 句柄
 * @param event 事件类型
 * @param user_data 用户数据
 */
typedef void (*net_socket_callback_t)(net_socket_t* sock, net_event_type_t event, void* user_data);

/**
 * @brief SSL 验证模式
 */
typedef enum {
    NET_SSL_VERIFY_NONE      = 0,  /* 不验证服务器证书 */
    NET_SSL_VERIFY_OPTIONAL  = 1,  /* 可选验证 */
    NET_SSL_VERIFY_REQUIRED  = 2,  /* 必须验证 */
} net_ssl_verify_mode_t;

/**
 * @brief SSL 协议版本
 */
typedef enum {
    NET_SSL_PROTOCOL_TLS12   = 0,  /* TLS 1.2 */
    NET_SSL_PROTOCOL_TLS13   = 1,  /* TLS 1.3 */
    NET_SSL_PROTOCOL_TLCP    = 2,  /* TLCP（国密协议） */
    NET_SSL_PROTOCOL_AUTO    = 3,  /* 自动协商 */
} net_ssl_protocol_t;

/**
 * @brief SSL 配置结构
 */
typedef struct {
    net_ssl_protocol_t protocol;         /* 协议版本 */
    net_ssl_verify_mode_t verify_mode;   /* 证书验证模式 */
    const char* ca_file;                 /* CA 证书文件路径 */
    const char* cert_file;               /* 客户端证书文件路径 */
    const char* key_file;                /* 客户端私钥文件路径 */
    const char* key_password;            /* 私钥密码 */
    const char* hostname;                /* 服务器主机名（用于 SNI） */
    int handshake_timeout_ms;            /* 握手超时时间（毫秒） */
} net_ssl_config_t;

/**
 * @brief 创建网络 socket
 * @param type socket 类型
 * @param ssl_config SSL 配置（NULL 表示不启用 SSL）
 * @param callback 事件回调函数
 * @param user_data 用户数据
 * @return socket 句柄，失败返回 INVALID_SOCKET
 */
sock_t net_socket_create(net_sock_type_t type, const net_ssl_config_t* ssl_config,
                        net_socket_callback_t callback, void* user_data);

/**
 * @brief 绑定 socket 到指定地址和端口
 * @param sock socket 句柄
 * @param ip 绑定的 IP 地址
 * @param port 绑定的端口号
 * @return 0 成功，-1 失败
 */
int net_socket_bind(sock_t sock, const char* ip, uint16_t port);

/**
 * @brief 开始监听 socket
 * @param sock socket 句柄
 * @param backlog 最大等待连接数
 * @return 0 成功，-1 失败
 */
int net_socket_listen(sock_t sock, int backlog);

/**
 * @brief 连接到远程服务器
 * @param sock socket 句柄
 * @param ip 服务器 IP 地址
 * @param port 服务器端口号
 * @return 0 成功（可能是异步连接中），-1 失败
 */
int net_socket_connect(sock_t sock, const char* ip, uint16_t port);

/**
 * @brief 接受新的连接
 * @param listen_sock 监听 socket 句柄
 * @param callback 新连接的事件回调函数
 * @param user_data 用户数据
 * @return 新连接的 socket 句柄，失败返回 INVALID_SOCKET
 */
sock_t net_socket_accept(sock_t listen_sock, net_socket_callback_t callback, void* user_data);

/**
 * @brief 发送数据
 * @param sock socket 句柄
 * @param data 要发送的数据
 * @param len 数据长度
 * @return 发送的字节数，-1 失败
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
 * @brief 检查是否为 SSL socket
 * @param sock socket 句柄
 * @return true 是 SSL socket，false 不是
 */
bool net_socket_is_ssl(sock_t sock);

/**
 * @brief 检查 SSL 握手是否完成
 * @param sock socket 句柄
 * @return true 握手完成，false 未完成
 */
bool net_socket_ssl_handshake_done(sock_t sock);

/**
 * @brief 获取 SSL 错误信息
 * @param sock socket 句柄
 * @return 错误信息字符串
 */
const char* net_socket_ssl_get_error(sock_t sock);

/**
 * @brief 获取用户数据
 * @param sock socket 句柄
 * @return 用户数据指针
 */
void* net_socket_get_user_data(sock_t sock);

/**
 * @brief 设置用户数据
 * @param sock socket 句柄
 * @param user_data 用户数据指针
 */
void net_socket_set_user_data(sock_t sock, void* user_data);

/**
 * @brief 获取接收缓冲区指针
 * @param sock socket 句柄
 * @return 接收缓冲区指针
 */
const char* net_socket_get_recv_buf(sock_t sock);

/**
 * @brief 获取接收缓冲区中数据长度
 * @param sock socket 句柄
 * @return 数据长度
 */
size_t net_socket_get_recv_len(sock_t sock);

/**
 * @brief 清空接收缓冲区
 * @param sock socket 句柄
 */
void net_socket_clear_recv_buf(sock_t sock);

/**
 * @brief DNS 解析回调函数
 * @param name 主机名
 * @param ip 解析后的 IP 地址（NULL 表示解析失败）
 * @param user_data 用户数据
 */
typedef void (*net_dns_callback_t)(const char* name, const char* ip, void* user_data);

/**
 * @brief 异步 DNS 解析
 * @param name 主机名
 * @param callback 解析完成回调
 * @param user_data 用户数据
 * @return 0 成功（异步进行中或已完成），-1 失败
 */
int net_dns_resolve(const char* name, net_dns_callback_t callback, void* user_data);

/**
 * @brief 初始化网络模块
 * @return 0 成功，-1 失败
 */
int net_init(void);

/**
 * @brief 反初始化网络模块
 */
void net_deinit(void);

/**
 * @brief 将点分十进制 IP 转换为网络字节序整数
 * @param ip 点分十进制 IP 字符串
 * @return 网络字节序整数
 */
uint32_t net_inet_addr(const char* ip);

/**
 * @brief 将网络字节序整数转换为点分十进制 IP
 * @param addr 网络字节序整数
 * @param buf 输出缓冲区
 * @param len 缓冲区长度
 * @return IP 字符串指针
 */
const char* net_inet_ntoa(uint32_t addr, char* buf, size_t len);

/**
 * @brief 初始化 SSL 配置
 * @param config SSL 配置结构
 */
void net_ssl_config_init(net_ssl_config_t* config);

#ifdef __cplusplus
}
#endif

#endif /* IOT_NET_H */