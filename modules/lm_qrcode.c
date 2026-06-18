#include "lua.h"
#include "lauxlib.h"
#include "iot_base.h"
#include "iot_qrcode.h"

#ifdef ENABLE_QRENCODE
#include "qrencode.h"
#endif

#ifdef ENABLE_QUIRC
#include "quirc.h"
#endif

/* QR码生成 */
static int iot_qrcode_encode(lua_State* L) {
#ifdef ENABLE_QRENCODE
    size_t text_len;
    const char* text = luaL_checklstring(L, 1, &text_len);
    int ecc_level = (int)luaL_optinteger(L, 2, QR_ECLEVEL_M);  /* 默认M等级 */
    int version = (int)luaL_optinteger(L, 3, 0);  /* 0表示自动选择版本 */

    /* 纠错等级范围: 0-3 */
    if (ecc_level < 0) ecc_level = 0;
    if (ecc_level > 3) ecc_level = 3;

    /* 创建QR码 */
    QRcode* qr = QRcode_encodeString(text, version, ecc_level, QR_MODE_8, 1);
    if (!qr) {
        lua_pushnil(L);
        lua_pushstring(L, "QR encode failed");
        return 2;
    }

    /* 返回QR码数据 */
    int size = qr->width;
    int data_size = size * size;

    lua_newtable(L);

    /* 尺寸信息 */
    lua_pushstring(L, "width");
    lua_pushinteger(L, size);
    lua_settable(L, -3);

    /* QR码数据 (每个字节代表一个模块: 1=黑色, 0=白色) */
    lua_pushstring(L, "data");
    lua_pushlstring(L, (const char*)qr->data, data_size);
    lua_settable(L, -3);

    /* 版本信息 */
    lua_pushstring(L, "version");
    lua_pushinteger(L, qr->version);
    lua_settable(L, -3);

    QRcode_free(qr);
    return 1;
#else
    lua_pushnil(L);
    lua_pushstring(L, "QRENCODE module not enabled");
    return 2;
#endif
}

/* QR码解码 */
static int iot_qrcode_decode(lua_State* L) {
#ifdef ENABLE_QUIRC
    size_t image_len;
    const char* image_data = luaL_checklstring(L, 1, &image_len);
    int width = (int)luaL_checkinteger(L, 2);
    int height = (int)luaL_checkinteger(L, 3);

    if (width <= 0 || height <= 0) {
        luaL_error(L, "invalid width or height");
        return 0;
    }

    /* 创建quirc上下文 */
    struct quirc* qr = quirc_new();
    if (!qr) {
        lua_pushnil(L);
        lua_pushstring(L, "quirc init failed");
        return 2;
    }

    /* 设置图像尺寸 */
    if (quirc_resize(qr, width, height) < 0) {
        quirc_destroy(qr);
        lua_pushnil(L);
        lua_pushstring(L, "quirc resize failed");
        return 2;
    }

    /* 获取图像缓冲区并填充数据 */
    uint8_t* buffer = quirc_begin(qr, NULL, NULL);
    if (!buffer) {
        quirc_destroy(qr);
        lua_pushnil(L);
        lua_pushstring(L, "quirc buffer failed");
        return 2;
    }

    /* 假设输入是灰度图像，直接复制 */
    size_t expected_len = width * height;
    if (image_len < expected_len) {
        quirc_destroy(qr);
        luaL_error(L, "image data too short");
        return 0;
    }
    memcpy(buffer, image_data, expected_len);

    /* 结束图像填充 */
    quirc_end(qr);

    /* 查找QR码 */
    int num_codes = quirc_count(qr);
    if (num_codes <= 0) {
        quirc_destroy(qr);
        lua_pushnil(L);
        lua_pushstring(L, "no QR code found");
        return 2;
    }

    /* 解码第一个QR码 */
    struct quirc_code code;
    struct quirc_result result;

    quirc_extract(qr, 0, &code);
    quirc_decode(&code, &result);

    if (result.payload_len <= 0) {
        quirc_destroy(qr);
        lua_pushnil(L);
        lua_pushstring(L, "decode failed");
        return 2;
    }

    /* 返回解码结果 */
    lua_pushstring(L, result.payload);

    quirc_destroy(qr);
    return 1;
#else
    lua_pushnil(L);
    lua_pushstring(L, "QUIRC module not enabled");
    return 2;
#endif
}

/* QR码数量统计 */
static int iot_qrcode_count(lua_State* L) {
#ifdef ENABLE_QUIRC
    size_t image_len;
    const char* image_data = luaL_checklstring(L, 1, &image_len);
    int width = (int)luaL_checkinteger(L, 2);
    int height = (int)luaL_checkinteger(L, 3);

    if (width <= 0 || height <= 0) {
        lua_pushinteger(L, 0);
        return 1;
    }

    struct quirc* qr = quirc_new();
    if (!qr) {
        lua_pushinteger(L, 0);
        return 1;
    }

    if (quirc_resize(qr, width, height) < 0) {
        quirc_destroy(qr);
        lua_pushinteger(L, 0);
        return 1;
    }

    uint8_t* buffer = quirc_begin(qr, NULL, NULL);
    if (!buffer) {
        quirc_destroy(qr);
        lua_pushinteger(L, 0);
        return 1;
    }

    size_t expected_len = width * height;
    if (image_len >= expected_len) {
        memcpy(buffer, image_data, expected_len);
    }

    quirc_end(qr);
    int count = quirc_count(qr);
    quirc_destroy(qr);

    lua_pushinteger(L, count);
    return 1;
#else
    lua_pushinteger(L, 0);
    return 1;
#endif
}

/* 版本信息 */
static int iot_qrcode_version(lua_State* L) {
#ifdef ENABLE_QRENCODE
    lua_pushstring(L, "libqrencode 4.1.1 + quirc");
#else
#ifdef ENABLE_QUIRC
    lua_pushstring(L, "quirc only");
#else
    lua_pushnil(L);
#endif
#endif
    return 1;
}

static const luaL_Reg qrcode_lib[] = {
    {"encode",  iot_qrcode_encode},
    {"decode",  iot_qrcode_decode},
    {"count",   iot_qrcode_count},
    {"version", iot_qrcode_version},
    {NULL, NULL}
};

LUAMOD_API int luaopen_qrcode(lua_State* L) {
    luaL_newlib(L, qrcode_lib);
    return 1;
}