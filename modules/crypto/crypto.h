/**
 * @file crypto.h
 * @brief 加密算法库
 *
 * 基于 GmSSL 封装常用加密算法，包括：
 * - 哈希算法: MD5, SHA1, SHA256, SM3
 * - HMAC
 * - 对称加密: AES, SM4 (ECB, CBC, CTR, GCM 模式)
 * - 随机数生成
 * - 十六进制编码/解码
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

/* 哈希上下文 */
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

#ifdef __cplusplus
}
#endif

#endif /* IOT_CRYPTO_H */
