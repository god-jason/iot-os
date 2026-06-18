/*
@module  gmssl
@summary 国密算法模块
@version 1.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Crypto
*/

/**
GmSSL参考示例

-- SM3哈希
local digest = gmssl.sm3("hello world")

-- SM4加密
local key = gmssl.rand_bytes(16)  -- 128位密钥
local iv = gmssl.rand_bytes(16)   -- 初始向量
local ciphertext = gmssl.sm4_encrypt("cbc", key, iv, "hello world")
local plaintext = gmssl.sm4_decrypt("cbc", key, iv, ciphertext)

-- SM2密钥生成
local sm2_key = gmssl.sm2_keygen()

-- SM2签名
local signature = gmssl.sm2_sign(sm2_key.private_key, "message to sign")

-- SM2验签
local valid = gmssl.sm2_verify(sm2_key.public_key, "message to sign", signature)

-- 十六进制转换
local hex = gmssl.hex_encode("hello")
local data = gmssl.hex_decode(hex)

-- Base64编码
local b64 = gmssl.base64_encode("hello")
local data = gmssl.base64_decode(b64)
*/

#ifndef IOT_GMSSL_H
#define IOT_GMSSL_H

#include "lua.h"

LUAMOD_API int luaopen_gmssl(lua_State* L);

#endif /* IOT_GMSSL_H */