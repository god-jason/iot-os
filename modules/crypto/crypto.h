/**
 * @file crypto.h
 * @brief 加密算法库
 *
 * 基于 GmSSL 封装常用加密算法，包括：
 * - 哈希算法: MD5, SHA1, SHA224, SHA256, SHA384, SHA512, SM3
 * - HMAC: 基于各种哈希算法的消息认证码
 * - 对称加密: AES, SM4 (ECB, CBC, CTR, GCM 模式)
 * - 校验算法: CRC16, CRC32, Adler32
 * - 编码转换: Hex, Base64
 * - 证书操作: X509 证书解析和验证
 * - 密钥派生: PBKDF2
 * - 随机数生成
 */
#ifndef IOT_CRYPTO_H
#define IOT_CRYPTO_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

/* 哈希算法类型 */
typedef enum {
    CRYPTO_HASH_MD5 = 0,
    CRYPTO_HASH_SHA1,
    CRYPTO_HASH_SHA224,
    CRYPTO_HASH_SHA256,
    CRYPTO_HASH_SHA384,
    CRYPTO_HASH_SHA512,
    CRYPTO_HASH_SM3
} crypto_hash_type_t;

/* 对称加密算法类型 */
typedef enum {
    CRYPTO_CIPHER_AES_128_ECB = 0,
    CRYPTO_CIPHER_AES_128_CBC,
    CRYPTO_CIPHER_AES_128_CTR,
    CRYPTO_CIPHER_AES_128_GCM,
    CRYPTO_CIPHER_AES_192_ECB,
    CRYPTO_CIPHER_AES_192_CBC,
    CRYPTO_CIPHER_AES_192_CTR,
    CRYPTO_CIPHER_AES_192_GCM,
    CRYPTO_CIPHER_AES_256_ECB,
    CRYPTO_CIPHER_AES_256_CBC,
    CRYPTO_CIPHER_AES_256_CTR,
    CRYPTO_CIPHER_AES_256_GCM,
    CRYPTO_CIPHER_SM4_ECB,
    CRYPTO_CIPHER_SM4_CBC,
    CRYPTO_CIPHER_SM4_CTR,
    CRYPTO_CIPHER_SM4_GCM
} crypto_cipher_type_t;

/* 校验算法类型 */
typedef enum {
    CRYPTO_CHECKSUM_CRC16 = 0,
    CRYPTO_CHECKSUM_CRC32,
    CRYPTO_CHECKSUM_ADLER32
} crypto_checksum_type_t;

/* X509 证书信息 */
typedef struct crypto_x509_cert crypto_x509_cert_t;
struct crypto_x509_cert {
    char* subject;           /* 证书主题 */
    char* issuer;           /* 颁发者 */
    char* serial_number;    /* 序列号 */
    char* not_before;       /* 生效时间 */
    char* not_after;        /* 过期时间 */
    char* common_name;      /* 通用名称 (CN) */
    char* organization;     /* 组织名称 (O) */
    uint8_t* public_key;    /* 公钥 */
    size_t public_key_len;  /* 公钥长度 */
    crypto_x509_cert_t* next;  /* 下一个证书 (用于证书链) */
};

/* 哈希上下文 (预留) */
typedef struct crypto_hash_ctx crypto_hash_ctx_t;

/*===========================================================
 * 哈希接口
 *===========================================================*/

/**
 * @brief 获取哈希算法摘要长度
 * @param type 哈希类型
 * @return 摘要长度，失败返回 0
 */
size_t crypto_hash_digest_size(crypto_hash_type_t type);

/**
 * @brief 获取哈希算法名称
 * @param type 哈希类型
 * @return 哈希算法名称字符串
 */
const char* crypto_hash_name(crypto_hash_type_t type);

/**
 * @brief 计算哈希值 (一步完成)
 * @param type 哈希类型
 * @param data 输入数据
 * @param datalen 数据长度
 * @param digest 输出摘要缓冲区
 * @param digestlen 缓冲区长度 (输入/输出)
 * @return 0 成功，-1 失败
 */
int crypto_hash(crypto_hash_type_t type, const uint8_t* data, size_t datalen,
                uint8_t* digest, size_t* digestlen);

/*===========================================================
 * HMAC 接口
 *===========================================================*/

/**
 * @brief 计算 HMAC (一步完成)
 * @param type 哈希类型
 * @param key 密钥
 * @param keylen 密钥长度
 * @param data 输入数据
 * @param datalen 数据长度
 * @param mac 输出 MAC 缓冲区
 * @param maclen 缓冲区长度 (输入/输出)
 * @return 0 成功，-1 失败
 */
int crypto_hmac(crypto_hash_type_t type, const uint8_t* key, size_t keylen,
                const uint8_t* data, size_t datalen,
                uint8_t* mac, size_t* maclen);

/*===========================================================
 * 对称加密接口
 *===========================================================*/

/**
 * @brief 获取对称加密算法密钥长度
 * @param type 加密算法类型
 * @return 密钥长度，失败返回 0
 */
size_t crypto_cipher_key_size(crypto_cipher_type_t type);

/**
 * @brief 获取对称加密算法 IV 长度
 * @param type 加密算法类型
 * @return IV 长度，失败返回 0
 */
size_t crypto_cipher_iv_size(crypto_cipher_type_t type);

/**
 * @brief 获取对称加密算法块大小
 * @param type 加密算法类型
 * @return 块大小，失败返回 0
 */
size_t crypto_cipher_block_size(crypto_cipher_type_t type);

/**
 * @brief AES/SM4 加密 (一步完成，PKCS7 填充)
 * @param type 加密算法类型
 * @param key 密钥
 * @param keylen 密钥长度
 * @param iv IV (可选，用于 CBC/CTR/GCM 模式)
 * @param ivlen IV 长度
 * @param aad 附加数据 (仅 GCM 模式使用，可为 NULL)
 * @param aadlen 附加数据长度
 * @param in 明文数据
 * @param inlen 明文长度
 * @param out 密文输出缓冲区
 * @param outlen 缓冲区长度 (输入/输出)
 * @return 0 成功，-1 失败
 */
int crypto_encrypt(crypto_cipher_type_t type,
                   const uint8_t* key, size_t keylen,
                   const uint8_t* iv, size_t ivlen,
                   const uint8_t* aad, size_t aadlen,
                   const uint8_t* in, size_t inlen,
                   uint8_t* out, size_t* outlen);

/**
 * @brief AES/SM4 解密 (一步完成，PKCS7 去填充)
 * @param type 加密算法类型
 * @param key 密钥
 * @param keylen 密钥长度
 * @param iv IV
 * @param ivlen IV 长度
 * @param aad 附加数据 (仅 GCM 模式使用，可为 NULL)
 * @param aadlen 附加数据长度
 * @param in 密文数据
 * @param inlen 密文长度
 * @param out 明文输出缓冲区
 * @param outlen 缓冲区长度 (输入/输出)
 * @return 0 成功，-1 失败
 */
int crypto_decrypt(crypto_cipher_type_t type,
                   const uint8_t* key, size_t keylen,
                   const uint8_t* iv, size_t ivlen,
                   const uint8_t* aad, size_t aadlen,
                   const uint8_t* in, size_t inlen,
                   uint8_t* out, size_t* outlen);

/*===========================================================
 * 随机数接口
 *===========================================================*/

/**
 * @brief 生成随机字节
 * @param buf 输出缓冲区
 * @param buflen 需要的随机字节数
 * @return 0 成功，-1 失败
 */
int crypto_rand_bytes(uint8_t* buf, size_t buflen);

/*===========================================================
 * 编码转换接口
 *===========================================================*/

/**
 * @brief 十六进制字符串转字节数组
 * @param in 十六进制字符串
 * @param inlen 输入长度
 * @param out 输出缓冲区
 * @param outlen 缓冲区长度 (输入/输出)
 * @return 0 成功，-1 失败
 */
int crypto_hex_to_bytes(const char* in, size_t inlen, uint8_t* out, size_t* outlen);

/**
 * @brief 字节数组转十六进制字符串
 * @param in 输入数据
 * @param inlen 输入长度
 * @param out 输出缓冲区
 * @param outlen 缓冲区长度 (输入/输出)
 * @return 0 成功，-1 失败
 */
int crypto_bytes_to_hex(const uint8_t* in, size_t inlen, char* out, size_t* outlen);

/*===========================================================
 * 校验算法接口
 *===========================================================*/

/**
 * @brief 获取校验算法名称
 * @param type 校验类型
 * @return 算法名称
 */
const char* crypto_checksum_name(crypto_checksum_type_t type);

/**
 * @brief 计算校验值 (一步完成)
 * @param type 校验类型
 * @param data 输入数据
 * @param datalen 数据长度
 * @return 校验值，失败返回 0
 */
uint32_t crypto_checksum(crypto_checksum_type_t type, const uint8_t* data, size_t datalen);

/**
 * @brief CRC16 计算 (常用 CRC16/MODBUS 变种)
 * @param data 输入数据
 * @param datalen 数据长度
 * @param init 初值
 * @param xorout 最终异或值
 * @param rev 反转标志
 * @return CRC16 值
 */
uint16_t crypto_crc16(const uint8_t* data, size_t datalen, uint16_t init, uint16_t xorout, int rev);

/**
 * @brief CRC32 计算 (常用 CRC32/MPEG2 变种)
 * @param data 输入数据
 * @param datalen 数据长度
 * @param init 初值
 * @param xorout 最终异或值
 * @param rev 反转标志
 * @return CRC32 值
 */
uint32_t crypto_crc32(const uint8_t* data, size_t datalen, uint32_t init, uint32_t xorout, int rev);

/*===========================================================
 * Base64 编解码接口
 *===========================================================*/

/**
 * @brief Base64 编码
 * @param in 输入数据
 * @param inlen 输入长度
 * @param out 输出缓冲区
 * @param outlen 缓冲区长度 (输入/输出)
 * @return 0 成功，-1 失败
 */
int crypto_base64_encode(const uint8_t* in, size_t inlen, char* out, size_t* outlen);

/**
 * @brief Base64 解码
 * @param in 输入字符串
 * @param inlen 输入长度
 * @param out 输出缓冲区
 * @param outlen 缓冲区长度 (输入/输出)
 * @return 0 成功，-1 失败
 */
int crypto_base64_decode(const char* in, size_t inlen, uint8_t* out, size_t* outlen);

/**
 * @brief 获取 Base64 编码输出长度
 * @param inlen 输入长度
 * @return 需要的输出缓冲区长度
 */
size_t crypto_base64_encode_len(size_t inlen);

/**
 * @brief 获取 Base64 解码输出长度
 * @param inlen 输入长度
 * @return 需要的输出缓冲区长度
 */
size_t crypto_base64_decode_len(size_t inlen);

/*===========================================================
 * X509 证书接口
 *===========================================================*/

/**
 * @brief 从 PEM 字符串解析证书
 * @param pem PEM 格式证书字符串
 * @param pemlen 字符串长度
 * @return 证书结构，失败返回 NULL
 */
crypto_x509_cert_t* crypto_x509_parse_pem(const char* pem, size_t pemlen);

/**
 * @brief 从 DER 数据解析证书
 * @param der DER 格式证书数据
 * @param derlen 数据长度
 * @return 证书结构，失败返回 NULL
 */
crypto_x509_cert_t* crypto_x509_parse_der(const uint8_t* der, size_t derlen);

/**
 * @brief 释放证书结构
 * @param cert 证书结构
 */
void crypto_x509_free(crypto_x509_cert_t* cert);

/**
 * @brief 获取证书主题字符串
 * @param cert 证书
 * @return 主题字符串，需要手动释放
 */
char* crypto_x509_get_subject(const crypto_x509_cert_t* cert);

/**
 * @brief 获取证书颁发者字符串
 * @param cert 证书
 * @return 颁发者字符串，需要手动释放
 */
char* crypto_x509_get_issuer(const crypto_x509_cert_t* cert);

/**
 * @brief 获取证书序列号
 * @param cert 证书
 * @return 序列号字符串，需要手动释放
 */
char* crypto_x509_get_serial_number(const crypto_x509_cert_t* cert);

/**
 * @brief 获取证书通用名称 (CN)
 * @param cert 证书
 * @return CN 字符串，需要手动释放
 */
char* crypto_x509_get_common_name(const crypto_x509_cert_t* cert);

/**
 * @brief 验证证书有效期
 * @param cert 证书
 * @param timestamp 要验证的时间戳 (0 表示当前时间)
 * @return 1 有效，0 无效
 */
int crypto_x509_verify_time(const crypto_x509_cert_t* cert, time_t timestamp);

/*===========================================================
 * PBKDF2 密钥派生接口
 *===========================================================*/

/**
 * @brief PBKDF2 密钥派生
 * @param password 密码
 * @param passwordlen 密码长度
 * @param salt 盐值
 * @param saltlen 盐值长度
 * @param iterations 迭代次数
 * @param keylen 期望输出的密钥长度
 * @param key 输出密钥缓冲区
 * @return 0 成功，-1 失败
 */
int crypto_pbkdf2_sha256(const uint8_t* password, size_t passwordlen,
                         const uint8_t* salt, size_t saltlen,
                         int iterations,
                         size_t keylen, uint8_t* key);

#ifdef __cplusplus
}
#endif

#endif /* IOT_CRYPTO_H */
