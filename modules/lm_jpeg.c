#include "lua.h"
#include "lauxlib.h"
#include "iot_base.h"
#include "iot_jpeg.h"

#ifdef ENABLE_JPEG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jpeglib.h"
#include "jerror.h"
#endif

/* JPEG编码 */
static int iot_jpeg_encode(lua_State* L) {
#ifdef ENABLE_JPEG
    size_t data_len;
    const char* rgb_data = luaL_checklstring(L, 1, &data_len);
    int width = (int)luaL_checkinteger(L, 2);
    int height = (int)luaL_checkinteger(L, 3);
    int quality = (int)luaL_optinteger(L, 4, 75);  /* 默认质量75 */

    /* 验证参数 */
    if (width <= 0 || height <= 0) {
        luaL_error(L, "invalid width or height");
        return 0;
    }

    /* 验证数据长度 (RGB24格式) */
    size_t expected_len = width * height * 3;
    if (data_len < expected_len) {
        luaL_error(L, "RGB data too short: expected %d, got %d", expected_len, data_len);
        return 0;
    }

    /* quality范围: 1-100 */
    if (quality < 1) quality = 1;
    if (quality > 100) quality = 100;

    /* 创建内存输出缓冲区 */
    size_t out_size = width * height * 3;
    uint8_t* out_buf = (uint8_t*)cm_malloc(out_size);
    if (!out_buf) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    /* JPEG编码结构 */
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    unsigned char* out_buffer = NULL;
    unsigned long out_len = 0;

    /* 初始化错误处理 */
    cinfo.err = jpeg_std_error(&jerr);

    /* 创建压缩对象 */
    jpeg_create_compress(&cinfo);

    /* 设置内存输出 */
    jpeg_mem_dest(&cinfo, &out_buffer, &out_len);

    /* 设置图像参数 */
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;      /* RGB */
    cinfo.in_color_space = JCS_RGB;  /* 颜色空间 */

    /* 设置默认压缩参数 */
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);

    /* 开始压缩 */
    jpeg_start_compress(&cinfo, TRUE);

    /* 按行写入数据 */
    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = (JSAMPROW)(rgb_data + cinfo.next_scanline * width * 3);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    /* 完成压缩 */
    jpeg_finish_compress(&cinfo);

    /* 复制输出数据 */
    if (out_len > 0 && out_buffer) {
        lua_pushlstring(L, (const char*)out_buffer, out_len);
        free(out_buffer);  /* jpeg_mem_dest 使用 malloc */
    } else {
        cm_free(out_buf);
        lua_pushnil(L);
        lua_pushstring(L, "JPEG encode failed");
        jpeg_destroy_compress(&cinfo);
        return 2;
    }

    /* 清理 */
    jpeg_destroy_compress(&cinfo);
    cm_free(out_buf);

    return 1;
#else
    lua_pushnil(L);
    lua_pushstring(L, "JPEG module not enabled");
    return 2;
#endif
}

/* JPEG解码 */
static int iot_jpeg_decode(lua_State* L) {
#ifdef ENABLE_JPEG
    size_t jpeg_len;
    const char* jpeg_data = luaL_checklstring(L, 1, &jpeg_len);

    if (jpeg_len <= 0) {
        luaL_error(L, "empty JPEG data");
        return 0;
    }

    /* JPEG解码结构 */
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    /* 初始化错误处理 */
    cinfo.err = jpeg_std_error(&jerr);

    /* 创建解压对象 */
    jpeg_create_decompress(&cinfo);

    /* 设置输入源 */
    jpeg_mem_src(&cinfo, (unsigned char*)jpeg_data, jpeg_len);

    /* 读取JPEG头 */
    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        jpeg_destroy_decompress(&cinfo);
        lua_pushnil(L);
        lua_pushstring(L, "invalid JPEG header");
        return 2;
    }

    /* 开始解压 */
    jpeg_start_decompress(&cinfo);

    int width = cinfo.output_width;
    int height = cinfo.output_height;
    int components = cinfo.output_components;  /* 通常为3(RGB)或1(灰度) */

    /* 分配输出缓冲区 */
    size_t rgb_size = width * height * components;
    uint8_t* rgb_buf = (uint8_t*)cm_malloc(rgb_size);
    if (!rgb_buf) {
        jpeg_destroy_decompress(&cinfo);
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    /* 按行读取数据 */
    JSAMPROW row_pointer[1];
    while (cinfo.output_scanline < cinfo.output_height) {
        row_pointer[0] = (JSAMPROW)(rgb_buf + cinfo.output_scanline * width * components);
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    /* 完成解压 */
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    /* 返回RGB数据、宽度、高度 */
    lua_pushlstring(L, (const char*)rgb_buf, rgb_size);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);

    cm_free(rgb_buf);
    return 3;
#else
    lua_pushnil(L);
    lua_pushstring(L, "JPEG module not enabled");
    return 2;
#endif
}

/* 获取JPEG信息 */
static int iot_jpeg_info(lua_State* L) {
#ifdef ENABLE_JPEG
    size_t jpeg_len;
    const char* jpeg_data = luaL_checklstring(L, 1, &jpeg_len);

    if (jpeg_len <= 0) {
        lua_pushnil(L);
        lua_pushstring(L, "empty JPEG data");
        return 2;
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_mem_src(&cinfo, (unsigned char*)jpeg_data, jpeg_len);

    if (jpeg_read_header(&cinfo, TRUE) != JPEG_HEADER_OK) {
        jpeg_destroy_decompress(&cinfo);
        lua_pushnil(L);
        lua_pushstring(L, "invalid JPEG header");
        return 2;
    }

    lua_newtable(L);
    lua_pushstring(L, "width");
    lua_pushinteger(L, cinfo.image_width);
    lua_settable(L, -3);

    lua_pushstring(L, "height");
    lua_pushinteger(L, cinfo.image_height);
    lua_settable(L, -3);

    lua_pushstring(L, "components");
    lua_pushinteger(L, cinfo.num_components);
    lua_settable(L, -3);

    lua_pushstring(L, "color_space");
    switch (cinfo.jpeg_color_space) {
        case JCS_GRAYSCALE: lua_pushstring(L, "grayscale"); break;
        case JCS_RGB: lua_pushstring(L, "rgb"); break;
        case JCS_YCbCr: lua_pushstring(L, "ycbcr"); break;
        case JCS_CMYK: lua_pushstring(L, "cmyk"); break;
        default: lua_pushstring(L, "unknown"); break;
    }
    lua_settable(L, -3);

    jpeg_destroy_decompress(&cinfo);
    return 1;
#else
    lua_pushnil(L);
    lua_pushstring(L, "JPEG module not enabled");
    return 2;
#endif
}

/* 版本信息 */
static int iot_jpeg_version(lua_State* L) {
#ifdef ENABLE_JPEG
    lua_pushstring(L, "libjpeg-turbo 3.0.1");
#else
    lua_pushnil(L);
#endif
    return 1;
}

static const luaL_Reg jpeg_lib[] = {
    {"encode",  iot_jpeg_encode},
    {"decode",  iot_jpeg_decode},
    {"info",    iot_jpeg_info},
    {"version", iot_jpeg_version},
    {NULL, NULL}
};

LUAMOD_API int luaopen_jpeg(lua_State* L) {
    luaL_newlib(L, jpeg_lib);
    return 1;
}