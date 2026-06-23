/**
 * @file crypto_module.c
 * @brief 加密库的 Lua 模块封装
 *
 * 提供常用加密算法的 Lua 接口
 */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Lua 头文件 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* IoT 核心头文件 */
#include "iot_log.h"
#include "platform.h"

/* 加密库头文件 */
#include "crypto.h"

/*===========================================================
 * 辅助函数
 *===========================================================*/

/* 将十六进制字符串转换为字节数组 */
static int hex_to_bin(lua_State* L, const char* hex, size_t hexlen, uint8_t** out, size_t* outlen) {
    if (hexlen % 2 != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "hex string must have even length");
        return 2;
    }

    *outlen = hexlen / 2;
    *out = (uint8_t*)iot_malloc(*outlen);
    if (!*out) {
        lua_pushnil(L);
        lua_pushstring(L, "out of memory");
        return 2;
    }

    for (size_t i = 0; i < *outlen; i++) {
        char hi = hex[i * 2];
        char lo = hex[i * 2 + 1];
        int h = 0, l = 0;

        if (hi >= '0' && hi <= '9') h = hi - '0';
        else if (hi >= 'a' && hi <= 'f') h = hi - 'a' + 10;
        else if (hi >= 'A' && hi <= 'F') h = hi - 'A' + 10;
        else {
            iot_free(*out);
            lua_pushnil(L);
            lua_pushstring(L, "invalid hex character");
            return 2;
        }

        if (lo >= '0' && lo <= '9') l = lo - '0';
        else if (lo >= 'a' && lo <= 'f') l = lo - 'a' + 10;
        else if (lo >= 'A' && lo <= 'F') l = lo - 'A' + 10;
        else {
            iot_free(*out);
            lua_pushnil(L);
            lua_pushstring(L, "invalid hex character");
            return 2;
        }

        (*out)[i] = (h << 4) | l;
    }

    return 0;
}

/* 将字节数组转换为十六进制字符串 */
static int bin_to_hex(lua_State* L, const uint8_t* bin, size_t binlen, char** out) {
    *out = (char*)iot_malloc(binlen * 2 + 1);
    if (!*out) {
        lua_pushnil(L);
        lua_pushstring(L, "out of memory");
        return 2;
    }

    for (size_t i = 0; i < binlen; i++) {
        snprintf(*out + i * 2, 3, "%02x", bin[i]);
    }
    (*out)[binlen * 2] = '\0';

    return 0;
}

/*===========================================================
 * 哈希函数
 *===========================================================*/

/**
 * @brief 计算哈希值
 * @api crypto.hash(algo, data, is_hex)
 * @string algo 算法名称: "md5", "sha1", "sha256", "sm3"
 * @string data 输入数据或十六进制字符串
 * @bool is_hex 输入是否为十六进制，默认为 false
 * @return 哈希值的十六进制字符串，失败返回 nil
 */
static int luaopen_crypto_hash(lua_State* L) {
    const char* algo = luaL_checkstring(L, 1);
    size_t datalen;
    const char* data = luaL_checklstring(L, 2, &datalen);
    int is_hex = lua_isnoneornil(L, 3) ? 0 : lua_toboolean(L, 3);

    uint8_t* input = NULL;
    size_t inputlen = datalen;

    /* 如果是十六进制输入，先转换 */
    if (is_hex) {
        if (hex_to_bin(L, data, datalen, &input, &inputlen) != 0) {
            return 2;
        }
        data = (const char*)input;
    }

    crypto_hash_type_t type;
    if (strcmp(algo, "md5") == 0) {
        type = CRYPTO_HASH_MD5;
    } else if (strcmp(algo, "sha1") == 0 || strcmp(algo, "sha") == 0) {
        type = CRYPTO_HASH_SHA1;
    } else if (strcmp(algo, "sha256") == 0) {
        type = CRYPTO_HASH_SHA256;
    } else if (strcmp(algo, "sha224") == 0) {
        type = CRYPTO_HASH_SHA224;
    } else if (strcmp(algo, "sha384") == 0) {
        type = CRYPTO_HASH_SHA384;
    } else if (strcmp(algo, "sha512") == 0) {
        type = CRYPTO_HASH_SHA512;
    } else if (strcmp(algo, "sm3") == 0) {
        type = CRYPTO_HASH_SM3;
    } else {
        if (input) iot_free(input);
        lua_pushnil(L);
        lua_pushstring(L, "unsupported hash algorithm");
        return 2;
    }

    uint8_t digest[64];
    size_t digestlen = sizeof(digest);

    int ret = crypto_hash(type, (const uint8_t*)data, inputlen, digest, &digestlen);
    if (input) iot_free(input);

    if (ret != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "hash failed");
        return 2;
    }

    char* hexout;
    if (bin_to_hex(L, digest, digestlen, &hexout) != 0) {
        return 2;
    }

    lua_pushstring(L, hexout);
    iot_free(hexout);

    return 1;
}

/*===========================================================
 * HMAC 函数
 *===========================================================*/

/**
 * @brief 计算 HMAC
 * @api crypto.hmac(algo, key, data, is_hex)
 * @string algo 算法名称: "md5", "sha1", "sha256", "sm3"
 * @string key 密钥或十六进制字符串
 * @string data 输入数据或十六进制字符串
 * @bool is_hex 密钥和数据是否为十六进制，默认为 false
 * @return HMAC 值的十六进制字符串，失败返回 nil
 */
static int luaopen_crypto_hmac(lua_State* L) {
    const char* algo = luaL_checkstring(L, 1);
    size_t keylen;
    const char* key = luaL_checklstring(L, 2, &keylen);
    size_t datalen;
    const char* data = luaL_checklstring(L, 3, &datalen);
    int is_hex = lua_isnoneornil(L, 4) ? 0 : lua_toboolean(L, 4);

    uint8_t* keybin = NULL;
    uint8_t* databin = NULL;
    size_t keybinlen = keylen;
    size_t databinlen = datalen;

    if (is_hex) {
        if (hex_to_bin(L, key, keylen, &keybin, &keybinlen) != 0) {
            return 2;
        }
        key = (const char*)keybin;
        if (hex_to_bin(L, data, datalen, &databin, &databinlen) != 0) {
            iot_free(keybin);
            return 2;
        }
        data = (const char*)databin;
    }

    crypto_hash_type_t type;
    if (strcmp(algo, "md5") == 0) {
        type = CRYPTO_HASH_MD5;
    } else if (strcmp(algo, "sha1") == 0 || strcmp(algo, "sha") == 0) {
        type = CRYPTO_HASH_SHA1;
    } else if (strcmp(algo, "sha256") == 0) {
        type = CRYPTO_HASH_SHA256;
    } else if (strcmp(algo, "sm3") == 0) {
        type = CRYPTO_HASH_SM3;
    } else {
        if (keybin) iot_free(keybin);
        if (databin) iot_free(databin);
        lua_pushnil(L);
        lua_pushstring(L, "unsupported hash algorithm");
        return 2;
    }

    uint8_t mac[64];
    size_t maclen = sizeof(mac);

    int ret = crypto_hmac(type, (const uint8_t*)key, keybinlen,
                          (const uint8_t*)data, databinlen, mac, &maclen);

    if (keybin) iot_free(keybin);
    if (databin) iot_free(databin);

    if (ret != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "hmac failed");
        return 2;
    }

    char* hexout;
    if (bin_to_hex(L, mac, maclen, &hexout) != 0) {
        return 2;
    }

    lua_pushstring(L, hexout);
    iot_free(hexout);

    return 1;
}

/*===========================================================
 * 对称加密函数
 *===========================================================*/

/**
 * @brief 对称加密
 * @api crypto.encrypt(algo, key, iv, data, is_hex)
 * @string algo 算法名称: "aes128-ecb", "aes128-cbc", "aes128-ctr", "aes128-gcm",
 *             "aes192-ecb", "aes192-cbc", "aes192-ctr", "aes192-gcm",
 *             "aes256-ecb", "aes256-cbc", "aes256-ctr", "aes256-gcm",
 *             "sm4-ecb", "sm4-cbc", "sm4-ctr", "sm4-gcm"
 * @string key 密钥或十六进制字符串
 * @string iv IV 或 nil (ECB 模式)
 * @string data 明文数据或十六进制字符串
 * @bool is_hex 密钥和明文是否为十六进制，默认为 false
 * @return 密文(十六进制)，失败返回 nil
 */
static int luaopen_crypto_encrypt(lua_State* L) {
    const char* algo = luaL_checkstring(L, 1);
    size_t keylen;
    const char* key = luaL_checklstring(L, 2, &keylen);
    const char* iv = lua_isnoneornil(L, 3) ? NULL : luaL_checklstring(L, 3, NULL);
    size_t ivlen = iv ? lua_rawlen(L, 3) : 0;
    size_t datalen;
    const char* data = luaL_checklstring(L, 4, &datalen);
    int is_hex = lua_isnoneornil(L, 5) ? 0 : lua_toboolean(L, 5);

    /* 解析算法类型 */
    crypto_cipher_type_t type;
    if (strcmp(algo, "aes128-ecb") == 0) type = CRYPTO_CIPHER_AES_128_ECB;
    else if (strcmp(algo, "aes128-cbc") == 0) type = CRYPTO_CIPHER_AES_128_CBC;
    else if (strcmp(algo, "aes128-ctr") == 0) type = CRYPTO_CIPHER_AES_128_CTR;
    else if (strcmp(algo, "aes128-gcm") == 0) type = CRYPTO_CIPHER_AES_128_GCM;
    else if (strcmp(algo, "aes192-ecb") == 0) type = CRYPTO_CIPHER_AES_192_ECB;
    else if (strcmp(algo, "aes192-cbc") == 0) type = CRYPTO_CIPHER_AES_192_CBC;
    else if (strcmp(algo, "aes192-ctr") == 0) type = CRYPTO_CIPHER_AES_192_CTR;
    else if (strcmp(algo, "aes192-gcm") == 0) type = CRYPTO_CIPHER_AES_192_GCM;
    else if (strcmp(algo, "aes256-ecb") == 0) type = CRYPTO_CIPHER_AES_256_ECB;
    else if (strcmp(algo, "aes256-cbc") == 0) type = CRYPTO_CIPHER_AES_256_CBC;
    else if (strcmp(algo, "aes256-ctr") == 0) type = CRYPTO_CIPHER_AES_256_CTR;
    else if (strcmp(algo, "aes256-gcm") == 0) type = CRYPTO_CIPHER_AES_256_GCM;
    else if (strcmp(algo, "sm4-ecb") == 0) type = CRYPTO_CIPHER_SM4_ECB;
    else if (strcmp(algo, "sm4-cbc") == 0) type = CRYPTO_CIPHER_SM4_CBC;
    else if (strcmp(algo, "sm4-ctr") == 0) type = CRYPTO_CIPHER_SM4_CTR;
    else if (strcmp(algo, "sm4-gcm") == 0) type = CRYPTO_CIPHER_SM4_GCM;
    else {
        lua_pushnil(L);
        lua_pushstring(L, "unsupported cipher algorithm");
        return 2;
    }

    uint8_t* keybin = NULL;
    uint8_t* ivbin = NULL;
    uint8_t* databin = NULL;
    size_t keybinlen = keylen;
    size_t ivbinlen = ivlen;
    size_t databinlen = datalen;

    if (is_hex) {
        if (hex_to_bin(L, key, keylen, &keybin, &keybinlen) != 0) {
            return 2;
        }
        key = (const char*)keybin;
        if (iv) {
            if (hex_to_bin(L, iv, ivlen, &ivbin, &ivbinlen) != 0) {
                iot_free(keybin);
                return 2;
            }
            iv = (const char*)ivbin;
        }
        if (hex_to_bin(L, data, datalen, &databin, &databinlen) != 0) {
            iot_free(keybin);
            if (ivbin) iot_free(ivbin);
            return 2;
        }
        data = (const char*)databin;
    }

    /* 检查 IV */
    size_t expected_ivlen = crypto_cipher_iv_size(type);
    if (expected_ivlen > 0 && ivbinlen != expected_ivlen) {
        if (keybin) iot_free(keybin);
        if (ivbin) iot_free(ivbin);
        if (databin) iot_free(databin);
        lua_pushnil(L);
        lua_pushstring(L, "invalid IV length");
        return 2;
    }

    /* 分配输出缓冲区 */
    size_t outlen = databinlen + 64;  /* 留足空间 */
    uint8_t* out = (uint8_t*)iot_malloc(outlen);
    if (!out) {
        if (keybin) iot_free(keybin);
        if (ivbin) iot_free(ivbin);
        if (databin) iot_free(databin);
        lua_pushnil(L);
        lua_pushstring(L, "out of memory");
        return 2;
    }

    size_t outlen_real = outlen;
    int ret = crypto_encrypt(type, (const uint8_t*)key, keybinlen,
                             (const uint8_t*)iv, ivbinlen,
                             NULL, 0,  /* aad */
                             (const uint8_t*)data, databinlen,
                             out, &outlen_real);

    if (keybin) iot_free(keybin);
    if (ivbin) iot_free(ivbin);
    if (databin) iot_free(databin);

    if (ret != 0) {
        iot_free(out);
        lua_pushnil(L);
        lua_pushstring(L, "encrypt failed");
        return 2;
    }

    char* hexout;
    if (bin_to_hex(L, out, outlen_real, &hexout) != 0) {
        iot_free(out);
        return 2;
    }
    iot_free(out);

    lua_pushstring(L, hexout);
    iot_free(hexout);

    return 1;
}

/**
 * @brief 对称解密
 * @api crypto.decrypt(algo, key, iv, data, is_hex)
 * @string algo 算法名称
 * @string key 密钥或十六进制字符串
 * @string iv IV 或 nil
 * @string data 密文数据(十六进制)
 * @bool is_hex 密钥和密文是否为十六进制，默认为 true
 * @return 明文字符串，失败返回 nil
 */
static int luaopen_crypto_decrypt(lua_State* L) {
    const char* algo = luaL_checkstring(L, 1);
    size_t keylen;
    const char* key = luaL_checklstring(L, 2, &keylen);
    const char* iv = lua_isnoneornil(L, 3) ? NULL : luaL_checklstring(L, 3, NULL);
    size_t ivlen = iv ? lua_rawlen(L, 3) : 0;
    size_t datalen;
    const char* data = luaL_checklstring(L, 4, &datalen);
    int is_hex = lua_isnoneornil(L, 5) ? 1 : lua_toboolean(L, 5);  /* 默认 true */

    /* 解析算法类型 */
    crypto_cipher_type_t type;
    if (strcmp(algo, "aes128-ecb") == 0) type = CRYPTO_CIPHER_AES_128_ECB;
    else if (strcmp(algo, "aes128-cbc") == 0) type = CRYPTO_CIPHER_AES_128_CBC;
    else if (strcmp(algo, "aes128-ctr") == 0) type = CRYPTO_CIPHER_AES_128_CTR;
    else if (strcmp(algo, "aes128-gcm") == 0) type = CRYPTO_CIPHER_AES_128_GCM;
    else if (strcmp(algo, "aes192-ecb") == 0) type = CRYPTO_CIPHER_AES_192_ECB;
    else if (strcmp(algo, "aes192-cbc") == 0) type = CRYPTO_CIPHER_AES_192_CBC;
    else if (strcmp(algo, "aes192-ctr") == 0) type = CRYPTO_CIPHER_AES_192_CTR;
    else if (strcmp(algo, "aes192-gcm") == 0) type = CRYPTO_CIPHER_AES_192_GCM;
    else if (strcmp(algo, "aes256-ecb") == 0) type = CRYPTO_CIPHER_AES_256_ECB;
    else if (strcmp(algo, "aes256-cbc") == 0) type = CRYPTO_CIPHER_AES_256_CBC;
    else if (strcmp(algo, "aes256-ctr") == 0) type = CRYPTO_CIPHER_AES_256_CTR;
    else if (strcmp(algo, "aes256-gcm") == 0) type = CRYPTO_CIPHER_AES_256_GCM;
    else if (strcmp(algo, "sm4-ecb") == 0) type = CRYPTO_CIPHER_SM4_ECB;
    else if (strcmp(algo, "sm4-cbc") == 0) type = CRYPTO_CIPHER_SM4_CBC;
    else if (strcmp(algo, "sm4-ctr") == 0) type = CRYPTO_CIPHER_SM4_CTR;
    else if (strcmp(algo, "sm4-gcm") == 0) type = CRYPTO_CIPHER_SM4_GCM;
    else {
        lua_pushnil(L);
        lua_pushstring(L, "unsupported cipher algorithm");
        return 2;
    }

    uint8_t* keybin = NULL;
    uint8_t* ivbin = NULL;
    uint8_t* databin = NULL;
    size_t keybinlen = keylen;
    size_t ivbinlen = ivlen;
    size_t databinlen = datalen;

    if (hex_to_bin(L, key, keylen, &keybin, &keybinlen) != 0) {
        return 2;
    }
    key = (const char*)keybin;

    if (iv) {
        if (hex_to_bin(L, iv, ivlen, &ivbin, &ivbinlen) != 0) {
            iot_free(keybin);
            return 2;
        }
        iv = (const char*)ivbin;
    }

    if (is_hex) {
        if (hex_to_bin(L, data, datalen, &databin, &databinlen) != 0) {
            iot_free(keybin);
            if (ivbin) iot_free(ivbin);
            return 2;
        }
        data = (const char*)databin;
    }

    /* 检查 IV */
    size_t expected_ivlen = crypto_cipher_iv_size(type);
    if (expected_ivlen > 0 && ivbinlen != expected_ivlen) {
        if (keybin) iot_free(keybin);
        if (ivbin) iot_free(ivbin);
        if (databin) iot_free(databin);
        lua_pushnil(L);
        lua_pushstring(L, "invalid IV length");
        return 2;
    }

    /* 分配输出缓冲区 */
    size_t outlen = databinlen + 64;
    uint8_t* out = (uint8_t*)iot_malloc(outlen);
    if (!out) {
        if (keybin) iot_free(keybin);
        if (ivbin) iot_free(ivbin);
        if (databin) iot_free(databin);
        lua_pushnil(L);
        lua_pushstring(L, "out of memory");
        return 2;
    }

    size_t outlen_real = outlen;
    int ret = crypto_decrypt(type, (const uint8_t*)key, keybinlen,
                             (const uint8_t*)iv, ivbinlen,
                             NULL, 0,
                             (const uint8_t*)data, databinlen,
                             out, &outlen_real);

    if (keybin) iot_free(keybin);
    if (ivbin) iot_free(ivbin);
    if (databin) iot_free(databin);

    if (ret != 0) {
        iot_free(out);
        lua_pushnil(L);
        lua_pushstring(L, "decrypt failed");
        return 2;
    }

    lua_pushlstring(L, (const char*)out, outlen_real);
    iot_free(out);

    return 1;
}

/*===========================================================
 * 随机数和编码函数
 *===========================================================*/

/**
 * @brief 生成随机字节
 * @api crypto.rand(len)
 * @int len 需要的随机字节数
 * @return 随机字节的十六进制字符串，失败返回 nil
 */
static int luaopen_crypto_rand(lua_State* L) {
    int len = (int)luaL_checkinteger(L, 1);
    if (len <= 0 || len > 256) {
        lua_pushnil(L);
        lua_pushstring(L, "length must be 1-256");
        return 2;
    }

    uint8_t* buf = (uint8_t*)iot_malloc(len);
    if (!buf) {
        lua_pushnil(L);
        lua_pushstring(L, "out of memory");
        return 2;
    }

    if (crypto_rand_bytes(buf, len) != 0) {
        iot_free(buf);
        lua_pushnil(L);
        lua_pushstring(L, "rand failed");
        return 2;
    }

    char* hexout;
    if (bin_to_hex(L, buf, len, &hexout) != 0) {
        iot_free(buf);
        return 2;
    }
    iot_free(buf);

    lua_pushstring(L, hexout);
    iot_free(hexout);

    return 1;
}

/**
 * @brief 十六进制字符串转二进制
 * @api crypto.hex2bin(hex)
 * @string hex 十六进制字符串
 * @return 二进制数据的十六进制表示(不变)，用于验证
 */
static int luaopen_crypto_hex2bin(lua_State* L) {
    size_t hexlen;
    const char* hex = luaL_checklstring(L, 1, &hexlen);

    uint8_t* out = NULL;
    size_t outlen;

    if (hex_to_bin(L, hex, hexlen, &out, &outlen) != 0) {
        return 2;
    }

    lua_pushlstring(L, (const char*)out, outlen);
    iot_free(out);

    return 1;
}

/**
 * @brief 二进制转十六进制字符串
 * @api crypto.bin2hex(bin)
 * @string bin 二进制数据
 * @return 十六进制字符串
 */
static int luaopen_crypto_bin2hex(lua_State* L) {
    size_t binlen;
    const char* bin = luaL_checklstring(L, 1, &binlen);

    char* hexout;
    if (bin_to_hex(L, (const uint8_t*)bin, binlen, &hexout) != 0) {
        return 2;
    }

    lua_pushstring(L, hexout);
    iot_free(hexout);

    return 1;
}

/*===========================================================
 * 校验算法 (CRC, Adler32)
 *===========================================================*/

/**
 * @brief 计算数据校验和
 * @api crypto.checksum(type, data)
 * @string type 校验算法类型: "crc16", "crc32", "adler32"
 * @string data 输入数据
 * @return 校验值的十六进制字符串
 */
static int luaopen_crypto_checksum(lua_State* L) {
    const char* type = luaL_checkstring(L, 1);
    size_t datalen;
    const char* data = luaL_checklstring(L, 2, &datalen);

    crypto_checksum_type_t checksum_type;
    if (strcmp(type, "crc16") == 0) {
        checksum_type = CRYPTO_CHECKSUM_CRC16;
    } else if (strcmp(type, "crc32") == 0) {
        checksum_type = CRYPTO_CHECKSUM_CRC32;
    } else if (strcmp(type, "adler32") == 0) {
        checksum_type = CRYPTO_CHECKSUM_ADLER32;
    } else {
        lua_pushnil(L);
        lua_pushstring(L, "unsupported checksum type, use crc16/crc32/adler32");
        return 2;
    }

    uint32_t result = crypto_checksum(checksum_type, (const uint8_t*)data, datalen);

    /* 返回十六进制格式 */
    char hex[16];
    if (checksum_type == CRYPTO_CHECKSUM_CRC16) {
        snprintf(hex, sizeof(hex), "%04X", result);
    } else {
        snprintf(hex, sizeof(hex), "%08X", result);
    }

    lua_pushstring(L, hex);
    return 1;
}

/**
 * @brief 计算 CRC16
 * @api crypto.crc16(data, init, xorout)
 * @string data 输入数据
 * @int init 初始值，默认为 0xFFFF
 * @int xorout 输出异或值，默认为 0x0000
 * @return CRC16 值
 */
static int luaopen_crypto_crc16(lua_State* L) {
    size_t datalen;
    const char* data = luaL_checklstring(L, 1, &datalen);
    uint16_t init = (uint16_t)luaL_optinteger(L, 2, 0xFFFF);
    uint16_t xorout = (uint16_t)luaL_optinteger(L, 3, 0x0000);

    uint16_t result = crypto_crc16((const uint8_t*)data, datalen, init, xorout, 1);
    lua_pushinteger(L, result);
    return 1;
}

/**
 * @brief 计算 CRC32
 * @api crypto.crc32(data, init, xorout)
 * @string data 输入数据
 * @int init 初始值，默认为 0xFFFFFFFF
 * @int xorout 输出异或值，默认为 0x00000000
 * @return CRC32 值
 */
static int luaopen_crypto_crc32(lua_State* L) {
    size_t datalen;
    const char* data = luaL_checklstring(L, 1, &datalen);
    uint32_t init = (uint32_t)luaL_optinteger(L, 2, 0xFFFFFFFF);
    uint32_t xorout = (uint32_t)luaL_optinteger(L, 3, 0x00000000);

    uint32_t result = crypto_crc32((const uint8_t*)data, datalen, init, xorout, 1);
    lua_pushinteger(L, result);
    return 1;
}

/*===========================================================
 * Base64 编解码
 *===========================================================*/

/**
 * @brief Base64 编码
 * @api crypto.base64_encode(data)
 * @string data 输入数据
 * @return Base64 编码字符串
 */
static int luaopen_crypto_base64_encode(lua_State* L) {
    size_t datalen;
    const char* data = luaL_checklstring(L, 1, &datalen);

    size_t outlen = crypto_base64_encode_len(datalen);
    char* out = (char*)iot_malloc(outlen);
    if (!out) {
        lua_pushnil(L);
        lua_pushstring(L, "out of memory");
        return 2;
    }

    size_t reallen = outlen;
    int ret = crypto_base64_encode((const uint8_t*)data, datalen, out, &reallen);

    if (ret != 0) {
        iot_free(out);
        lua_pushnil(L);
        lua_pushstring(L, "base64 encode failed");
        return 2;
    }

    lua_pushlstring(L, out, reallen);
    iot_free(out);

    return 1;
}

/**
 * @brief Base64 解码
 * @api crypto.base64_decode(str)
 * @string str Base64 编码字符串
 * @return 解码后的二进制数据
 */
static int luaopen_crypto_base64_decode(lua_State* L) {
    size_t datalen;
    const char* data = luaL_checklstring(L, 1, &datalen);

    size_t outlen = crypto_base64_decode_len(datalen);
    uint8_t* out = (uint8_t*)iot_malloc(outlen);
    if (!out) {
        lua_pushnil(L);
        lua_pushstring(L, "out of memory");
        return 2;
    }

    size_t reallen = outlen;
    int ret = crypto_base64_decode(data, datalen, out, &reallen);

    if (ret != 0) {
        iot_free(out);
        lua_pushnil(L);
        lua_pushstring(L, "base64 decode failed");
        return 2;
    }

    lua_pushlstring(L, (const char*)out, reallen);
    iot_free(out);

    return 1;
}

/*===========================================================
 * X509 证书操作
 *===========================================================*/

/* X509 证书 userdata 元表 */
#define X509_CERT_METATABLE "crypto.x509_cert"

static int luaopen_crypto_x509_cert_gc(lua_State* L) {
    crypto_x509_cert_t** cert = (crypto_x509_cert_t**)luaL_checkudata(L, 1, X509_CERT_METATABLE);
    if (cert && *cert) {
        crypto_x509_free(*cert);
        *cert = NULL;
    }
    return 0;
}

static int luaopen_crypto_x509_cert_tostring(lua_State* L) {
    crypto_x509_cert_t** cert = (crypto_x509_cert_t**)luaL_checkudata(L, 1, X509_CERT_METATABLE);
    if (!cert || !*cert) {
        lua_pushstring(L, "x509_cert: invalid");
        return 1;
    }

    lua_pushfstring(L, "x509_cert: CN=%s", (*cert)->common_name ? (*cert)->common_name : "N/A");
    return 1;
}

/**
 * @brief 解析 PEM 格式 X509 证书
 * @api crypto.x509_parse_pem(pem)
 * @string pem PEM 格式证书
 * @return X509 证书对象，失败返回 nil
 */
static int luaopen_crypto_x509_parse_pem(lua_State* L) {
    size_t pemlen;
    const char* pem = luaL_checklstring(L, 1, &pemlen);

    crypto_x509_cert_t* cert = crypto_x509_parse_pem(pem, pemlen);
    if (!cert) {
        lua_pushnil(L);
        lua_pushstring(L, "parse x509 pem failed");
        return 2;
    }

    crypto_x509_cert_t** udata = (crypto_x509_cert_t**)lua_newuserdata(L, sizeof(crypto_x509_cert_t*));
    *udata = cert;

    if (luaL_newmetatable(L, X509_CERT_METATABLE)) {
        lua_pushcfunction(L, luaopen_crypto_x509_cert_gc);
        lua_setfield(L, -2, "__gc");
        lua_pushcfunction(L, luaopen_crypto_x509_cert_tostring);
        lua_setfield(L, -2, "__tostring");
        lua_pushvalue(L, -1);
        lua_setfield(L, -2, "__index");
    }
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 获取证书主题
 * @api cert:get_subject()
 * @return 主题字符串
 */
static int luaopen_crypto_x509_cert_get_subject(lua_State* L) {
    crypto_x509_cert_t** cert = (crypto_x509_cert_t**)luaL_checkudata(L, 1, X509_CERT_METATABLE);
    if (!cert || !*cert) {
        lua_pushnil(L);
        return 1;
    }

    char* subject = crypto_x509_get_subject(*cert);
    if (subject) {
        lua_pushstring(L, subject);
        free(subject);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取证书颁发者
 * @api cert:get_issuer()
 * @return 颁发者字符串
 */
static int luaopen_crypto_x509_cert_get_issuer(lua_State* L) {
    crypto_x509_cert_t** cert = (crypto_x509_cert_t**)luaL_checkudata(L, 1, X509_CERT_METATABLE);
    if (!cert || !*cert) {
        lua_pushnil(L);
        return 1;
    }

    char* issuer = crypto_x509_get_issuer(*cert);
    if (issuer) {
        lua_pushstring(L, issuer);
        free(issuer);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取证书序列号
 * @api cert:get_serial_number()
 * @return 序列号字符串
 */
static int luaopen_crypto_x509_cert_get_serial_number(lua_State* L) {
    crypto_x509_cert_t** cert = (crypto_x509_cert_t**)luaL_checkudata(L, 1, X509_CERT_METATABLE);
    if (!cert || !*cert) {
        lua_pushnil(L);
        return 1;
    }

    char* serial = crypto_x509_get_serial_number(*cert);
    if (serial) {
        lua_pushstring(L, serial);
        free(serial);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 获取证书通用名称
 * @api cert:get_common_name()
 * @return 通用名称字符串
 */
static int luaopen_crypto_x509_cert_get_common_name(lua_State* L) {
    crypto_x509_cert_t** cert = (crypto_x509_cert_t**)luaL_checkudata(L, 1, X509_CERT_METATABLE);
    if (!cert || !*cert) {
        lua_pushnil(L);
        return 1;
    }

    char* cn = crypto_x509_get_common_name(*cert);
    if (cn) {
        lua_pushstring(L, cn);
        free(cn);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/* X509 证书方法表 */
static const luaL_Reg x509_cert_methods[] = {
    { "get_subject",      luaopen_crypto_x509_cert_get_subject },
    { "get_issuer",       luaopen_crypto_x509_cert_get_issuer },
    { "get_serial_number",luaopen_crypto_x509_cert_get_serial_number },
    { "get_common_name",  luaopen_crypto_x509_cert_get_common_name },
    { NULL,               NULL }
};

/*===========================================================
 * PBKDF2 密钥派生
 *===========================================================*/

/**
 * @brief PBKDF2 密钥派生
 * @api crypto.pbkdf2_sha256(password, salt, iterations, keylen)
 * @string password 密码
 * @string salt 盐值
 * @int iterations 迭代次数
 * @int keylen 派生密钥长度
 * @return 派生的密钥(十六进制字符串)
 */
static int luaopen_crypto_pbkdf2_sha256(lua_State* L) {
    size_t passwordlen;
    const char* password = luaL_checklstring(L, 1, &passwordlen);
    size_t saltlen;
    const char* salt = luaL_checklstring(L, 2, &saltlen);
    int iterations = (int)luaL_checkinteger(L, 3);
    int keylen = (int)luaL_checkinteger(L, 4);

    if (iterations <= 0 || keylen <= 0 || keylen > 256) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid iterations or keylen");
        return 2;
    }

    uint8_t* key = (uint8_t*)iot_malloc(keylen);
    if (!key) {
        lua_pushnil(L);
        lua_pushstring(L, "out of memory");
        return 2;
    }

    int ret = crypto_pbkdf2_sha256((const uint8_t*)password, passwordlen,
                                   (const uint8_t*)salt, saltlen,
                                   iterations, keylen, key);

    if (ret != 0) {
        iot_free(key);
        lua_pushnil(L);
        lua_pushstring(L, "pbkdf2 failed");
        return 2;
    }

    char* hexout;
    if (bin_to_hex(L, key, keylen, &hexout) != 0) {
        iot_free(key);
        return 2;
    }
    iot_free(key);

    lua_pushstring(L, hexout);
    iot_free(hexout);

    return 1;
}

/*===========================================================
 * 模块注册
 *===========================================================*/

/* crypto 模块方法表 */
static const luaL_Reg crypto_methods[] = {
    { "hash",         luaopen_crypto_hash },
    { "hmac",         luaopen_crypto_hmac },
    { "encrypt",      luaopen_crypto_encrypt },
    { "decrypt",      luaopen_crypto_decrypt },
    { "rand",         luaopen_crypto_rand },
    { "hex2bin",      luaopen_crypto_hex2bin },
    { "bin2hex",      luaopen_crypto_bin2hex },
    { "checksum",     luaopen_crypto_checksum },
    { "crc16",        luaopen_crypto_crc16 },
    { "crc32",        luaopen_crypto_crc32 },
    { "base64_encode",luaopen_crypto_base64_encode },
    { "base64_decode",luaopen_crypto_base64_decode },
    { "x509_parse_pem",luaopen_crypto_x509_parse_pem },
    { "pbkdf2_sha256",luaopen_crypto_pbkdf2_sha256 },
    { NULL,           NULL }
};

LUAMOD_API int luaopen_crypto_register(lua_State* L) {
    luaL_newlib(L, crypto_methods);

    /* 注册算法名称常量 */
    /* 哈希算法 */
    lua_pushstring(L, "md5");    lua_setfield(L, -2, "MD5");
    lua_pushstring(L, "sha1");   lua_setfield(L, -2, "SHA1");
    lua_pushstring(L, "sha256"); lua_setfield(L, -2, "SHA256");
    lua_pushstring(L, "sm3");    lua_setfield(L, -2, "SM3");

    /* AES 算法 */
    lua_pushstring(L, "aes128-ecb"); lua_setfield(L, -2, "AES128_ECB");
    lua_pushstring(L, "aes128-cbc"); lua_setfield(L, -2, "AES128_CBC");
    lua_pushstring(L, "aes128-ctr"); lua_setfield(L, -2, "AES128_CTR");
    lua_pushstring(L, "aes128-gcm"); lua_setfield(L, -2, "AES128_GCM");
    lua_pushstring(L, "aes192-ecb"); lua_setfield(L, -2, "AES192_ECB");
    lua_pushstring(L, "aes192-cbc"); lua_setfield(L, -2, "AES192_CBC");
    lua_pushstring(L, "aes192-ctr"); lua_setfield(L, -2, "AES192_CTR");
    lua_pushstring(L, "aes192-gcm"); lua_setfield(L, -2, "AES192_GCM");
    lua_pushstring(L, "aes256-ecb"); lua_setfield(L, -2, "AES256_ECB");
    lua_pushstring(L, "aes256-cbc"); lua_setfield(L, -2, "AES256_CBC");
    lua_pushstring(L, "aes256-ctr"); lua_setfield(L, -2, "AES256_CTR");
    lua_pushstring(L, "aes256-gcm"); lua_setfield(L, -2, "AES256_GCM");

    /* SM4 算法 */
    lua_pushstring(L, "sm4-ecb"); lua_setfield(L, -2, "SM4_ECB");
    lua_pushstring(L, "sm4-cbc"); lua_setfield(L, -2, "SM4_CBC");
    lua_pushstring(L, "sm4-ctr"); lua_setfield(L, -2, "SM4_CTR");
    lua_pushstring(L, "sm4-gcm"); lua_setfield(L, -2, "SM4_GCM");

    /* 校验算法常量 */
    lua_pushstring(L, "crc16");   lua_setfield(L, -2, "CHECKSUM_CRC16");
    lua_pushstring(L, "crc32");   lua_setfield(L, -2, "CHECKSUM_CRC32");
    lua_pushstring(L, "adler32"); lua_setfield(L, -2, "CHECKSUM_ADLER32");

    /* 注册 X509 证书类型元表 */
    luaL_newmetatable(L, X509_CERT_METATABLE);
    lua_pushcfunction(L, luaopen_crypto_x509_cert_gc);
    lua_setfield(L, -2, "__gc");
    lua_pushcfunction(L, luaopen_crypto_x509_cert_tostring);
    lua_setfield(L, -2, "__tostring");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    /* 注册 X509 证书方法 */
    luaL_setfuncs(L, x509_cert_methods, 0);
    lua_pop(L, 1);  /* 弹出元表 */

    return 1;
}
