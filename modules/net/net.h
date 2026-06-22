/**
 * @file net.h
 * @brief 基于 lwip 的 socket 异步接口封装
 *
 * 提供类似 BSD socket 的异步接口，底层基于 lwip TCP/IP 协议栈
 * 支持 SSL/TLS 安全连接（基于 GmSSL）
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
 * @param ssl_config SSL 配置（可选，为 NULL 表示普通 socket）
 * @param callback 事件回调函数
 * @param user_data 用户数据
 * @return socket 句柄，失败返回 INVALID_SOCKET
 */
sock_t net_socket_create(net_sock_type_t type, const net_ssl_config_t* ssl_config,
                        net_socket_callback_t callback, void* user_data);

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

/**
 * @brief 检查 socket 是否为 SSL/TLS
 * @param sock socket 句柄
 * @return true 是 SSL，false 否
 */
bool net_socket_is_ssl(sock_t sock);

/**
 * @brief 检查 SSL 握手是否完成
 * @param sock socket 句柄（必须为 SSL socket）
 * @return true 已完成，false 未完成
 */
bool net_socket_ssl_handshake_done(sock_t sock);

/**
 * @brief 获取 SSL 错误信息
 * @param sock socket 句柄（必须为 SSL socket）
 * @return 错误信息字符串
 */
const char* net_socket_ssl_get_error(sock_t sock);

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

/*===========================================================
 * SSL/TLS 接口
 *===========================================================*/

/* SSL 配置选项 */
typedef enum {
    NET_SSL_VERIFY_NONE      = 0,    /* 不验证证书 */
    NET_SSL_VERIFY_OPTIONAL  = 1,    /* 可选验证 */
    NET_SSL_VERIFY_REQUIRED  = 2,    /* 必须验证 */
} net_ssl_verify_mode_t;

typedef enum {
    NET_SSL_PROTOCOL_TLS1    = 0,    /* TLS 1.0 */
    NET_SSL_PROTOCOL_TLS11   = 1,    /* TLS 1.1 */
    NET_SSL_PROTOCOL_TLS12   = 2,    /* TLS 1.2 */
    NET_SSL_PROTOCOL_TLS13   = 3,    /* TLS 1.3 */
    NET_SSL_PROTOCOL_TLCP    = 4,    /* TLCP（国密） */
    NET_SSL_PROTOCOL_AUTO    = 5,    /* 自动选择 */
} net_ssl_protocol_t;

typedef enum {
    NET_SSL_CIPHER_AUTO      = 0,    /* 自动选择 */
    NET_SSL_CIPHER_SM4       = 1,    /* SM4 系列 */
    NET_SSL_CIPHER_AES       = 2,    /* AES 系列 */
} net_ssl_cipher_t;

/* SSL 配置结构 */
typedef struct {
    /* 协议和加密套件 */
    net_ssl_protocol_t protocol;     /* 协议版本 */
    net_ssl_cipher_t cipher;         /* 加密套件类型 */
    
    /* 证书验证 */
    net_ssl_verify_mode_t verify_mode; /* 验证模式 */
    const char* ca_file;             /* CA 证书文件路径 */
    const char* ca_dir;              /* CA 证书目录路径 */
    
    /* 客户端证书（双向认证） */
    const char* cert_file;           /* 客户端证书文件路径 */
    const char* key_file;            /* 客户端私钥文件路径 */
    const char* key_password;        /* 私钥密码 */
    
    /* SNI（Server Name Indication） */
    const char* hostname;            /* 服务器主机名 */
    
    /* 超时设置 */
    int handshake_timeout_ms;        /* 握手超时时间（毫秒） */
} net_ssl_config_t;

/**
 * @brief 初始化 SSL 配置
 * @param config 配置结构
 */
void net_ssl_config_init(net_ssl_config_t* config);

#ifdef __cplusplus
}
#endif

#endif /* IOT_NET_H */
