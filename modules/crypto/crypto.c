/**
 * @file crypto.c
 * @brief 加密算法库实现
 *
 * 基于 GmSSL 封装常用加密算法
 */

#include "crypto.h"
#include "iot.h"
#include "iot_log.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* GmSSL 头文件 */
#include <gmssl/mem.h>
#include <gmssl/rand.h>
#include <gmssl/hex.h>
#include <gmssl/sm3.h>
#include <gmssl/sha1.h>
#include <gmssl/sha2.h>
#include <gmssl/hmac.h>
#include <gmssl/aes.h>
#include <gmssl/sm4.h>
#include <gmssl/digest.h>
#include "gmssl/pbkdf2.h"

/*===========================================================
 * 内部函数声明
 *===========================================================*/

static const DIGEST* crypto_hash_to_gmssl_digest(crypto_hash_type_t type);
static const char* crypto_hash_name_internal(crypto_hash_type_t type);
static size_t crypto_hash_digest_size_internal(crypto_hash_type_t type);

/*===========================================================
 * 哈希实现
 *===========================================================*/

static const char* crypto_hash_name_internal(crypto_hash_type_t type) {
    switch (type) {
        case CRYPTO_HASH_MD5:    return "MD5";
        case CRYPTO_HASH_SHA1:   return "SHA1";
        case CRYPTO_HASH_SHA224: return "SHA224";
        case CRYPTO_HASH_SHA256: return "SHA256";
        case CRYPTO_HASH_SHA384: return "SHA384";
        case CRYPTO_HASH_SHA512: return "SHA512";
        case CRYPTO_HASH_SM3:    return "SM3";
        default:                 return NULL;
    }
}

static size_t crypto_hash_digest_size_internal(crypto_hash_type_t type) {
    switch (type) {
        case CRYPTO_HASH_MD5:    return 16;
        case CRYPTO_HASH_SHA1:   return 20;
        case CRYPTO_HASH_SHA224: return 28;
        case CRYPTO_HASH_SHA256: return 32;
        case CRYPTO_HASH_SHA384: return 48;
        case CRYPTO_HASH_SHA512: return 64;
        case CRYPTO_HASH_SM3:    return 32;
        default:                 return 0;
    }
}

static const DIGEST* crypto_hash_to_gmssl_digest(crypto_hash_type_t type) {
    switch (type) {
        case CRYPTO_HASH_SM3:
            return DIGEST_sm3();
#ifdef ENABLE_SHA1
        case CRYPTO_HASH_SHA1:
            return DIGEST_sha1();
#endif
#ifdef ENABLE_SHA2
        case CRYPTO_HASH_SHA224:
            return DIGEST_sha224();
        case CRYPTO_HASH_SHA256:
            return DIGEST_sha256();
        case CRYPTO_HASH_SHA384:
            return DIGEST_sha384();
        case CRYPTO_HASH_SHA512:
            return DIGEST_sha512();
#endif
        default:
            return NULL;
    }
}

size_t crypto_hash_digest_size(crypto_hash_type_t type) {
    return crypto_hash_digest_size_internal(type);
}

const char* crypto_hash_name(crypto_hash_type_t type) {
    return crypto_hash_name_internal(type);
}

int crypto_hash(crypto_hash_type_t type, const uint8_t* data, size_t datalen,
               uint8_t* digest, size_t* digestlen) {
    if (!digest || !digestlen) {
        return -1;
    }

    size_t expected_len = crypto_hash_digest_size_internal(type);
    if (expected_len == 0) {
        LOG_INFO("ERR: unsupported hash type %d", type);
        return -1;
    }

    if (*digestlen < expected_len) {
        LOG_INFO("ERR: digest buffer too small");
        *digestlen = expected_len;
        return -1;
    }

    /* SM3 使用独立接口 */
    if (type == CRYPTO_HASH_SM3) {
        SM3_CTX ctx;
        sm3_init(&ctx);
        sm3_update(&ctx, data, datalen);
        sm3_finish(&ctx, digest);
        *digestlen = expected_len;
        return 0;
    }

    /* 其他哈希使用通用 DIGEST 接口 */
    const DIGEST* digest_alg = crypto_hash_to_gmssl_digest(type);
    if (!digest_alg) {
        LOG_INFO("ERR: digest algorithm not available");
        return -1;
    }

    if (digest_init(NULL, digest_alg) != 1) {
        return -1;
    }

    DIGEST_CTX ctx;
    if (digest_init(&ctx, digest_alg) != 1) {
        return -1;
    }
    digest_update(&ctx, data, datalen);
    size_t outlen = expected_len;
    digest_finish(&ctx, digest, &outlen);
    *digestlen = outlen;

    return 0;
}

/*===========================================================
 * HMAC 实现
 *===========================================================*/

int crypto_hmac(crypto_hash_type_t type, const uint8_t* key, size_t keylen,
               const uint8_t* data, size_t datalen,
               uint8_t* mac, size_t* maclen) {
    if (!mac || !maclen) {
        return -1;
    }

    /* SM3 HMAC 使用独立接口 */
    if (type == CRYPTO_HASH_SM3) {
        size_t expected_len = SM3_DIGEST_SIZE;
        if (*maclen < expected_len) {
            *maclen = expected_len;
            return -1;
        }
        SM3_HMAC_CTX ctx;
        sm3_hmac_init(&ctx, key, keylen);
        sm3_hmac_update(&ctx, data, datalen);
        sm3_hmac_finish(&ctx, mac);
        *maclen = expected_len;
        return 0;
    }

    /* 其他 HMAC 使用通用接口 */
    const DIGEST* digest_alg = crypto_hash_to_gmssl_digest(type);
    if (!digest_alg) {
        LOG_INFO("ERR: digest algorithm not available for HMAC");
        return -1;
    }

    size_t outlen = *maclen;
    if (hmac(digest_alg, key, keylen, data, datalen, mac, &outlen) != 1) {
        return -1;
    }
    *maclen = outlen;

    return 0;
}

/*===========================================================
 * 对称加密实现
 *===========================================================*/

size_t crypto_cipher_key_size(crypto_cipher_type_t type) {
    switch (type) {
        case CRYPTO_CIPHER_AES_128_ECB:
        case CRYPTO_CIPHER_AES_128_CBC:
        case CRYPTO_CIPHER_AES_128_CTR:
        case CRYPTO_CIPHER_AES_128_GCM:
            return 16;

        case CRYPTO_CIPHER_AES_192_ECB:
        case CRYPTO_CIPHER_AES_192_CBC:
        case CRYPTO_CIPHER_AES_192_CTR:
        case CRYPTO_CIPHER_AES_192_GCM:
            return 24;

        case CRYPTO_CIPHER_AES_256_ECB:
        case CRYPTO_CIPHER_AES_256_CBC:
        case CRYPTO_CIPHER_AES_256_CTR:
        case CRYPTO_CIPHER_AES_256_GCM:
            return 32;

        case CRYPTO_CIPHER_SM4_ECB:
        case CRYPTO_CIPHER_SM4_CBC:
        case CRYPTO_CIPHER_SM4_CTR:
        case CRYPTO_CIPHER_SM4_GCM:
            return 16;

        default:
            return 0;
    }
}

size_t crypto_cipher_iv_size(crypto_cipher_type_t type) {
    switch (type) {
        case CRYPTO_CIPHER_AES_128_ECB:
        case CRYPTO_CIPHER_AES_192_ECB:
        case CRYPTO_CIPHER_AES_256_ECB:
        case CRYPTO_CIPHER_SM4_ECB:
            return 0;  /* ECB 模式不需要 IV */

        case CRYPTO_CIPHER_AES_128_CBC:
        case CRYPTO_CIPHER_AES_192_CBC:
        case CRYPTO_CIPHER_AES_256_CBC:
        case CRYPTO_CIPHER_SM4_CBC:
            return 16;

        case CRYPTO_CIPHER_AES_128_CTR:
        case CRYPTO_CIPHER_AES_192_CTR:
        case CRYPTO_CIPHER_AES_256_CTR:
        case CRYPTO_CIPHER_SM4_CTR:
            return 16;

        case CRYPTO_CIPHER_AES_128_GCM:
        case CRYPTO_CIPHER_AES_192_GCM:
        case CRYPTO_CIPHER_AES_256_GCM:
        case CRYPTO_CIPHER_SM4_GCM:
            return 12;  /* GCM 推荐 12 字节 IV */

        default:
            return 0;
    }
}

size_t crypto_cipher_block_size(crypto_cipher_type_t type) {
    switch (type) {
        case CRYPTO_CIPHER_AES_128_ECB:
        case CRYPTO_CIPHER_AES_128_CBC:
        case CRYPTO_CIPHER_AES_128_CTR:
        case CRYPTO_CIPHER_AES_128_GCM:
        case CRYPTO_CIPHER_AES_192_ECB:
        case CRYPTO_CIPHER_AES_192_CBC:
        case CRYPTO_CIPHER_AES_192_CTR:
        case CRYPTO_CIPHER_AES_192_GCM:
        case CRYPTO_CIPHER_AES_256_ECB:
        case CRYPTO_CIPHER_AES_256_CBC:
        case CRYPTO_CIPHER_AES_256_CTR:
        case CRYPTO_CIPHER_AES_256_GCM:
            return 16;

        case CRYPTO_CIPHER_SM4_ECB:
        case CRYPTO_CIPHER_SM4_CBC:
        case CRYPTO_CIPHER_SM4_CTR:
        case CRYPTO_CIPHER_SM4_GCM:
            return 16;

        default:
            return 0;
    }
}

/* 判断是否为 GCM 模式 */
static int cipher_is_gcm(crypto_cipher_type_t type) {
    return (type == CRYPTO_CIPHER_AES_128_GCM ||
            type == CRYPTO_CIPHER_AES_192_GCM ||
            type == CRYPTO_CIPHER_AES_256_GCM ||
            type == CRYPTO_CIPHER_SM4_GCM);
}

/* 判断是否为 CTR 模式 */
static int cipher_is_ctr(crypto_cipher_type_t type) {
    return (type == CRYPTO_CIPHER_AES_128_CTR ||
            type == CRYPTO_CIPHER_AES_192_CTR ||
            type == CRYPTO_CIPHER_AES_256_CTR ||
            type == CRYPTO_CIPHER_SM4_CTR);
}

/* 判断是否为 CBC 模式 */
static int cipher_is_cbc(crypto_cipher_type_t type) {
    return (type == CRYPTO_CIPHER_AES_128_CBC ||
            type == CRYPTO_CIPHER_AES_192_CBC ||
            type == CRYPTO_CIPHER_AES_256_CBC ||
            type == CRYPTO_CIPHER_SM4_CBC);
}

/* 判断是否为 ECB 模式 */
static int cipher_is_ecb(crypto_cipher_type_t type) {
    return (type == CRYPTO_CIPHER_AES_128_ECB ||
            type == CRYPTO_CIPHER_AES_192_ECB ||
            type == CRYPTO_CIPHER_AES_256_ECB ||
            type == CRYPTO_CIPHER_SM4_ECB);
}

/* 判断是否为 SM4 */
static int cipher_is_sm4(crypto_cipher_type_t type) {
    return (type == CRYPTO_CIPHER_SM4_ECB ||
            type == CRYPTO_CIPHER_SM4_CBC ||
            type == CRYPTO_CIPHER_SM4_CTR ||
            type == CRYPTO_CIPHER_SM4_GCM);
}

/* 判断是否为 AES */
static int cipher_is_aes(crypto_cipher_type_t type) {
    return (type >= CRYPTO_CIPHER_AES_128_ECB && type <= CRYPTO_CIPHER_AES_256_GCM);
}

int crypto_encrypt(crypto_cipher_type_t type,
                   const uint8_t* key, size_t keylen,
                   const uint8_t* iv, size_t ivlen,
                   const uint8_t* aad, size_t aadlen,
                   const uint8_t* in, size_t inlen,
                   uint8_t* out, size_t* outlen) {
    if (!in || !out || !outlen) {
        return -1;
    }

    size_t expected_keylen = crypto_cipher_key_size(type);
    size_t expected_ivlen = crypto_cipher_iv_size(type);

    if (expected_keylen == 0 || keylen != expected_keylen) {
        LOG_INFO("ERR: invalid key length %d, expected %d", (int)keylen, (int)expected_keylen);
        return -1;
    }

    if (expected_ivlen > 0 && ivlen != expected_ivlen) {
        LOG_INFO("ERR: invalid IV length %d, expected %d", (int)ivlen, (int)expected_ivlen);
        return -1;
    }

    /* GCM 模式 */
    if (cipher_is_gcm(type)) {
        if (cipher_is_sm4(type)) {
            /* SM4-GCM */
            LOG_INFO("ERR: SM4-GCM not implemented");
            return -1;
        } else {
            /* AES-GCM */
            AES_KEY aes_key;
            if (aes_set_encrypt_key(&aes_key, key, keylen) != 1) {
                return -1;
            }
            size_t taglen = 16;
            if (aes_gcm_encrypt(&aes_key, iv, ivlen, aad, aadlen, in, inlen, out, taglen, out + inlen) != 1) {
                return -1;
            }
            *outlen = inlen + taglen;
            return 0;
        }
    }

    /* 计算输出缓冲区需要的最小大小 */
    size_t block_size = crypto_cipher_block_size(type);
    size_t min_outlen = inlen;

    if (cipher_is_cbc(type) || cipher_is_ecb(type)) {
        /* CBC 和 ECB 需要 PKCS7 填充 */
        min_outlen = ((inlen / block_size) + 1) * block_size;
    }

    if (*outlen < min_outlen) {
        *outlen = min_outlen;
        return -1;
    }

    /* ECB 模式 */
    if (cipher_is_ecb(type)) {
        if (cipher_is_sm4(type)) {
            SM4_KEY sm4_key;
            sm4_set_encrypt_key(&sm4_key, key);
            size_t blocks = (inlen + block_size - 1) / block_size;
            uint8_t tmp[16] = {0};
            memcpy(tmp, in, inlen);
            /* 简单 PKCS7 填充 */
            uint8_t pad = (uint8_t)(block_size - inlen % block_size);
            memset(tmp + inlen, pad, pad);
            for (size_t i = 0; i < blocks; i++) {
                sm4_encrypt(&sm4_key, tmp + i * block_size, out + i * block_size);
            }
            *outlen = blocks * block_size;
        } else {
            AES_KEY aes_key;
            if (aes_set_encrypt_key(&aes_key, key, keylen) != 1) {
                return -1;
            }
            size_t blocks = (inlen + block_size - 1) / block_size;
            uint8_t tmp[16] = {0};
            memcpy(tmp, in, inlen);
            uint8_t pad = (uint8_t)(block_size - inlen % block_size);
            memset(tmp + inlen, pad, pad);
            for (size_t i = 0; i < blocks; i++) {
                aes_encrypt(&aes_key, tmp + i * block_size, out + i * block_size);
            }
            *outlen = blocks * block_size;
        }
        return 0;
    }

    /* CBC 模式 */
    if (cipher_is_cbc(type)) {
        if (cipher_is_sm4(type)) {
            SM4_KEY sm4_key;
            sm4_set_encrypt_key(&sm4_key, key);
            size_t outlen_tmp = *outlen;
            if (sm4_cbc_padding_encrypt(&sm4_key, iv, in, inlen, out, &outlen_tmp) != 1) {
                return -1;
            }
            *outlen = outlen_tmp;
        } else {
            AES_KEY aes_key;
            if (aes_set_encrypt_key(&aes_key, key, keylen) != 1) {
                return -1;
            }
            size_t outlen_tmp = *outlen;
            if (aes_cbc_padding_encrypt(&aes_key, iv, in, inlen, out, &outlen_tmp) != 1) {
                return -1;
            }
            *outlen = outlen_tmp;
        }
        return 0;
    }

    /* CTR 模式 */
    if (cipher_is_ctr(type)) {
        if (cipher_is_sm4(type)) {
            SM4_KEY sm4_key;
            uint8_t ctr[SM4_BLOCK_SIZE];
            sm4_set_encrypt_key(&sm4_key, key);
            memcpy(ctr, iv, SM4_BLOCK_SIZE);
            sm4_ctr_encrypt(&sm4_key, ctr, in, inlen, out);
            *outlen = inlen;
        } else {
            AES_KEY aes_key;
            if (aes_set_encrypt_key(&aes_key, key, keylen) != 1) {
                return -1;
            }
            aes_ctr_encrypt(&aes_key, (uint8_t*)iv, in, inlen, out);
            *outlen = inlen;
        }
        return 0;
    }

    LOG_INFO("ERR: unsupported cipher type %d", type);
    return -1;
}

int crypto_decrypt(crypto_cipher_type_t type,
                   const uint8_t* key, size_t keylen,
                   const uint8_t* iv, size_t ivlen,
                   const uint8_t* aad, size_t aadlen,
                   const uint8_t* in, size_t inlen,
                   uint8_t* out, size_t* outlen) {
    if (!in || !out || !outlen) {
        return -1;
    }

    size_t expected_keylen = crypto_cipher_key_size(type);
    size_t expected_ivlen = crypto_cipher_iv_size(type);

    if (expected_keylen == 0 || keylen != expected_keylen) {
        LOG_INFO("ERR: invalid key length");
        return -1;
    }

    if (expected_ivlen > 0 && ivlen != expected_ivlen) {
        LOG_INFO("ERR: invalid IV length");
        return -1;
    }

    /* GCM 模式 */
    if (cipher_is_gcm(type)) {
        if (cipher_is_sm4(type)) {
            LOG_INFO("ERR: SM4-GCM not implemented");
            return -1;
        } else {
            /* AES-GCM */
            AES_KEY aes_key;
            if (aes_set_decrypt_key(&aes_key, key, keylen) != 1) {
                return -1;
            }
            if (inlen < 16) {
                return -1;
            }
            size_t taglen = 16;
            size_t ciphertext_len = inlen - taglen;
            if (aes_gcm_decrypt(&aes_key, iv, ivlen, aad, aadlen, in, ciphertext_len,
                                (uint8_t*)in + ciphertext_len, taglen, out) != 1) {
                LOG_INFO("ERR: GCM authentication failed");
                return -1;
            }
            *outlen = ciphertext_len;
            return 0;
        }
    }

    size_t block_size = crypto_cipher_block_size(type);

    /* ECB 模式 */
    if (cipher_is_ecb(type)) {
        if (inlen % block_size != 0) {
            LOG_INFO("ERR: invalid ciphertext length");
            return -1;
        }
        if (*outlen < inlen) {
            *outlen = inlen;
            return -1;
        }
        if (cipher_is_sm4(type)) {
            SM4_KEY sm4_key;
            sm4_set_decrypt_key(&sm4_key, key);
            size_t blocks = inlen / block_size;
            for (size_t i = 0; i < blocks; i++) {
                sm4_encrypt(&sm4_key, in + i * block_size, out + i * block_size);
            }
        } else {
            AES_KEY aes_key;
            if (aes_set_decrypt_key(&aes_key, key, keylen) != 1) {
                return -1;
            }
            size_t blocks = inlen / block_size;
            for (size_t i = 0; i < blocks; i++) {
                aes_decrypt(&aes_key, in + i * block_size, out + i * block_size);
            }
        }
        /* 去除 PKCS7 填充 */
        uint8_t pad = out[inlen - 1];
        if (pad == 0 || pad > block_size) {
            LOG_INFO("ERR: invalid padding");
            return -1;
        }
        *outlen = inlen - pad;
        return 0;
    }

    /* CBC 模式 */
    if (cipher_is_cbc(type)) {
        if (cipher_is_sm4(type)) {
            SM4_KEY sm4_key;
            sm4_set_decrypt_key(&sm4_key, key);
            size_t outlen_tmp = *outlen;
            if (sm4_cbc_padding_decrypt(&sm4_key, iv, in, inlen, out, &outlen_tmp) != 1) {
                return -1;
            }
            *outlen = outlen_tmp;
        } else {
            AES_KEY aes_key;
            if (aes_set_decrypt_key(&aes_key, key, keylen) != 1) {
                return -1;
            }
            size_t outlen_tmp = *outlen;
            if (aes_cbc_padding_decrypt(&aes_key, iv, in, inlen, out, &outlen_tmp) != 1) {
                return -1;
            }
            *outlen = outlen_tmp;
        }
        return 0;
    }

    /* CTR 模式 */
    if (cipher_is_ctr(type)) {
        if (cipher_is_sm4(type)) {
            SM4_KEY sm4_key;
            uint8_t ctr[SM4_BLOCK_SIZE];
            sm4_set_encrypt_key(&sm4_key, key);  /* CTR 模式解密也用加密密钥 */
            memcpy(ctr, iv, SM4_BLOCK_SIZE);
            sm4_ctr_encrypt(&sm4_key, ctr, in, inlen, out);
            *outlen = inlen;
        } else {
            AES_KEY aes_key;
            if (aes_set_encrypt_key(&aes_key, key, keylen) != 1) {
                return -1;
            }
            aes_ctr_encrypt(&aes_key, (uint8_t*)iv, in, inlen, out);
            *outlen = inlen;
        }
        return 0;
    }

    LOG_INFO("ERR: unsupported cipher type %d", type);
    return -1;
}

/*===========================================================
 * 随机数实现
 *===========================================================*/

int crypto_rand_bytes(uint8_t* buf, size_t buflen) {
    if (!buf) {
        return -1;
    }
    if (buflen > RAND_BYTES_MAX_SIZE) {
        buflen = RAND_BYTES_MAX_SIZE;
    }
    return rand_bytes(buf, buflen);
}

/*===========================================================
 * 编码转换实现
 *===========================================================*/

int crypto_hex_to_bytes(const char* in, size_t inlen, uint8_t* out, size_t* outlen) {
    if (!in || !out || !outlen) {
        return -1;
    }
    if (*outlen < inlen / 2) {
        *outlen = inlen / 2;
        return -1;
    }
    size_t outlen_tmp = *outlen;
    if (hex_to_bytes(in, inlen, out, &outlen_tmp) != 1) {
        return -1;
    }
    *outlen = outlen_tmp;
    return 0;
}

int crypto_bytes_to_hex(const uint8_t* in, size_t inlen, char* out, size_t* outlen) {
    if (!in || !out || !outlen) {
        return -1;
    }
    /* 十六进制输出需要 2 倍长度 + 1 for null */
    if (*outlen < inlen * 2 + 1) {
        *outlen = inlen * 2 + 1;
        return -1;
    }
    for (size_t i = 0; i < inlen; i++) {
        snprintf(out + i * 2, 3, "%02x", in[i]);
    }
    *outlen = inlen * 2;
    return 0;
}

/*===========================================================
 * Base64 编解码实现
 *===========================================================*/

/* Base64 编码表 */
static const char base64_encode_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/* Base64 解码表 */
static const int8_t base64_decode_table[256] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
    -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

size_t crypto_base64_encode_len(size_t inlen) {
    /* 每个3字节组编码为4字符，加上可能的填充 */
    return ((inlen + 2) / 3) * 4 + 1;
}

size_t crypto_base64_decode_len(size_t inlen) {
    /* 每个4字符组解码为3字节 */
    return (inlen / 4) * 3 + 2;
}

int crypto_base64_encode(const uint8_t* in, size_t inlen, char* out, size_t* outlen) {
    if (!in || !out || !outlen) {
        return -1;
    }

    size_t needed_len = crypto_base64_encode_len(inlen);
    if (*outlen < needed_len) {
        *outlen = needed_len;
        return -1;
    }

    size_t i, j;
    j = 0;

    for (i = 0; i < inlen; i += 3) {
        uint32_t v = (uint32_t)in[i] << 16;
        if (i + 1 < inlen) {
            v |= (uint32_t)in[i + 1] << 8;
        }
        if (i + 2 < inlen) {
            v |= (uint32_t)in[i + 2];
        }

        out[j++] = base64_encode_table[(v >> 18) & 0x3F];
        out[j++] = base64_encode_table[(v >> 12) & 0x3F];

        if (i + 1 < inlen) {
            out[j++] = base64_encode_table[(v >> 6) & 0x3F];
        } else {
            out[j++] = '=';
        }

        if (i + 2 < inlen) {
            out[j++] = base64_encode_table[v & 0x3F];
        } else {
            out[j++] = '=';
        }
    }

    out[j] = '\0';
    *outlen = j;

    return 0;
}

int crypto_base64_decode(const char* in, size_t inlen, uint8_t* out, size_t* outlen) {
    if (!in || !out || !outlen) {
        return -1;
    }

    /* 移除结尾的 '=' 填充字符 */
    while (inlen > 0 && in[inlen - 1] == '=') {
        inlen--;
    }

    size_t needed_len = crypto_base64_decode_len(inlen);
    if (*outlen < needed_len) {
        *outlen = needed_len;
        return -1;
    }

    size_t i, j;
    j = 0;

    for (i = 0; i < inlen; i += 4) {
        int8_t v0, v1, v2, v3;

        v0 = base64_decode_table[(unsigned char)in[i]];
        v1 = (i + 1 < inlen) ? base64_decode_table[(unsigned char)in[i + 1]] : -1;
        v2 = (i + 2 < inlen) ? base64_decode_table[(unsigned char)in[i + 2]] : -1;
        v3 = (i + 3 < inlen) ? base64_decode_table[(unsigned char)in[i + 3]] : -1;

        if (v0 < 0 || v1 < 0) {
            return -1;
        }

        uint32_t v = ((uint32_t)v0 << 18) | ((uint32_t)v1 << 12);

        out[j++] = (uint8_t)(v >> 16);

        if (v2 >= 0) {
            v |= ((uint32_t)v2 << 6);
            out[j++] = (uint8_t)(v >> 8);
        }

        if (v3 >= 0) {
            v |= ((uint32_t)v3);
            out[j++] = (uint8_t)v;
        }
    }

    *outlen = j;

    return 0;
}

/*===========================================================
 * X509 证书实现
 *===========================================================*/

/* 简化 X509 证书解析 - 使用 mbedtls */
#ifdef USE_MBEDTLS
#include <mbedtls/x509_crt.h>
#include <mbedtls/oid.h>

crypto_x509_cert_t* crypto_x509_parse_pem(const char* pem, size_t pemlen) {
    if (!pem || pemlen == 0) {
        return NULL;
    }

    mbedtls_x509_crt cert;
    mbedtls_x509_crt_init(&cert);

    int ret = mbedtls_x509_crt_parse(&cert, (const unsigned char*)pem, pemlen + 1);
    if (ret != 0) {
        mbedtls_x509_crt_free(&cert);
        return NULL;
    }

    crypto_x509_cert_t* out = (crypto_x509_cert_t*)malloc(sizeof(crypto_x509_cert_t));
    if (!out) {
        mbedtls_x509_crt_free(&cert);
        return NULL;
    }

    memset(out, 0, sizeof(crypto_x509_cert_t));

    /* 复制主题 */
    char buf[512];
    ret = mbedtls_x509_get_subject(&cert, buf, sizeof(buf));
    if (ret == 0) {
        out->subject = strdup(buf);
    }

    /* 复制颁发者 */
    ret = mbedtls_x509_get_issuer(&cert, buf, sizeof(buf));
    if (ret == 0) {
        out->issuer = strdup(buf);
    }

    /* 复制序列号 */
    ret = mbedtls_x509_get_serial(&cert, buf, sizeof(buf));
    if (ret == 0) {
        out->serial_number = strdup(buf);
    }

    /* 复制有效期 */
    if (cert.valid_from) {
        out->not_before = strdup(cert.valid_from);
    }
    if (cert.valid_to) {
        out->not_after = strdup(cert.valid_to);
    }

    /* 提取 CN */
    char* cn = strstr(buf, "CN=");
    if (cn) {
        cn += 3;
        char* end = strchr(cn, ',');
        if (end) {
            out->common_name = strndup(cn, end - cn);
        } else {
            out->common_name = strdup(cn);
        }
    }

    mbedtls_x509_crt_free(&cert);
    return out;
}

crypto_x509_cert_t* crypto_x509_parse_der(const uint8_t* der, size_t derlen) {
    if (!der || derlen == 0) {
        return NULL;
    }

    mbedtls_x509_crt cert;
    mbedtls_x509_crt_init(&cert);

    int ret = mbedtls_x509_crt_parse(&cert, der, derlen);
    if (ret != 0) {
        mbedtls_x509_crt_free(&cert);
        return NULL;
    }

    crypto_x509_cert_t* out = (crypto_x509_cert_t*)malloc(sizeof(crypto_x509_cert_t));
    if (!out) {
        mbedtls_x509_crt_free(&cert);
        return NULL;
    }

    memset(out, 0, sizeof(crypto_x509_cert_t));

    char buf[512];
    int ret2 = mbedtls_x509_get_subject(&cert, buf, sizeof(buf));
    if (ret2 == 0) {
        out->subject = strdup(buf);
    }

    ret2 = mbedtls_x509_get_issuer(&cert, buf, sizeof(buf));
    if (ret2 == 0) {
        out->issuer = strdup(buf);
    }

    mbedtls_x509_crt_free(&cert);
    return out;
}

#else
/* 简化的占位实现 - 无需外部库 */

crypto_x509_cert_t* crypto_x509_parse_pem(const char* pem, size_t pemlen) {
    (void)pem;
    (void)pemlen;
    /* 实际实现需要完整的 ASN.1/DER 解析器 */
    /* 这里返回 NULL 表示功能不可用 */
    return NULL;
}

crypto_x509_cert_t* crypto_x509_parse_der(const uint8_t* der, size_t derlen) {
    (void)der;
    (void)derlen;
    return NULL;
}
#endif /* USE_MBEDTLS */

void crypto_x509_free(crypto_x509_cert_t* cert) {
    if (!cert) {
        return;
    }

    if (cert->subject) free(cert->subject);
    if (cert->issuer) free(cert->issuer);
    if (cert->serial_number) free(cert->serial_number);
    if (cert->not_before) free(cert->not_before);
    if (cert->not_after) free(cert->not_after);
    if (cert->common_name) free(cert->common_name);
    if (cert->organization) free(cert->organization);
    if (cert->public_key) free(cert->public_key);
    if (cert->next) crypto_x509_free(cert->next);

    free(cert);
}

char* crypto_x509_get_subject(const crypto_x509_cert_t* cert) {
    if (!cert) return NULL;
    return cert->subject ? strdup(cert->subject) : NULL;
}

char* crypto_x509_get_issuer(const crypto_x509_cert_t* cert) {
    if (!cert) return NULL;
    return cert->issuer ? strdup(cert->issuer) : NULL;
}

char* crypto_x509_get_serial_number(const crypto_x509_cert_t* cert) {
    if (!cert) return NULL;
    return cert->serial_number ? strdup(cert->serial_number) : NULL;
}

char* crypto_x509_get_common_name(const crypto_x509_cert_t* cert) {
    if (!cert) return NULL;
    return cert->common_name ? strdup(cert->common_name) : NULL;
}

int crypto_x509_verify_time(const crypto_x509_cert_t* cert, time_t timestamp) {
    (void)cert;
    (void)timestamp;
    /* 需要实现时间验证 */
    return 1;
}

/*===========================================================
 * PBKDF2 实现
 *===========================================================*/

int crypto_pbkdf2_sha256(const uint8_t* password, size_t passwordlen,
                         const uint8_t* salt, size_t saltlen,
                         int iterations,
                         size_t keylen, uint8_t* key) {
    if (!password || !salt || !key) {
        return -1;
    }

    const DIGEST* digest = crypto_hash_to_gmssl_digest(CRYPTO_HASH_SHA256);
    if (!digest) {
        return -1;
    }

    if (pbkdf2_hmac_genkey(digest, (const char*)password, passwordlen,
                           salt, saltlen, (size_t)iterations, keylen, key) != 1) {
        return -1;
    }

    return 0;
}
