/*
@module  base64
@summary Base64编解码
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@usage
-- 编码
local encoded = base64.encode("Hello")
-- 解码
local decoded = base64.decode(encoded)
*/

#include "lua.h"
#include "iot_base.h"

/*
Base64编码表
*/
static const unsigned char base64_enc_map[64] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
    'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', '+', '/'
};

/*
Base64解码表
*/
static const unsigned char base64_dec_map[128] = {
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127,  62, 127, 127, 127,  63,  52,  53,
     54,  55,  56,  57,  58,  59,  60,  61, 127, 127,
    127,  64, 127, 127, 127,   0,   1,   2,   3,   4,
      5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
     15,  16,  17,  18,  19,  20,  21,  22,  23,  24,
     25, 127, 127, 127, 127, 127, 127,  26,  27,  28,
     29,  30,  31,  32,  33,  34,  35,  36,  37,  38,
     39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
     49,  50,  51, 127, 127, 127, 127, 127
};

#define BASE64_SIZE_T_MAX   ((size_t) -1)

/*
@function base64.encode
@param str 输入字符串
@return string 编码后的Base64字符串
*/
static int base64_encode(lua_State *L) {
    size_t slen = 0;
    const unsigned char *src = (const unsigned char *)luaL_checklstring(L, 1, &slen);
    
    if (slen == 0) {
        lua_pushstring(L, "");
        return 1;
    }

    /* 计算输出缓冲区大小 */
    size_t n = slen / 3 + (slen % 3 != 0);
    if (n > (BASE64_SIZE_T_MAX - 1) / 4) {
        return luaL_error(L, "input too long");
    }
    n *= 4;

    /* 分配输出缓冲区 */
    unsigned char *dst = (unsigned char *)iot_malloc(n + 1);
    if (dst == NULL) {
        return luaL_error(L, "out of memory");
    }

    size_t i;
    unsigned char *p = dst;
    int C1, C2, C3;

    /* 处理完整的三字节组 */
    size_t full_groups = (slen / 3) * 3;
    for (i = 0; i < full_groups; i += 3) {
        C1 = src[i];
        C2 = src[i + 1];
        C3 = src[i + 2];

        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];
        *p++ = base64_enc_map[(((C2 & 15) << 2) + (C3 >> 6)) & 0x3F];
        *p++ = base64_enc_map[C3 & 0x3F];
    }

    /* 处理剩余的字节 */
    if (i < slen) {
        C1 = src[i];
        C2 = (i + 1 < slen) ? src[i + 1] : 0;

        *p++ = base64_enc_map[(C1 >> 2) & 0x3F];
        *p++ = base64_enc_map[(((C1 & 3) << 4) + (C2 >> 4)) & 0x3F];

        if (i + 1 < slen) {
            *p++ = base64_enc_map[((C2 & 15) << 2) & 0x3F];
        } else {
            *p++ = '=';
        }
        *p++ = '=';
    }

    *p = '\0';
    lua_pushstring(L, (const char *)dst);
    iot_free(dst);
    return 1;
}

/*
@function base64.decode
@param str Base64编码的字符串
@return string 解码后的原始字符串，如果解码失败返回nil
*/
static int base64_decode(lua_State *L) {
    size_t slen = 0;
    const unsigned char *src = (const unsigned char *)luaL_checklstring(L, 1, &slen);

    if (slen == 0) {
        lua_pushstring(L, "");
        return 1;
    }

    /* 第一遍：检查有效性并计算输出长度 */
    size_t n = 0;
    int padding = 0;
    for (size_t i = 0; i < slen; i++) {
        /* 跳过空格 */
        while (i < slen && src[i] == ' ') {
            i++;
        }
        if (i >= slen) break;

        /* 跳过换行 */
        if ((slen - i) >= 2 && src[i] == '\r' && src[i + 1] == '\n') {
            i++;
            continue;
        }
        if (src[i] == '\n') continue;

        /* 检查填充字符 */
        if (src[i] == '=') {
            padding++;
            if (padding > 2) {
                lua_pushnil(L);
                lua_pushstring(L, "invalid padding");
                return 2;
            }
            continue;
        }

        /* 检查字符有效性 */
        if (src[i] > 127 || base64_dec_map[src[i]] == 127) {
            lua_pushnil(L);
            lua_pushstring(L, "invalid character");
            return 2;
        }

        n++;
    }

    if (n == 0) {
        lua_pushstring(L, "");
        return 1;
    }

    /* 计算输出长度 */
    n = (6 * (n >> 3)) + ((6 * (n & 0x7) + 7) >> 3);
    n -= padding;

    /* 分配输出缓冲区 */
    unsigned char *dst = (unsigned char *)iot_malloc(n + 1);
    if (dst == NULL) {
        return luaL_error(L, "out of memory");
    }

    /* 第二遍：解码 */
    int j, x;
    unsigned char *p = dst;
    for (j = 3, x = 0, n = 0; slen > 0; slen--, src++) {
        if (*src == '\r' || *src == '\n' || *src == ' ') continue;

        j -= (base64_dec_map[*src] == 64);
        x = (x << 6) | (base64_dec_map[*src] & 0x3F);

        if (++n == 4) {
            n = 0;
            if (j > 0) *p++ = (unsigned char)(x >> 16);
            if (j > 1) *p++ = (unsigned char)(x >> 8);
            if (j > 2) *p++ = (unsigned char)(x);
        }
    }

    *p = '\0';
    lua_pushstring(L, (const char *)dst);
    iot_free(dst);
    return 1;
}

/*
注册base64模块
*/
static const luaL_Reg base64_lib[] = {
    {"encode", base64_encode},
    {"decode", base64_decode},
    {NULL, NULL}
};

LUAMOD_API int luaopen_base64(lua_State *L) {
    luaL_newlibtable(L, base64_lib);
    luaL_setfuncs(L, base64_lib, 0);
    return 1;
}
