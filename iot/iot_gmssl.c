#include "lua.h"
#include "lauxlib.h"
#include "iot_base.h"
#include "iot_gmssl.h"
#include "iot_mem.h"

#include <gmssl/sm3.h>
#include <gmssl/sm4.h>
#include <gmssl/sm2.h>
#include <gmssl/rand.h>
#include <gmssl/hex.h>
#include <gmssl/base64.h>

/* SM3 哈希 */
static int iot_gmssl_sm3(lua_State* L) {
    size_t len;
    const char* data = luaL_checklstring(L, 1, &len);

    SM3_CTX ctx;
    uint8_t dgst[SM3_DIGEST_SIZE];

    sm3_init(&ctx);
    sm3_update(&ctx, (const uint8_t*)data, len);
    sm3_finish(&ctx, dgst);

    lua_pushlstring(L, (const char*)dgst, SM3_DIGEST_SIZE);
    return 1;
}

/* SM3-HMAC */
static int iot_gmssl_sm3_hmac(lua_State* L) {
    size_t keylen, datalen;
    const char* key = luaL_checklstring(L, 1, &keylen);
    const char* data = luaL_checklstring(L, 2, &datalen);

    SM3_HMAC_CTX ctx;
    uint8_t mac[SM3_HMAC_SIZE];

    sm3_hmac_init(&ctx, (const uint8_t*)key, keylen);
    sm3_hmac_update(&ctx, (const uint8_t*)data, datalen);
    sm3_hmac_finish(&ctx, mac);

    lua_pushlstring(L, (const char*)mac, SM3_HMAC_SIZE);
    return 1;
}

/* SM4 加密/解密 */
static int iot_gmssl_sm4_crypt(lua_State* L, int decrypt) {
    const char* mode_str = luaL_checkstring(L, 1);
    size_t keylen, ivlen, inlen;
    const char* key = luaL_checklstring(L, 2, &keylen);
    const char* iv = luaL_checklstring(L, 3, &ivlen);
    const char* in = luaL_checklstring(L, 4, &inlen);

    if (keylen != SM4_KEY_SIZE) {
        luaL_error(L, "SM4 key must be 16 bytes");
        return 0;
    }

    if (ivlen != SM4_BLOCK_SIZE && ivlen != 0) {
        luaL_error(L, "SM4 IV must be 16 bytes or empty");
        return 0;
    }

    SM4_KEY sm4_key;
    if (decrypt) {
        sm4_set_decrypt_key(&sm4_key, (const uint8_t*)key);
    } else {
        sm4_set_encrypt_key(&sm4_key, (const uint8_t*)key);
    }

    uint8_t iv_copy[SM4_BLOCK_SIZE] = {0};
    if (ivlen > 0) {
        memcpy(iv_copy, iv, ivlen);
    }

    size_t outlen = inlen + SM4_BLOCK_SIZE;
    uint8_t* out = (uint8_t*)iot_malloc(outlen);
    if (!out) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    int ret = -1;
    if (strcmp(mode_str, "cbc") == 0 || strcmp(mode_str, "CBC") == 0) {
        if (ivlen != SM4_BLOCK_SIZE) {
            iot_free(out);
            luaL_error(L, "CBC mode requires 16-byte IV");
            return 0;
        }
        if (decrypt) {
            ret = sm4_cbc_padding_decrypt(&sm4_key, iv_copy, (const uint8_t*)in, inlen, out, &outlen);
        } else {
            ret = sm4_cbc_padding_encrypt(&sm4_key, iv_copy, (const uint8_t*)in, inlen, out, &outlen);
        }
    } else if (strcmp(mode_str, "ecb") == 0 || strcmp(mode_str, "ECB") == 0) {
        if (inlen % SM4_BLOCK_SIZE != 0) {
            iot_free(out);
            luaL_error(L, "ECB mode requires block-aligned input");
            return 0;
        }
        sm4_encrypt_blocks(&sm4_key, (const uint8_t*)in, inlen / SM4_BLOCK_SIZE, out);
        ret = 0;
        outlen = inlen;
    } else if (strcmp(mode_str, "ctr") == 0 || strcmp(mode_str, "CTR") == 0) {
        if (ivlen != SM4_BLOCK_SIZE) {
            iot_free(out);
            luaL_error(L, "CTR mode requires 16-byte counter");
            return 0;
        }
        sm4_ctr_encrypt(&sm4_key, iv_copy, (const uint8_t*)in, inlen, out);
        ret = 0;
        outlen = inlen;
    } else {
        iot_free(out);
        luaL_error(L, "unsupported mode: %s", mode_str);
        return 0;
    }

    if (ret != 0) {
        iot_free(out);
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, (const char*)out, outlen);
    iot_free(out);
    return 1;
}

static int iot_gmssl_sm4_encrypt(lua_State* L) {
    return iot_gmssl_sm4_crypt(L, 0);
}

static int iot_gmssl_sm4_decrypt(lua_State* L) {
    return iot_gmssl_sm4_crypt(L, 1);
}

/* SM2 密钥生成 */
static int iot_gmssl_sm2_keygen(lua_State* L) {
    SM2_KEY key;

    if (sm2_key_generate(&key) != 1) {
        lua_pushnil(L);
        lua_pushstring(L, "SM2 key generation failed");
        return 2;
    }

    /* 获取公钥和私钥的DER编码 */
    uint8_t* p = NULL;
    size_t len = 0;

    /* 私钥 */
    sm2_private_key_to_der(&key, &p, &len);
    uint8_t* priv_der = (uint8_t*)iot_malloc(len);
    memcpy(priv_der, p, len);
    size_t priv_len = len;

    /* 公钥 */
    sm2_public_key_to_der(&key, &p, &len);
    uint8_t* pub_der = (uint8_t*)iot_malloc(len);
    memcpy(pub_der, p, len);
    size_t pub_len = len;

    /* 转换为PEM格式 */
    char priv_pem[512];
    char pub_pem[256];
    snprintf(priv_pem, sizeof(priv_pem), "-----BEGIN EC PRIVATE KEY-----\n%s-----END EC PRIVATE KEY-----\n",
             "");  /* 简化处理，直接使用DER编码的十六进制 */
    snprintf(pub_pem, sizeof(pub_pem), "-----BEGIN PUBLIC KEY-----\n%s-----END PUBLIC KEY-----\n",
             "");

    lua_newtable(L);

    /* 返回十六进制格式 */
    uint8_t priv_hex[128];
    uint8_t pub_hex[128];
    hex_encode(priv_der, priv_len, (char*)priv_hex);
    hex_encode(pub_der, pub_len, (char*)pub_hex);

    lua_pushstring(L, "private_key");
    lua_pushlstring(L, (const char*)priv_hex, priv_len * 2);
    lua_settable(L, -3);

    lua_pushstring(L, "public_key");
    lua_pushlstring(L, (const char*)pub_hex, pub_len * 2);
    lua_settable(L, -3);

    iot_free(priv_der);
    iot_free(pub_der);

    return 1;
}

/* SM2 签名 */
static int iot_gmssl_sm2_sign(lua_State* L) {
    size_t keylen, msglen;
    const char* key_hex = luaL_checklstring(L, 1, &keylen);
    const char* msg = luaL_checklstring(L, 2, &msglen);

    /* 解码私钥 */
    uint8_t key_bin[64];
    size_t key_bin_len = hex_decode(key_hex, key_bin);
    if (key_bin_len != SM2_PRIVATE_KEY_SIZE) {
        luaL_error(L, "invalid SM2 private key length");
        return 0;
    }

    SM2_KEY key;
    sm2_z256_t prv;
    memcpy(prv, key_bin, sizeof(sm2_z256_t));

    /* 从私钥派生公钥（简化处理） */
    if (sm2_key_set_private_key(&key, prv) != 1) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid SM2 private key");
        return 2;
    }

    /* 签名 */
    uint8_t sig[64];
    size_t siglen = sizeof(sig);

    if (sm2_sign(&key, (const uint8_t*)msg, msglen, sig, &siglen) != 1) {
        lua_pushnil(L);
        lua_pushstring(L, "SM2 sign failed");
        return 2;
    }

    /* 转换为十六进制 */
    uint8_t sig_hex[128];
    hex_encode(sig, siglen, (char*)sig_hex);

    lua_pushlstring(L, (const char*)sig_hex, siglen * 2);
    return 1;
}

/* SM2 验签 */
static int iot_gmssl_sm2_verify(lua_State* L) {
    size_t pubkeylen, msglen, siglen;
    const char* pubkey_hex = luaL_checklstring(L, 1, &pubkeylen);
    const char* msg = luaL_checklstring(L, 2, &msglen);
    const char* sig_hex = luaL_checklstring(L, 3, &siglen);

    /* 解码公钥 */
    uint8_t pub_bin[64];
    size_t pub_bin_len = hex_decode(pubkey_hex, pub_bin);
    if (pub_bin_len != SM2_PUBLIC_KEY_SIZE) {
        lua_pushboolean(L, 0);
        return 1;
    }

    SM2_KEY key;
    SM2_Z256_POINT pub;
    memcpy(&pub, pub_bin, sizeof(SM2_Z256_POINT));
    if (sm2_key_set_public_key(&key, &pub) != 1) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 解码签名 */
    uint8_t sig[64];
    size_t sig_bin_len = hex_decode(sig_hex, sig);
    if (sig_bin_len < 64) {
        lua_pushboolean(L, 0);
        return 1;
    }

    /* 验签 */
    int ret = sm2_verify(&key, (const uint8_t*)msg, msglen, sig);

    lua_pushboolean(L, ret == 1);
    return 1;
}

/* 随机数生成 */
static int iot_gmssl_rand_bytes(lua_State* L) {
    int len = (int)luaL_checkinteger(L, 1);
    if (len <= 0 || len > 1024) {
        luaL_error(L, "invalid length: %d", len);
        return 0;
    }

    uint8_t* buf = (uint8_t*)iot_malloc(len);
    if (!buf) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    if (rand_bytes(buf, len) != 1) {
        iot_free(buf);
        luaL_error(L, "random generation failed");
        return 0;
    }

    lua_pushlstring(L, (const char*)buf, len);
    iot_free(buf);
    return 1;
}

/* 十六进制编码 */
static int iot_gmssl_hex_encode(lua_State* L) {
    size_t len;
    const char* data = luaL_checklstring(L, 1, &len);

    char* hex = (char*)iot_malloc(len * 2 + 1);
    if (!hex) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    hex_encode((const uint8_t*)data, len, hex);

    lua_pushstring(L, hex);
    iot_free(hex);
    return 1;
}

/* 十六进制解码 */
static int iot_gmssl_hex_decode(lua_State* L) {
    size_t hexlen;
    const char* hex = luaL_checkstring(L, 1, &hexlen);

    if (hexlen % 2 != 0) {
        luaL_error(L, "hex string must have even length");
        return 0;
    }

    uint8_t* data = (uint8_t*)iot_malloc(hexlen / 2);
    if (!data) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    size_t len = hex_decode(hex, data);

    lua_pushlstring(L, (const char*)data, len);
    iot_free(data);
    return 1;
}

/* Base64 编码 */
static int iot_gmssl_base64_encode(lua_State* L) {
    size_t len;
    const char* data = luaL_checklstring(L, 1, &len);

    char* b64 = (char*)iot_malloc(len * 2 + 1);
    if (!b64) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    size_t b64len = base64_encode((const uint8_t*)data, len, b64);

    lua_pushlstring(L, b64, b64len);
    iot_free(b64);
    return 1;
}

/* Base64 解码 */
static int iot_gmssl_base64_decode(lua_State* L) {
    size_t len;
    const char* b64 = luaL_checkstring(L, 1, &len);

    uint8_t* data = (uint8_t*)iot_malloc(len);
    if (!data) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    size_t outlen = base64_decode(b64, data);
    if (outlen == 0) {
        iot_free(data);
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, (const char*)data, outlen);
    iot_free(data);
    return 1;
}

/* 版本信息 */
static int iot_gmssl_version(lua_State* L) {
    lua_pushstring(L, "GmSSL 3.1.0");
    return 1;
}

static const luaL_Reg gmssl_lib[] = {
    {"sm3",           iot_gmssl_sm3},
    {"sm3_hmac",      iot_gmssl_sm3_hmac},
    {"sm4_encrypt",   iot_gmssl_sm4_encrypt},
    {"sm4_decrypt",   iot_gmssl_sm4_decrypt},
    {"sm2_keygen",    iot_gmssl_sm2_keygen},
    {"sm2_sign",      iot_gmssl_sm2_sign},
    {"sm2_verify",    iot_gmssl_sm2_verify},
    {"rand_bytes",    iot_gmssl_rand_bytes},
    {"hex_encode",    iot_gmssl_hex_encode},
    {"hex_decode",    iot_gmssl_hex_decode},
    {"base64_encode", iot_gmssl_base64_encode},
    {"base64_decode", iot_gmssl_base64_decode},
    {"version",       iot_gmssl_version},
    {NULL, NULL}
};

LUAMOD_API int luaopen_gmssl(lua_State* L) {
    luaL_newlib(L, gmssl_lib);
    return 1;
}