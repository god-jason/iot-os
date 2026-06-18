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
#include <stdbool.h>
/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/
#define MSSL_CERTS_FILENAME_SIZE_MAX 64
/****************************************************************************
 * Public Types
 ****************************************************************************/

/** SSL证书文件类型 */
typedef enum
{
    CM_SSL_FILE_TYPE_CERT = 1,
    CM_SSL_FILE_TYPE_KEY,
    CM_SSL_FILE_TYPE_PSKID,
    
    CM_SSL_FILE_TYPE_MAX,
}cm_ssl_file_type;

/** SSL证书文件结构体**/
typedef struct _cm_ssl_certfile_t
{
    uint8_t file_name[MSSL_CERTS_FILENAME_SIZE_MAX + 1]; //证书名
    cm_ssl_file_type file_type; //证书类型
    uint16_t cert_length; // 证书长度
    uint16_t write_len; //已写入长度
#ifndef CM_SSL_FILESYSTEM_ENABLE //不支持文件系统时使用，用于RAM中保存证书
    uint8_t *cert; //证书数据
#else
    cm_fs_t fs; //w文件句柄
#endif
    void *next; //下一个证书
} cm_ssl_certfile_t;

/** SSL配置类型 */
typedef enum
{
    CM_SSL_PARAM_VERIFY,            /**< 设置认证等级, 参数(value): 类型uint8_t, 0无身份验证，1单向验证， 2双向验证. */
    CM_SSL_PARAM_VERSION,           /**< 设置支持的协议版本, 参数(value): 类型uint8_t,  3 TLS1.2；255 全部*/
    CM_SSL_PARAM_SESSION,           /**< 设置是否开启会话恢复功能, 参数(value): 类型uint8_t, 0 关闭；1 打开. */
    CM_SSL_PARAM_IGNORE_STAMP,      /**< 设置是否忽略证书时间, 参数(value): 类型uint8_t, 0 关闭；1 打开.. */
    CM_SSL_PARAM_IGNORE_VERIFY,     /**< 设置是否忽略证书认证结果, 参数(value): 类型uint8_t, 0 关闭；1 打开. */
    CM_SSL_PARAM_NEGOTIME,          /**< 设置协商阶段的最大超时时间, 参数(value): 类型uint16_t，10~300s. */
    CM_SSL_PARAM_SUITES,            /**< 设置加密套件, 参数(value): 类型int32_t，0支持所有套件. */
    CM_SSL_PARAM_CA_CERT_FILENAME,  /**< 设置CA证书文件名, 参数(value): 类型uint8_t *. */
    CM_SSL_PARAM_CLI_CERT_FILENAME, /**< 设置客户端证书文件名, 参数(value): 类型uint8_t *. */
    CM_SSL_PARAM_CLI_KEY_FILENAME,  /**< 设置私钥文件名, 参数(value): 类型uint8_t *. */
    CM_SSL_PARAM_PSKID,             /**< 设置PSK预共享密钥文件名, 参数(value): 类型uint8_t *. */
    CM_SSL_PARAM_SNI,               /**< 设置是否使能SNI功能, 参数(value): 类型uint8_t, 0 关闭；1 打开.. */
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
 * @brief 获取ssl相关参数
 *
 *  @param [in] ssl_id         客户端句柄
 *  @param [in] type            配置项
 *  @param [in] value          设定值(传入参数值)
 *
 * @return -1 失败；0 成功
 *
 * @details 获取ssl相关参数，支持项见cm_ssl_param_type_e
 */
int32_t cm_ssl_getopt(int32_t ssl_id,cm_ssl_param_type_e type,void * value);

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
int cm_ssl_conn_ex(void **cm_ssl_ctx_p, int ssl_id, int socket, int timeout, char *host);

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
 * @brief 证书是否已创建
 *
 *  @param [in] file_name       证书名
 *
 * @return 0:未创建； 1:已创建
 *
 * @details
 */
bool cm_ssl_check_cert_file_exists(uint8_t *file_name);

/**
 * @brief 写入证书
 *
 *  @param [in] file_name       证书名
 *  @param [in] length          写入长度
 *  @param [in] remain_length   剩余长度
 *  @param [in] data            数据
 *  @param [in] file_type       证书类型
 *
 * @return 0:成功；其他:失败
 *
 * @details
 */
int cm_ssl_write_cert_file(uint8_t *file_name, uint16_t length, uint16_t remain_length, uint8_t *data, cm_ssl_file_type file_type);

/**
 * @brief 获取证书类型
 *
 *  @param [in] file_name       证书名
 *
 * @return -1:失败；其他:文件类型
 *
 * @details 获取证书类型。
 */
int cm_ssl_get_cert_file_type(uint8_t *file_name);

/**
 * @brief 读取证书
 *
 *  @param [in] file_name       证书名
 *  @param [in] data            数据
 *  @param [in] length          读取长度
 *
 * @return 0:成功；其他:失败
 *
 * @details 私钥不支持读取,data外部传入空指针，非文件系统下，复制为内部文件保存
 * 的地址，外部使用后不可释放，文件系统下，内部分配空间，外部使用后，需释放空间。
 */
int cm_ssl_read_cert_file(uint8_t *file_name, uint8_t **data, uint16_t *length);

/**
 * @brief 删除证书
 *
 *  @param [in] file_name       证书名
 *
 * @return 0:成功；其他:失败
 *
 * @details 
 */
int cm_ssl_rm_cert_file(uint8_t *file_name);

/**
 * @brief 列举证书
 *
 * @return 链表指针
 *
 * @details 返回证书保存链表
 */
cm_ssl_certfile_t* cm_ssl_list_cert_file(void);

/**
 * @brief 校验证书
 *
 *  @param [in] file_name       证书名
 *  @param [in] verify_alg      校验类型（当前只支持0：MD5校验）
 *  @param [out] out            校验码
 *
 * @return 0:成功；其他:失败
 *
 * @details out外部传入空指针，接口内部将分配内存，外部使用后需在外部释放。
 */
int cm_ssl_check_cert_file(uint8_t *file_name, uint8_t verify_alg, uint8_t **out);

/**
 * @brief 列举支持的加密套件
 *
 * @return 加密套件列表
 *
 * @details 返回加密套件列表数组，数据为0时为数组结尾
 */
int *cm_ssl_lis_cipher(void);

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif /* __CM_SSL_H__ */

/** @}*/
