/**
 * @file iot_socket.h
 * @brief ML307N 平台 Socket 适配器头文件
 * @details 基于 lwip 库实现统一的 Socket 接口封装，
 *          支持 TCP/UDP Socket 的同步操作，提供标准的 BSD-like 接口。
 */

#ifndef IOT_SOCKET_ML307N_H
#define IOT_SOCKET_ML307N_H

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/inet.h"

/* ============================================================
 * 统一 Socket 类型定义（跨平台兼容）
 * ============================================================ */

/** @brief Socket描述符类型 */
typedef int iot_socket_t;

/** @brief 无效Socket */
#define IOT_INVALID_SOCKET  (-1)

/** @brief 地址族枚举 */
typedef enum {
    IOT_AF_INET   = AF_INET,      /**< IPv4 */
    IOT_AF_INET6  = AF_INET6,     /**< IPv6 */
    IOT_AF_UNSPEC = AF_UNSPEC,    /**< 未指定 */
} iot_socket_af_t;

/** @brief Socket类型枚举 */
typedef enum {
    IOT_SOCK_STREAM    = SOCK_STREAM,     /**< TCP流式Socket */
    IOT_SOCK_DGRAM     = SOCK_DGRAM,      /**< UDP数据报Socket */
    IOT_SOCK_RAW       = SOCK_RAW,        /**< 原始Socket */
} iot_socket_type_t;

/** @brief 协议枚举 */
typedef enum {
    IOT_IPPROTO_IP    = IPPROTO_IP,     /**< IP协议 */
    IOT_IPPROTO_TCP   = IPPROTO_TCP,    /**< TCP协议 */
    IOT_IPPROTO_UDP   = IPPROTO_UDP,    /**< UDP协议 */
    IOT_IPPROTO_ICMP  = IPPROTO_ICMP,   /**< ICMP协议 */
} iot_socket_proto_t;

/** @brief Socket地址结构体 */
typedef struct {
    uint16_t family;                 /**< 地址族 */
    uint16_t port;                    /**< 端口号(网络字节序) */
    uint32_t addr;                   /**< IP地址(网络字节序) */
} iot_socket_addr_t;

/** @brief Socket配置结构体 */
typedef struct {
    iot_socket_af_t family;           /**< 地址族 */
    iot_socket_type_t type;           /**< Socket类型 */
    iot_socket_proto_t proto;         /**< 协议 */
    bool nonblock;                    /**< 是否非阻塞 */
    uint32_t recv_timeout;            /**< 接收超时(ms) */
    uint32_t send_timeout;            /**< 发送超时(ms) */
} iot_socket_config_t;

/* ============================================================
 * Socket 选项级别
 * ============================================================ */

/** @brief Socket层选项 */
#define IOT_SOL_SOCKET     SOL_SOCKET
/** @brief IP层选项 */
#define IOT_SOL_IP         IPPROTO_IP
/** @brief TCP层选项 */
#define IOT_SOL_TCP        IPPROTO_TCP
/** @brief UDP层选项 */
#define IOT_SOL_UDP         IPPROTO_UDP

/* ============================================================
 * Socket 选项名称
 * ============================================================ */

/** @brief 接收超时 */
#define IOT_SO_RCVTIMEO   SO_RCVTIMEO
/** @brief 发送超时 */
#define IOT_SO_SNDTIMEO   SO_SNDTIMEO
/** @brief 错误状态 */
#define IOT_SO_ERROR      SO_ERROR
/** @brief 地址复用 */
#define IOT_SO_REUSEADDR  SO_REUSEADDR
/** @brief 保持连接 */
#define IOT_SO_KEEPALIVE  SO_KEEPALIVE
/** @brief 广播 */
#define IOT_SO_BROADCAST  SO_BROADCAST
/** @brief 接收缓冲区大小 */
#define IOT_SO_RCVBUF     SO_RCVBUF
/** @brief 发送缓冲区大小 */
#define IOT_SO_SNDBUF     SO_SNDBUF

/* ============================================================
 * Socket 关闭方式
 * ============================================================ */

/** @brief 关闭读 */
#define IOT_SHUT_RD       SHUT_RD
/** @brief 关闭写 */
#define IOT_SHUT_WR       SHUT_WR
/** @brief 关闭读写 */
#define IOT_SHUT_RDWR     SHUT_RDWR

/* ============================================================
 * Socket 发送/接收标志
 * ============================================================ */

/** @brief 发送时不路由 */
#define IOT_MSG_PEEK      MSG_PEEK
/** @brief 发送外带数据 */
#define IOT_MSG_OOB      MSG_OOB
/** @brief 发送时不等待 */
#define IOT_MSG_DONTWAIT MSG_DONTWAIT

/* ============================================================
 * Socket 创建与关闭
 * ============================================================ */

/**
 * @brief 创建 Socket
 * @param[in] config Socket 配置参数
 * @return Socket 描述符，失败返回 IOT_INVALID_SOCKET
 */
iot_socket_t iot_socket_create(const iot_socket_config_t *config);

/**
 * @brief 关闭 Socket
 * @param[in] sock Socket 描述符
 * @return 0 成功，<0 失败
 */
int iot_socket_close(iot_socket_t sock);

/* ============================================================
 * Socket 连接与绑定
 * ============================================================ */

/**
 * @brief 连接远程服务器
 * @param[in] sock Socket 描述符
 * @param[in] addr 服务器地址
 * @return 0 成功，<0 失败
 * @note 非阻塞模式下返回 IOT_OK 表示正在连接
 */
int iot_socket_connect(iot_socket_t sock, const iot_socket_addr_t *addr);

/**
 * @brief 绑定本地地址
 * @param[in] sock Socket 描述符
 * @param[in] addr 本地地址
 * @return 0 成功，<0 失败
 */
int iot_socket_bind(iot_socket_t sock, const iot_socket_addr_t *addr);

/**
 * @brief 开始监听连接
 * @param[in] sock    Socket 描述符
 * @param[in] backlog 最大监听队列长度
 * @return 0 成功，<0 失败
 */
int iot_socket_listen(iot_socket_t sock, int backlog);

/**
 * @brief 接受客户端连接
 * @param[in]  sock 监听 Socket 描述符
 * @param[out] addr 客户端地址（可选）
 * @return 新的 Socket 描述符，失败返回 IOT_INVALID_SOCKET
 */
iot_socket_t iot_socket_accept(iot_socket_t sock, iot_socket_addr_t *addr);

/* ============================================================
 * Socket 数据发送与接收
 * ============================================================ */

/**
 * @brief 发送数据
 * @param[in] sock   Socket 描述符
 * @param[in] data   发送数据缓冲区
 * @param[in] len    数据长度
 * @param[in] flags  发送标志
 * @return >=0 实际发送长度，<0 失败
 */
int iot_socket_send(iot_socket_t sock, const void *data, size_t len, int flags);

/**
 * @brief 接收数据
 * @param[in] sock   Socket 描述符
 * @param[out] buf   接收缓冲区
 * @param[in] len    缓冲区大小
 * @param[in] flags  接收标志
 * @return >=0 实际接收长度，<0 失败
 */
int iot_socket_recv(iot_socket_t sock, void *buf, size_t len, int flags);

/**
 * @brief 发送数据到指定地址（UDP）
 * @param[in] sock   Socket 描述符
 * @param[in] data   发送数据缓冲区
 * @param[in] len    数据长度
 * @param[in] addr   目标地址（可选）
 * @param[in] flags  发送标志
 * @return >=0 实际发送长度，<0 失败
 */
int iot_socket_sendto(iot_socket_t sock, const void *data, size_t len,
                       const iot_socket_addr_t *addr, int flags);

/**
 * @brief 从指定地址接收数据（UDP）
 * @param[in]  sock  Socket 描述符
 * @param[out] buf   接收缓冲区
 * @param[in]  len   缓冲区大小
 * @param[out] addr  发送方地址（可选）
 * @param[in]  flags 接收标志
 * @return >=0 实际接收长度，<0 失败
 */
int iot_socket_recvfrom(iot_socket_t sock, void *buf, size_t len,
                         iot_socket_addr_t *addr, int flags);

/* ============================================================
 * Socket 关闭连接
 * ============================================================ */

/**
 * @brief 关闭 Socket 的读写通道
 * @param[in] sock Socket 描述符
 * @param[in] how  关闭方式（IOT_SHUT_RD/IOT_SHUT_WR/IOT_SHUT_RDWR）
 * @return 0 成功，<0 失败
 */
int iot_socket_shutdown(iot_socket_t sock, int how);

/* ============================================================
 * Socket 地址操作
 * ============================================================ */

/**
 * @brief 获取本地地址
 * @param[in]  sock Socket 描述符
 * @param[out] addr 本地地址
 * @return 0 成功，<0 失败
 */
int iot_socket_get_local_addr(iot_socket_t sock, iot_socket_addr_t *addr);

/**
 * @brief 获取对端地址
 * @param[in]  sock Socket 描述符
 * @param[out] addr 对端地址
 * @return 0 成功，<0 失败
 */
int iot_socket_get_peer_addr(iot_socket_t sock, iot_socket_addr_t *addr);

/* ============================================================
 * Socket 选项操作
 * ============================================================ */

/**
 * @brief 设置 Socket 选项
 * @param[in] sock     Socket 描述符
 * @param[in] level    选项级别
 * @param[in] optname  选项名称
 * @param[in] optval   选项值
 * @param[in] optlen   选项值长度
 * @return 0 成功，<0 失败
 */
int iot_socket_setsockopt(iot_socket_t sock, int level, int optname,
                                const void *optval, socklen_t optlen);

/**
 * @brief 获取 Socket 选项
 * @param[in]  sock    Socket 描述符
 * @param[in]  level   选项级别
 * @param[in]  optname 选项名称
 * @param[out] optval  选项值
 * @param[out] optlen  选项值长度
 * @return 0 成功，<0 失败
 */
int iot_socket_getsockopt(iot_socket_t sock, int level, int optname,
                                void *optval, socklen_t *optlen);

/* ============================================================
 * Socket 状态查询
 * ============================================================ */

/**
 * @brief 查询 Socket 连接状态
 * @param[in] sock Socket 描述符
 * @return true 已连接，false 未连接
 */
bool iot_socket_is_connected(iot_socket_t sock);

/* ============================================================
 * Socket select 多路复用
 * ============================================================ */

/**
 * @brief 多路复用等待
 * @param[out] reads      就绪的读 Socket 数组
 * @param[in]  read_count 输入的读 Socket 数量
 * @param[out] writes     就绪的写 Socket 数组
 * @param[in]  write_count 输入的写 Socket 数量
 * @param[in]  timeout_ms 超时时间（毫秒）
 * @return 0 成功，<0 失败
 */
int iot_socket_select(iot_socket_t *reads, int read_count,
                            iot_socket_t *writes, int write_count,
                            uint32_t timeout_ms);

/* ============================================================
 * DNS 域名解析
 * ============================================================ */

/**
 * @brief 域名解析
 * @param[in]  hostname 主机名
 * @param[out] ipaddr  IP 地址（网络字节序）
 * @return 0 成功，<0 失败
 */
int iot_socket_get_host_by_name(const char *hostname, uint32_t *ipaddr);

/* ============================================================
 * 地址转换函数
 * ============================================================ */

/**
 * @brief 将字符串地址转换为二进制地址
 * @param[in]  af    地址族（IOT_AF_INET 或 IOT_AF_INET6）
 * @param[in]  src   字符串地址
 * @param[out] dst   二进制地址
 * @return 0 成功，<0 失败
 */
int iot_socket_inet_pton(int af, const char *src, void *dst);

/**
 * @brief 将二进制地址转换为字符串地址
 * @param[in]  af    地址族（IOT_AF_INET 或 IOT_AF_INET6）
 * @param[in]  src   二进制地址
 * @param[out] dst   字符串地址缓冲区
 * @param[in]  size  缓冲区大小
 * @return 成功返回地址字符串，失败返回 NULL
 */
const char *iot_socket_inet_ntop(int af, const void *src, char *dst, size_t size);

/* ============================================================
 * 字节序转换函数
 * ============================================================ */

/** @brief 16位主机序转网络序 */
#define iot_socket_htons(n)  lwip_htons(n)
/** @brief 16位网络序转主机序 */
#define iot_socket_ntohs(n)  lwip_ntohs(n)
/** @brief 32位主机序转网络序 */
#define iot_socket_htonl(n)  lwip_htonl(n)
/** @brief 32位网络序转主机序 */
#define iot_socket_ntohl(n)  lwip_ntohl(n)

#endif /* IOT_SOCKET_ML307N_H */