/**
 * @file lua_module.c
 * @brief GmSSL 库的 Lua 模块封装
 *
 * 提供国密算法的 Lua 接口，包括 SM2、SM3、SM4 等
 */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* Lua 头文件 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* GmSSL 头文件 */
#include "gmssl/sm2.h"
#include "gmssl/sm3.h"
#include "gmssl/sm4.h"
#include "gmssl/hmac.h"
#include "gmssl/digest.h"
#include "gmssl/rand.h"

/*===========================================================
 * 常量定义
 *===========================================================*/

#define SM3_DIGEST_SIZE    32
#define SM4_KEY_SIZE       16
#define SM4_BLOCK_SIZE     16

/*===========================================================
 * 辅助函数
 *===========================================================*/

/* 将二进制数据转为十六进制字符串 */
static void bin_to_hex(const uint8_t* bin, size_t len, char* hex) {
    for (size_t i = 0; i < len; i++) {
        sprintf(hex + i * 2, "%02x", bin[i]);
    }
    hex[len * 2] = '\0';
}

/* 将十六进制字符串转为二进制数据 */
static int hex_to_bin(const char* hex, uint8_t* bin, size_t max_len) {
    size_t hex_len = strlen(hex);
    size_t bin_len = hex_len / 2;

    if (bin_len > max_len) {
        return -1;
    }

    for (size_t i = 0; i < bin_len; i++) {
        unsigned int val;
        if (sscanf(hex + i * 2, "%2x", &val) != 1) {
            return -1;
        }
        bin[i] = (uint8_t)val;
    }

    return (int)bin_len;
}

/*===========================================================
 * SM3 哈希算法
 *===========================================================*/

/**
 * @brief SM3 单次哈希计算
 * @api gmssl.sm3(data)
 * @string data 待哈希的数据
 * @return 十六进制哈希值字符串
 */
static int luaopen_gmssl_sm3(lua_State* L) {
    size_t data_len;
    const uint8_t* data = (const uint8_t*)luaL_checklstring(L, 1, &data_len);

    uint8_t digest[SM3_DIGEST_SIZE];
    SM3_CTX ctx;

    sm3_init(&ctx);
    sm3_update(&ctx, data, data_len);
    sm3_finish(&ctx, digest);

    char hex[SM3_DIGEST_SIZE * 2 + 1];
    bin_to_hex(digest, SM3_DIGEST_SIZE, hex);

    lua_pushstring(L, hex);
    return 1;
}

/* SM3 上下文元表名称 */
#define SM3_CTX_METATABLE "gmssl.sm3_ctx"

/**
 * @brief 创建 SM3 哈希上下文
 * @api gmssl.sm3_new()
 * @return SM3 上下文对象
 */
static int luaopen_gmssl_sm3_new(lua_State* L) {
    SM3_CTX* ctx = (SM3_CTX*)lua_newuserdata(L, sizeof(SM3_CTX));
    sm3_init(ctx);

    luaL_getmetatable(L, SM3_CTX_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 更新 SM3 哈希数据
 * @api ctx:update(data)
 * @string data 数据
 */
static int luaopen_gmssl_sm3_ctx_update(lua_State* L) {
    SM3_CTX* ctx = (SM3_CTX*)luaL_checkudata(L, 1, SM3_CTX_METATABLE);
    size_t data_len;
    const uint8_t* data = (const uint8_t*)luaL_checklstring(L, 2, &data_len);

    sm3_update(ctx, data, data_len);

    return 0;
}

/**
 * @brief 完成 SM3 哈希计算
 * @api ctx:finish()
 * @return 十六进制哈希值字符串
 */
static int luaopen_gmssl_sm3_ctx_finish(lua_State* L) {
    SM3_CTX* ctx = (SM3_CTX*)luaL_checkudata(L, 1, SM3_CTX_METATABLE);

    uint8_t digest[SM3_DIGEST_SIZE];
    sm3_finish(ctx, digest);

    char hex[SM3_DIGEST_SIZE * 2 + 1];
    bin_to_hex(digest, SM3_DIGEST_SIZE, hex);

    lua_pushstring(L, hex);
    return 1;
}

/*===========================================================
 * SM3 HMAC
 *===========================================================*/

/**
 * @brief SM3 HMAC 计算
 * @api gmssl.sm3_hmac(key, data)
 * @string key 密钥
 * @string data 数据
 * @return 十六进制 HMAC 值字符串
 */
static int luaopen_gmssl_sm3_hmac(lua_State* L) {
    size_t key_len, data_len;
    const uint8_t* key = (const uint8_t*)luaL_checklstring(L, 1, &key_len);
    const uint8_t* data = (const uint8_t*)luaL_checklstring(L, 2, &data_len);

    uint8_t mac[SM3_HMAC_SIZE];
    SM3_HMAC_CTX ctx;

    sm3_hmac_init(&ctx, key, key_len);
    sm3_hmac_update(&ctx, data, data_len);
    sm3_hmac_finish(&ctx, mac);

    char hex[SM3_HMAC_SIZE * 2 + 1];
    bin_to_hex(mac, SM3_HMAC_SIZE, hex);

    lua_pushstring(L, hex);
    return 1;
}

/*===========================================================
 * SM4 对称加密
 *===========================================================*/

/* SM4 上下文元表名称 */
#define SM4_CBC_CTX_METATABLE "gmssl.sm4_cbc_ctx"

/**
 * @brief SM4 CBC 加密
 * @api gmssl.sm4_cbc_encrypt(key, iv, data)
 * @string key 16字节密钥（十六进制）
 * @string iv 16字节IV（十六进制）
 * @string data 待加密数据
 * @return 加密后的数据（十六进制）
 */
static int luaopen_gmssl_sm4_cbc_encrypt(lua_State* L) {
    const char* key_hex = luaL_checkstring(L, 1);
    const char* iv_hex = luaL_checkstring(L, 2);
    size_t data_len;
    const uint8_t* data = (const uint8_t*)luaL_checklstring(L, 3, &data_len);

    uint8_t key[SM4_KEY_SIZE];
    uint8_t iv[SM4_BLOCK_SIZE];

    if (hex_to_bin(key_hex, key, SM4_KEY_SIZE) != SM4_KEY_SIZE) {
        luaL_error(L, "invalid key length");
    }
    if (hex_to_bin(iv_hex, iv, SM4_BLOCK_SIZE) != SM4_BLOCK_SIZE) {
        luaL_error(L, "invalid iv length");
    }

    /* 计算输出长度（包含填充） */
    size_t out_len = data_len + SM4_BLOCK_SIZE - (data_len % SM4_BLOCK_SIZE);
    uint8_t* out = (uint8_t*)malloc(out_len);
    if (!out) {
        luaL_error(L, "memory allocation failed");
    }

    SM4_KEY sm4_key;
    sm4_set_encrypt_key(&sm4_key, key);

    size_t actual_out_len;
    if (sm4_cbc_padding_encrypt(&sm4_key, iv, data, data_len, out, &actual_out_len) != 1) {
        free(out);
        luaL_error(L, "encryption failed");
    }

    char* hex_out = (char*)malloc(actual_out_len * 2 + 1);
    if (!hex_out) {
        free(out);
        luaL_error(L, "memory allocation failed");
    }
    bin_to_hex(out, actual_out_len, hex_out);

    lua_pushstring(L, hex_out);

    free(out);
    free(hex_out);

    return 1;
}

/**
 * @brief SM4 CBC 解密
 * @api gmssl.sm4_cbc_decrypt(key, iv, data)
 * @string key 16字节密钥（十六进制）
 * @string iv 16字节IV（十六进制）
 * @string data 待解密数据（十六进制）
 * @return 解密后的数据
 */
static int luaopen_gmssl_sm4_cbc_decrypt(lua_State* L) {
    const char* key_hex = luaL_checkstring(L, 1);
    const char* iv_hex = luaL_checkstring(L, 2);
    const char* data_hex = luaL_checkstring(L, 3);

    uint8_t key[SM4_KEY_SIZE];
    uint8_t iv[SM4_BLOCK_SIZE];

    if (hex_to_bin(key_hex, key, SM4_KEY_SIZE) != SM4_KEY_SIZE) {
        luaL_error(L, "invalid key length");
    }
    if (hex_to_bin(iv_hex, iv, SM4_BLOCK_SIZE) != SM4_BLOCK_SIZE) {
        luaL_error(L, "invalid iv length");
    }

    size_t data_hex_len = strlen(data_hex);
    size_t data_len = data_hex_len / 2;
    uint8_t* data = (uint8_t*)malloc(data_len);
    if (!data) {
        luaL_error(L, "memory allocation failed");
    }

    if (hex_to_bin(data_hex, data, data_len) != (int)data_len) {
        free(data);
        luaL_error(L, "invalid data format");
    }

    uint8_t* out = (uint8_t*)malloc(data_len);
    if (!out) {
        free(data);
        luaL_error(L, "memory allocation failed");
    }

    SM4_KEY sm4_key;
    sm4_set_decrypt_key(&sm4_key, key);

    size_t actual_out_len;
    if (sm4_cbc_padding_decrypt(&sm4_key, iv, data, data_len, out, &actual_out_len) != 1) {
        free(data);
        free(out);
        luaL_error(L, "decryption failed");
    }

    lua_pushlstring(L, (const char*)out, actual_out_len);

    free(data);
    free(out);

    return 1;
}

/**
 * @brief SM4 ECB 加密单块
 * @api gmssl.sm4_ecb_encrypt(key, data)
 * @string key 16字节密钥（十六进制）
 * @string data 16字节数据（十六进制）
 * @return 加密后的数据（十六进制）
 */
static int luaopen_gmssl_sm4_ecb_encrypt(lua_State* L) {
    const char* key_hex = luaL_checkstring(L, 1);
    const char* data_hex = luaL_checkstring(L, 2);

    uint8_t key[SM4_KEY_SIZE];
    uint8_t data[SM4_BLOCK_SIZE];
    uint8_t out[SM4_BLOCK_SIZE];

    if (hex_to_bin(key_hex, key, SM4_KEY_SIZE) != SM4_KEY_SIZE) {
        luaL_error(L, "invalid key length");
    }
    if (hex_to_bin(data_hex, data, SM4_BLOCK_SIZE) != SM4_BLOCK_SIZE) {
        luaL_error(L, "invalid data length");
    }

    SM4_KEY sm4_key;
    sm4_set_encrypt_key(&sm4_key, key);
    sm4_encrypt(&sm4_key, data, out);

    char hex[SM4_BLOCK_SIZE * 2 + 1];
    bin_to_hex(out, SM4_BLOCK_SIZE, hex);

    lua_pushstring(L, hex);
    return 1;
}

/**
 * @brief SM4 ECB 解密单块
 * @api gmssl.sm4_ecb_decrypt(key, data)
 * @string key 16字节密钥（十六进制）
 * @string data 16字节数据（十六进制）
 * @return 解密后的数据（十六进制）
 */
static int luaopen_gmssl_sm4_ecb_decrypt(lua_State* L) {
    const char* key_hex = luaL_checkstring(L, 1);
    const char* data_hex = luaL_checkstring(L, 2);

    uint8_t key[SM4_KEY_SIZE];
    uint8_t data[SM4_BLOCK_SIZE];
    uint8_t out[SM4_BLOCK_SIZE];

    if (hex_to_bin(key_hex, key, SM4_KEY_SIZE) != SM4_KEY_SIZE) {
        luaL_error(L, "invalid key length");
    }
    if (hex_to_bin(data_hex, data, SM4_BLOCK_SIZE) != SM4_BLOCK_SIZE) {
        luaL_error(L, "invalid data length");
    }

    SM4_KEY sm4_key;
    sm4_set_decrypt_key(&sm4_key, key);
    sm4_encrypt(&sm4_key, data, out);

    char hex[SM4_BLOCK_SIZE * 2 + 1];
    bin_to_hex(out, SM4_BLOCK_SIZE, hex);

    lua_pushstring(L, hex);
    return 1;
}

/*===========================================================
 * SM2 非对称加密
 *===========================================================*/

/* SM2 密钥元表名称 */
#define SM2_KEY_METATABLE "gmssl.sm2_key"

/**
 * @brief 生成 SM2 密钥对
 * @api gmssl.sm2_key_generate()
 * @return SM2 密钥对象
 */
static int luaopen_gmssl_sm2_key_generate(lua_State* L) {
    SM2_KEY* key = (SM2_KEY*)lua_newuserdata(L, sizeof(SM2_KEY));

    if (sm2_key_generate(key) != 1) {
        luaL_error(L, "key generation failed");
    }

    luaL_getmetatable(L, SM2_KEY_METATABLE);
    lua_setmetatable(L, -2);

    return 1;
}

/**
 * @brief 获取 SM2 公钥
 * @api key:get_public_key()
 * @return 十六进制公钥字符串
 */
static int luaopen_gmssl_sm2_key_get_public_key(lua_State* L) {
    SM2_KEY* key = (SM2_KEY*)luaL_checkudata(L, 1, SM2_KEY_METATABLE);

    uint8_t pub_key[SM2_PUBLIC_KEY_SIZE];
    uint8_t* out = pub_key;
    size_t outlen = SM2_PUBLIC_KEY_SIZE;

    if (sm2_public_key_to_der(key, &out, &outlen) != 1) {
        luaL_error(L, "get public key failed");
    }

    char hex[SM2_PUBLIC_KEY_SIZE * 2 + 1];
    bin_to_hex(pub_key, SM2_PUBLIC_KEY_SIZE, hex);

    lua_pushstring(L, hex);
    return 1;
}

/**
 * @brief 获取 SM2 私钥
 * @api key:get_private_key()
 * @return 十六进制私钥字符串
 */
static int luaopen_gmssl_sm2_key_get_private_key(lua_State* L) {
    SM2_KEY* key = (SM2_KEY*)luaL_checkudata(L, 1, SM2_KEY_METATABLE);

    uint8_t priv_key[SM2_PRIVATE_KEY_DEFAULT_SIZE];
    uint8_t* out = priv_key;
    size_t outlen = SM2_PRIVATE_KEY_DEFAULT_SIZE;

    if (sm2_private_key_to_der(key, &out, &outlen) != 1) {
        luaL_error(L, "get private key failed");
    }

    char* hex = (char*)malloc(outlen * 2 + 1);
    if (!hex) {
        luaL_error(L, "memory allocation failed");
    }
    bin_to_hex(priv_key, outlen, hex);

    lua_pushstring(L, hex);
    free(hex);

    return 1;
}

/**
 * @brief SM2 垃圾回收
 */
static int luaopen_gmssl_sm2_key_gc(lua_State* L) {
    /* SM2_KEY 结构体没有需要特别释放的资源 */
    return 0;
}

/*===========================================================
 * 随机数生成
 *===========================================================*/

/**
 * @brief 生成随机字节
 * @api gmssl.rand_bytes(len)
 * @int len 字节数
 * @return 十六进制随机字节字符串
 */
static int luaopen_gmssl_rand_bytes(lua_State* L) {
    int len = (int)luaL_checkinteger(L, 1);
    if (len <= 0 || len > 1024) {
        luaL_error(L, "invalid length");
    }

    uint8_t* bytes = (uint8_t*)malloc(len);
    if (!bytes) {
        luaL_error(L, "memory allocation failed");
    }

    if (rand_bytes(bytes, len) != 1) {
        free(bytes);
        luaL_error(L, "random generation failed");
    }

    char* hex = (char*)malloc(len * 2 + 1);
    if (!hex) {
        free(bytes);
        luaL_error(L, "memory allocation failed");
    }
    bin_to_hex(bytes, len, hex);

    lua_pushstring(L, hex);

    free(bytes);
    free(hex);

    return 1;
}

/*===========================================================
 * 模块注册
 *===========================================================*/

/* SM3 上下文方法表 */
static const luaL_Reg sm3_ctx_methods[] = {
    { "update",  luaopen_gmssl_sm3_ctx_update },
    { "finish",  luaopen_gmssl_sm3_ctx_finish },
    { NULL,      NULL }
};

/* SM2 密钥方法表 */
static const luaL_Reg sm2_key_methods[] = {
    { "get_public_key",  luaopen_gmssl_sm2_key_get_public_key },
    { "get_private_key", luaopen_gmssl_sm2_key_get_private_key },
    { "__gc",            luaopen_gmssl_sm2_key_gc },
    { NULL,              NULL }
};

/* gmssl 模块方法表 */
static const luaL_Reg gmssl_module_methods[] = {
    /* SM3 哈希 */
    { "sm3",           luaopen_gmssl_sm3 },
    { "sm3_new",       luaopen_gmssl_sm3_new },
    { "sm3_hmac",      luaopen_gmssl_sm3_hmac },

    /* SM4 对称加密 */
    { "sm4_cbc_encrypt", luaopen_gmssl_sm4_cbc_encrypt },
    { "sm4_cbc_decrypt", luaopen_gmssl_sm4_cbc_decrypt },
    { "sm4_ecb_encrypt", luaopen_gmssl_sm4_ecb_encrypt },
    { "sm4_ecb_decrypt", luaopen_gmssl_sm4_ecb_decrypt },

    /* SM2 非对称加密 */
    { "sm2_key_generate", luaopen_gmssl_sm2_key_generate },

    /* 随机数 */
    { "rand_bytes",    luaopen_gmssl_rand_bytes },

    { NULL,            NULL }
};

/* 模块初始化 */
LUAMOD_API int luaopen_gmssl_register(lua_State* L) {
    /* 创建模块 */
    luaL_newlib(L, gmssl_module_methods);

    /* 创建 SM3 上下文元表 */
    luaL_newmetatable(L, SM3_CTX_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, sm3_ctx_methods, 0);
    lua_pop(L, 1);

    /* 创建 SM2 密钥元表 */
    luaL_newmetatable(L, SM2_KEY_METATABLE);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, sm2_key_methods, 0);
    lua_pop(L, 1);

    /* 注册常量 */
    lua_pushinteger(L, SM3_DIGEST_SIZE);
    lua_setfield(L, -2, "SM3_DIGEST_SIZE");

    lua_pushinteger(L, SM4_KEY_SIZE);
    lua_setfield(L, -2, "SM4_KEY_SIZE");

    lua_pushinteger(L, SM4_BLOCK_SIZE);
    lua_setfield(L, -2, "SM4_BLOCK_SIZE");

    return 1;
}