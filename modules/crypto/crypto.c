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
#include "deflate.h"

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
        LOG("ERR: unsupported hash type %d", type);
        return -1;
    }

    if (*digestlen < expected_len) {
        LOG("ERR: digest buffer too small");
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
        LOG("ERR: digest algorithm not available");
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
        LOG("ERR: digest algorithm not available for HMAC");
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
        LOG("ERR: invalid key length %d, expected %d", (int)keylen, (int)expected_keylen);
        return -1;
    }

    if (expected_ivlen > 0 && ivlen != expected_ivlen) {
        LOG("ERR: invalid IV length %d, expected %d", (int)ivlen, (int)expected_ivlen);
        return -1;
    }

    /* GCM 模式 */
    if (cipher_is_gcm(type)) {
        if (cipher_is_sm4(type)) {
            /* SM4-GCM */
            LOG("ERR: SM4-GCM not implemented");
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

    LOG("ERR: unsupported cipher type %d", type);
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
        LOG("ERR: invalid key length");
        return -1;
    }

    if (expected_ivlen > 0 && ivlen != expected_ivlen) {
        LOG("ERR: invalid IV length");
        return -1;
    }

    /* GCM 模式 */
    if (cipher_is_gcm(type)) {
        if (cipher_is_sm4(type)) {
            LOG("ERR: SM4-GCM not implemented");
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
                LOG("ERR: GCM authentication failed");
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
            LOG("ERR: invalid ciphertext length");
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
            LOG("ERR: invalid padding");
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

    LOG("ERR: unsupported cipher type %d", type);
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
 * 校验算法实现
 *===========================================================*/

/* CRC16 查找表 (CRC-16/MODBUS) */
static const uint16_t crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

/* CRC32 查找表 (CRC-32/MPEG2) */
static const uint32_t crc32_table[256] = {
    0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9, 0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475B05,
    0x2608EDB8, 0x22C9F20F, 0x2F0ED5D6, 0x2B0CD461, 0x35B63A64, 0x31B72DD3, 0x3C80190A, 0x384106BD,
    0x4C11DB28, 0x48D6C79F, 0x459E0146, 0x415FD4F1, 0x5F1E8FF4, 0x5BFF8243, 0x56BBA69A, 0x52BABD2D,
    0x6ABBB690, 0x6EBA0727, 0x633F21FE, 0x673E3E49, 0x7909654C, 0x7D0878FB, 0x704A5E22, 0x744B4395,
    0x9A579EC8, 0x9E56837F, 0x931AA5A6, 0x971BB111, 0x89B6EAF4, 0x8DB7F743, 0x80F4D19A, 0x84F5CC2D,
    0xACB4D890, 0xA8B5C727, 0xA5F2E1FE, 0xA1F3FC49, 0xBFBEA74C, 0xBBFFBAFB, 0xB6BC9C22, 0xB2BD8195,
    0xD6C2C200, 0xD2C3DFB7, 0xDF80F96E, 0xDB81E6D9, 0xC5C4BDDC, 0xC1C5A06B, 0xCC8686B2, 0xC8879305,
    0xF0C693B8, 0xF4C78E0F, 0xF980A8D6, 0xFD81BF61, 0xE3C4E464, 0xE7C5F9D3, 0xEA86DF0A, 0xEE87C2BD,
    0x3D2ECDC0, 0x392FD277, 0x346C140E, 0x306D29B9, 0x3E0872BC, 0x3A096F0B, 0x37AA49D2, 0x33AB5465,
    0x0BEA52D8, 0x0FEB4F6F, 0x02A869B6, 0x06A97401, 0x18DC2F04, 0x1CDD32B3, 0x119E146A, 0x159F09DD,
    0x71D44A48, 0x75D557FF, 0x78967126, 0x7C976C91, 0x62D23794, 0x66D32A23, 0x6B900CFA, 0x6F91114D,
    0x57D015F0, 0x53D10847, 0x5E922E9E, 0x5A933329, 0x44D4682C, 0x40D5759B, 0x4D965342, 0x49974EF5,
    0xA70CEE50, 0xA30DF3E7, 0xAE4ED53E, 0xAA4FC889, 0xB408938C, 0xB0098E3B, 0xBDA4A8E2, 0xB9A5BF55,
    0x81E4B9E8, 0x85E5A45F, 0x88A68286, 0x8CA79F31, 0x92C1C434, 0x96C0D983, 0x9B83FF5A, 0x9F82E2ED,
    0xEBC9A178, 0xEFC8BCCF, 0xE28B9A16, 0xE68A87A1, 0xF8CDDCA4, 0xFCCCC113, 0xF18FE7CA, 0xF58EFA7D,
    0xCDCFECC0, 0xC9CEF177, 0xC48DD7AE, 0xC08CCA19, 0xDECB911C, 0xDACA8CAB, 0xD789AA72, 0xD388B7C5,
    0x4AD6E7A0, 0x4ED7FA17, 0x4394DCCE, 0x4795C179, 0x59D29A7C, 0x5DD387CB, 0x5090A112, 0x5491BCA5,
    0x6CD0B818, 0x68D1A5AF, 0x65928376, 0x61939EC1, 0x7FD4C5C4, 0x7BD5D873, 0x7696FEAA, 0x7297E31D,
    0x06DCA088, 0x02DDBD3F, 0x0F9E9BE6, 0x0B9F8651, 0x15D8DD54, 0x11D9C0E3, 0x1C9AE63A, 0x189BFB8D,
    0x20DABD30, 0x24DBA087, 0x2998865E, 0x2D999BE9, 0x33DEC0EC, 0x37DFDD5B, 0x3A9CFB82, 0x3E9DE635,
    0x5A1A4F88, 0x5E1B523F, 0x535874E6, 0x57596951, 0x491E3254, 0x4D1F2FE3, 0x405C093A, 0x445D148D,
    0x7C1C1230, 0x781D0F87, 0x755E295E, 0x715F34E9, 0x6F186FEC, 0x6B19725B, 0x665A5482, 0x625B4935,
    0x1610EAA0, 0x1211F717, 0x1F52D1CE, 0x1B53CC79, 0x0514977C, 0x01158ACB, 0x0C56AC12, 0x0857B1A5,
    0x3016B718, 0x3417AAAF, 0x39548C76, 0x3D5591C1, 0x2312CAC4, 0x2713D773, 0x2A50F1AA, 0x2E51EC1D,
    0xBAD6E748, 0xBED7FADF, 0xB394DC06, 0xB795C1B1, 0xA9D29AB4, 0xADD38703, 0xA090A1DA, 0xA491BC6D,
    0x9CD0B8D0, 0x98D1A567, 0x959283BE, 0x91939E09, 0x8FD4C50C, 0x8BD5D8BB, 0x8696FE62, 0x8297E3D5,
    0xF6DCA050, 0xF2DDBDE7, 0xFF9E9B3E, 0xFB9F8689, 0xE5D8DD8C, 0xE1D9C03B, 0xEC9AE6E2, 0xE89BFB55,
    0xD0DAFFE8, 0xD4DBE25F, 0xD998C486, 0xDD99D931, 0xC3DC8234, 0xC7DD9F83, 0xCA9EB95A, 0xCE9FA4ED,
    0xAA1838D8, 0xAE19256F, 0xA35A03B6, 0xA75B1E01, 0xB91C4504, 0xBD1D58B3, 0xB05E7E6A, 0xB45F63DD,
    0x8C1E6760, 0x881F7AD7, 0x855C5C0E, 0x815D41B9, 0x9F1A1ABC, 0x9B1B074B, 0x96582192, 0x92593C25,
    0x0E147FB0, 0x0A156207, 0x075644DE, 0x03575969, 0x2D10026C, 0x29111FDB, 0x24523902, 0x205324B5,
    0x18122208, 0x1C133FBF, 0x11501966, 0x155104D1, 0x0B165FD4, 0x0F174263, 0x025464BA, 0x0655790D
};

/* Adler32 查找表 */
#define BASE 65521
#define NMAX 5552

static const char* crypto_checksum_name_internal(crypto_checksum_type_t type) {
    switch (type) {
        case CRYPTO_CHECKSUM_CRC16:   return "CRC16";
        case CRYPTO_CHECKSUM_CRC32:   return "CRC32";
        case CRYPTO_CHECKSUM_ADLER32: return "ADLER32";
        default:                       return NULL;
    }
}

const char* crypto_checksum_name(crypto_checksum_type_t type) {
    return crypto_checksum_name_internal(type);
}

uint32_t crypto_checksum(crypto_checksum_type_t type, const uint8_t* data, size_t datalen) {
    switch (type) {
        case CRYPTO_CHECKSUM_CRC16:
            return crypto_crc16(data, datalen, 0xFFFF, 0x0000, 1);
        case CRYPTO_CHECKSUM_CRC32:
            return crypto_crc32(data, datalen, 0xFFFFFFFF, 0x00000000, 1);
        case CRYPTO_CHECKSUM_ADLER32:
            return zlib_adler32(0, data, datalen);
        default:
            return 0;
    }
}

uint16_t crypto_crc16(const uint8_t* data, size_t datalen, uint16_t init, uint16_t xorout, int rev) {
    uint16_t crc = init;

    if (rev) {
        /* 反转模式 */
        for (size_t i = 0; i < datalen; i++) {
            crc = (crc >> 8) ^ crc16_table[(crc ^ data[i]) & 0xFF];
        }
        crc ^= xorout;
    } else {
        /* 正向模式 */
        for (size_t i = 0; i < datalen; i++) {
            crc = (crc << 8) ^ crc16_table[((crc >> 8) ^ data[i]) & 0xFF];
        }
    }

    return crc;
}

uint32_t crypto_crc32(const uint8_t* data, size_t datalen, uint32_t init, uint32_t xorout, int rev) {
    uint32_t crc = init;

    if (rev) {
        /* 反转模式 (标准 CRC32) */
        for (size_t i = 0; i < datalen; i++) {
            crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
        }
        crc ^= xorout;
    } else {
        /* 正向模式 (MPEG2) */
        for (size_t i = 0; i < datalen; i++) {
            crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ data[i]) & 0xFF];
        }
    }

    return crc;
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
