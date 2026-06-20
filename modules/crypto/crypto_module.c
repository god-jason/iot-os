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
#include "iot_mem.h"

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
static int crypto_hash(lua_State* L) {
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
static int crypto_hmac(lua_State* L) {
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
static int crypto_encrypt(lua_State* L) {
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
static int crypto_decrypt(lua_State* L) {
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
static int crypto_rand(lua_State* L) {
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
static int crypto_hex2bin(lua_State* L) {
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
static int crypto_bin2hex(lua_State* L) {
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
 * 模块注册
 *===========================================================*/

/* crypto 模块方法表 */
static const luaL_Reg crypto_methods[] = {
    { "hash",    crypto_hash },
    { "hmac",    crypto_hmac },
    { "encrypt", crypto_encrypt },
    { "decrypt", crypto_decrypt },
    { "rand",    crypto_rand },
    { "hex2bin", crypto_hex2bin },
    { "bin2hex", crypto_bin2hex },
    { NULL,      NULL }
};

/* 模块初始化 */
LUAMOD_API int luaopen_crypto(lua_State* L) {
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

    return 1;
}
