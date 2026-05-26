/**
 * @file        cm_ssl.h
 * @brief       SSL通用API接口
 * @copyright   Copyright ? 2021 China Mobile IOT. All rights reserved.
 * @author      By YXJ
 * @date        2021/8/5
 *
 * @defgroup ssl SSL
 * @ingroup SSL
 * @{
 */

#ifndef _CM_API_SSL_H_
#define _CM_API_SSL_H_

/****************************************************************************
 * Included Files
 ****************************************************************************/
 
#include <stdint.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Public Types
 ****************************************************************************/

/** SSL配置类型 */
typedef enum
{
    CM_SSL_PARAM_VERIFY,           /**< 设置验证方式, 参数(value): 类型uint8_t, 值为验证方式类型，0无身份验证，1单向验证， 2双向验证. */
    CM_SSL_PARAM_NEGOTIME,         /**< 设置握手超时时间, 参数(value): 类型uint16_t, 值为超时时间，范围：10~300，默认值300 单位：s*/
    CM_SSL_PARAM_SESSION,          /**< 设置是否开启会话恢复功能, 参数(value): 类型uint8_t, 值为0表示关闭, 值为1表示开启 默认为1*/
    CM_SSL_PARAM_SNI,              /**< 设置是否开启SNI扩展功能, 参数(value): 类型uint8_t, 值为0表示关闭, 值为1表示开启 默认为0*/
    CM_SSL_PARAM_VERSION,          /**< 设置ssl指定版本, 参数(value): 类型uint8_t, 3 TLS1.2；255 全部. 默认为255，建议使用默认值即可，当前版本不支持TLS1.2之前的协议 */
    CM_SSL_PARAM_CIPHER_SUITE,     /**< 设置ssl指定加密套件, 参数(value): 类型uint16_t, 0表示支持所有加密套件 默认为0。若要配置指定加密套件，同时只能指定一个*/
    CM_SSL_PARAM_IGNORESTAMP,      /**< 设置ssl是否忽略证书时间戳, 参数(value): 类型uint8_t, 0表示不忽略，1表示忽略 默认为1*/
    CM_SSL_PARAM_IGNOREVERIFY,     /**< 设置ssl是否忽略证书认证结果, 参数(value): 类型uint8_t, 0表示不忽略，1表示忽略 默认为0*/
    CM_SSL_PARAM_CA_CERT,          /**< 设置CA证书, 参数(value): 类型char*, 值为CA证书字符串指针. */
    CM_SSL_PARAM_CLI_CERT,         /**< 设置客户端证书, 参数(value): 类型char*, 值为客户端证书字符串指针. */
    CM_SSL_PARAM_CLI_KEY,          /**< 设置客户端密钥, 参数(value): 类型char*, 值为客户端密钥. */
    CM_SSL_PARAM_CA_CERT_DELETE,   /**< 删除CA证书, 参数(value): 无意义. */
    CM_SSL_PARAM_CLI_CERT_DELETE,  /**< 删除客户端证书, 参数(value): 无意义. */
    CM_SSL_PARAM_CLI_KEY_DELETE,   /**< 删除客户端密钥, 参数(value): 无意义. */
} cm_ssl_param_type_e;

struct cm_ssl_ctx;
typedef struct cm_ssl_ctx cm_ssl_ctx_t;

/****************************************************************************
 * Public Data
 ****************************************************************************/

/****************************************************************************
 * Public Function Prototypes
 ****************************************************************************/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/**
 * @brief 配置SSL参数
 *
 * @param [in] ssl_id       ssl通道号，范围为0-5
 * @param [in] type         SSL配置项类型
 * @param [in] value        SSL配置项参数
 *
 * @return 
 *   = 0  - 成功 \n
 *   = -1  - 参数错误
 *
 * @details 设置SSL配置参数，本接口配置证书时会将证书写入文件系统，故使用时需确保文件系统有足够空间
 */
int32_t cm_ssl_setopt(int32_t ssl_id,cm_ssl_param_type_e type,void * value);

/**
 * @brief SSL连接
 *
 * @param [out] cm_ssl_ctx_p  ssl上下文
 * @param [in] ssl_id         ssl通道号，范围为0-5
 * @param [in] socket         socket描述符
 * @param [in] timeout        读取超时时间，单位ms
 *
 * @return 
 *   = 0  - 成功 \n
 *  != 0  - 失败
 *
 * @details 由于底层代码使用该接口，所以未对0-5之外的ssl_id作限制，用户使用0-5之外的ssl_id可能有风险，应自行确保使用0-5的ssl_id
 */
int cm_ssl_conn(void **cm_ssl_ctx_p, int ssl_id, int socket, int timeout);

/**
 * @brief SSL连接(可带host参数)
 *
 * @param [out] cm_ssl_ctx_p  ssl上下文
 * @param [in] ssl_id         ssl通道号，范围为0-5
 * @param [in] socket         socket描述符
 * @param [in] timeout        读取超时时间，单位ms
 * @param [in] host           服务器地址(仅用于SNI扩展或session恢复)
 *
 * @return 
 *   = 0  - 成功 \n
 *  != 0  - 失败
 *
 * @details 本接口相较于cm_ssl_conn增加了host参数，该参数为服务器地址，仅用于SNI扩展或session恢复，若不用这两个功能设置为空即可或者直接使用cm_ssl_conn接口
 */
int cm_ssl_conn_with_host(void **cm_ssl_ctx_p, int ssl_id, int socket, int timeout, char* host);

/**
 * @brief SSL关闭
 *
 * @param [in] cm_ssl_ctx_p  ssl上下文
 *
 * @details 
 */
void cm_ssl_close(void **cm_ssl_ctx_p);

/**
 * @brief SSL写
 *
 * @param [in] cm_ssl_ctx     ssl上下文
 * @param [in] data           待写数据
 * @param [in] data_len       数据长度
 *
 * @return 
 *   >= 0 - 发送长度 \n
 *   < 0  - 错误码
 *
 * @details 
 */
int cm_ssl_write(cm_ssl_ctx_t *cm_ssl_ctx, void *data, int data_len);

/**
 * @brief SSL读
 *
 * @param [in] cm_ssl_ctx     ssl上下文
 * @param [in] data           读数据缓存区
 * @param [in] data_len       缓存区长度
 *
 * @return 
 *   >= 0 - 读取长度 \n
 *   < 0  - 错误码
 *
 * @details 
 */
int cm_ssl_read(cm_ssl_ctx_t *cm_ssl_ctx, void *data, int data_len);

/**
 * @brief 缓存区是否有未读数据
 *
 * @param [in] cm_ssl_ctx     ssl上下文
 *
 * @return 
 *   = 0 - 缓存中无未读数据\n
 *   = 1 - 缓存中有数据未读
 *
 * @details 
 */
int cm_ssl_check_pending(cm_ssl_ctx_t *cm_ssl_ctx);

/**
 * @brief 缓存区可读数据长度
 *
 * @param [in] cm_ssl_ctx     ssl上下文
 *
 * @return 
 *   缓存区可读数据长度
 *
 * @details 使用本函数获取可读数据长度之前，需要先用cm_ssl_read()进行一次空读取，具体参考使用示例;
 */
int cm_ssl_get_bytes_avail(cm_ssl_ctx_t *cm_ssl_ctx);

/**
 * @brief 列举支持的加密套件
 *
 * @return 加密套件列表
 *
 * @details 返回加密套件列表数组，数据为0时为数组结尾
 */
int *cm_ssl_list_cipher(void);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_SSL_H__ */

/** @}*/
