/**
 * @file lua_module.c
 * @brief zlib Lua 模块封装
 *
 * 提供面向对象的压缩/解压接口，支持 DEFLATE、GZIP、ZIP、TAR 格式
 */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Lua 头文件 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

/* IoT 核心头文件 */
#include "iot_log.h"
#include "iot.h"

/* zlib 组件头文件 */
#include "zlib.h"
#include "deflate.h"
#include "gzip.h"
#include "zip.h"
#include "tar.h"

/*===========================================================
 * 辅助函数
 *===========================================================*/

/* 将 Lua 字符串压入栈（处理 nil） */
static void lua_pushstring_safe(lua_State* L, const char* str, size_t len) {
    if (str && len > 0) {
        lua_pushlstring(L, str, len);
    } else {
        lua_pushstring(L, "");
    }
}

/* 将错误码转换为错误消息 */
static const char* zlib_error_str(int err) {
    switch (err) {
        case ZLIB_OK:           return "success";
        case ZLIB_ERR_MEM:      return "memory error";
        case ZLIB_ERR_BUF:      return "buffer error";
        case ZLIB_ERR_FORMAT:   return "format error";
        case ZLIB_ERR_CRC:      return "crc error";
        case ZLIB_ERR_FILE:      return "file error";
        case ZLIB_ERR_NOT_FOUND: return "not found";
        default:                return "unknown error";
    }
}

/*===========================================================
 * DEFLATE 模块 (deflate)
 *===========================================================*/

/**
 * @brief deflate.compress(data [, level])
 * @string data 要压缩的数据
 * @int level 压缩级别（1-9），默认6
 * @return string 压缩后的数据，失败返回 nil
 */
static int luaopen_zlib_deflate_compress(lua_State* L) {
    size_t src_len = 0;
    const char* src = luaL_checklstring(L, 1, &src_len);
    int level = (int)luaL_optinteger(L, 2, ZLIB_DEFLATE_LEVEL_DEFAULT);
    
    if (level < 1) level = ZLIB_DEFLATE_LEVEL_FAST;
    if (level > 9) level = ZLIB_DEFLATE_LEVEL_BEST;
    
    /* 计算输出缓冲区大小 */
    size_t dst_len = zlib_deflate_bound(src_len);
    uint8_t* dst = (uint8_t*)iot_malloc(dst_len);
    if (!dst) {
        lua_pushnil(L);
        lua_pushstring(L, "memory error");
        return 2;
    }
    
    size_t out_len = dst_len;
    int ret = zlib_deflate_compress((const uint8_t*)src, src_len, dst, &out_len, level);
    
    if (ret != ZLIB_DEFLATE_OK) {
        iot_free(dst);
        lua_pushnil(L);
        lua_pushstring(L, zlib_error_str(ret));
        return 2;
    }
    
    lua_pushlstring(L, (const char*)dst, out_len);
    iot_free(dst);
    return 1;
}

/**
 * @brief deflate.decompress(data)
 * @string data 压缩的数据
 * @return string 解压后的数据，失败返回 nil
 */
static int luaopen_zlib_deflate_decompress(lua_State* L) {
    size_t src_len = 0;
    const char* src = luaL_checklstring(L, 1, &src_len);
    
    /* 估算解压后大小（通常是压缩后的10倍） */
    size_t dst_len = src_len * 10;
    if (dst_len < 4096) dst_len = 4096;
    
    uint8_t* dst = (uint8_t*)iot_malloc(dst_len);
    if (!dst) {
        lua_pushnil(L);
        lua_pushstring(L, "memory error");
        return 2;
    }
    
    int ret = zlib_deflate_decompress((const uint8_t*)src, src_len, dst, dst_len);
    
    if (ret < 0) {
        iot_free(dst);
        lua_pushnil(L);
        lua_pushstring(L, "decompress failed");
        return 2;
    }
    
    lua_pushlstring(L, (const char*)dst, (size_t)ret);
    iot_free(dst);
    return 1;
}

/**
 * @brief deflate.adler32(data [, adler])
 * @string data 要计算的数据
 * @int adler 初始值（可选），默认1
 * @return int ADLER32 校验码
 */
static int luaopen_zlib_deflate_adler32(lua_State* L) {
    size_t len = 0;
    const char* data = luaL_checklstring(L, 1, &len);
    uint32_t adler = (uint32_t)luaL_optinteger(L, 2, 1);
    
    uint32_t result = zlib_adler32(adler, (const uint8_t*)data, len);
    lua_pushinteger(L, result);
    return 1;
}

/**
 * @brief deflate.crc32(data [, crc])
 * @string data 要计算的数据
 * @int crc 初始值（可选），默认0
 * @return int CRC32 校验码
 */
static int luaopen_zlib_deflate_crc32(lua_State* L) {
    size_t len = 0;
    const char* data = luaL_checklstring(L, 1, &len);
    uint32_t crc = (uint32_t)luaL_optinteger(L, 2, 0);
    
    uint32_t result = zlib_crc32(crc, (const uint8_t*)data, len);
    lua_pushinteger(L, result);
    return 1;
}

/* deflate 模块方法表 */
static const luaL_Reg deflate_lib[] = {
    { "compress",  luaopen_zlib_deflate_compress },
    { "decompress", luaopen_zlib_deflate_decompress },
    { "adler32",   luaopen_zlib_deflate_adler32 },
    { "crc32",     luaopen_zlib_deflate_crc32 },
    { NULL, NULL }
};

/*===========================================================
 * GZIP 模块 (gzip)
 *===========================================================*/

/**
 * @brief gzip.compress(data [, level])
 * @string data 要压缩的数据
 * @int level 压缩级别（1-9），默认6
 * @return string 压缩后的数据，失败返回 nil
 */
static int luaopen_zlib_gzip_compress(lua_State* L) {
    size_t src_len = 0;
    const char* src = luaL_checklstring(L, 1, &src_len);
    int level = (int)luaL_optinteger(L, 2, GZIP_COMPRESS_DEFAULT);
    
    if (level < 1) level = GZIP_COMPRESS_FAST;
    if (level > 9) level = GZIP_COMPRESS_BEST;
    
    /* 估算输出缓冲区大小 */
    size_t dst_len = src_len + src_len / 10 + 128;
    uint8_t* dst = (uint8_t*)iot_malloc(dst_len);
    if (!dst) {
        lua_pushnil(L);
        lua_pushstring(L, "memory error");
        return 2;
    }
    
    size_t out_len = dst_len;
    int ret = gzip_compress((const uint8_t*)src, src_len, dst, &out_len, level);
    
    if (ret != GZIP_OK) {
        iot_free(dst);
        lua_pushnil(L);
        lua_pushstring(L, zlib_error_str(ret));
        return 2;
    }
    
    lua_pushlstring(L, (const char*)dst, out_len);
    iot_free(dst);
    return 1;
}

/**
 * @brief gzip.decompress(data)
 * @string data 压缩的数据
 * @return string 解压后的数据，失败返回 nil
 */
static int luaopen_zlib_gzip_decompress(lua_State* L) {
    size_t src_len = 0;
    const char* src = luaL_checklstring(L, 1, &src_len);
    
    /* 估算解压后大小 */
    size_t dst_len = src_len * 10;
    if (dst_len < 4096) dst_len = 4096;
    
    uint8_t* dst = (uint8_t*)iot_malloc(dst_len);
    if (!dst) {
        lua_pushnil(L);
        lua_pushstring(L, "memory error");
        return 2;
    }
    
    size_t out_len = dst_len;
    int ret = gzip_decompress((const uint8_t*)src, src_len, dst, &out_len);
    
    if (ret != GZIP_OK) {
        iot_free(dst);
        lua_pushnil(L);
        lua_pushstring(L, zlib_error_str(ret));
        return 2;
    }
    
    lua_pushlstring(L, (const char*)dst, out_len);
    iot_free(dst);
    return 1;
}

/**
 * @brief gzip.compress_file(src_path, dst_path [, level])
 * @string src_path 源文件路径
 * @string dst_path 目标 gzip 文件路径
 * @int level 压缩级别（可选）
 * @return bool 成功返回 true
 */
static int luaopen_zlib_gzip_compress_file(lua_State* L) {
    const char* src_path = luaL_checkstring(L, 1);
    const char* dst_path = luaL_checkstring(L, 2);
    int level = (int)luaL_optinteger(L, 3, GZIP_COMPRESS_DEFAULT);
    
    int ret = gzip_compress_file(src_path, dst_path, level);
    if (ret != GZIP_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, zlib_error_str(ret));
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief gzip.decompress_file(src_path, dst_path)
 * @string src_path 源 gzip 文件路径
 * @string dst_path 目标文件路径
 * @return bool 成功返回 true
 */
static int luaopen_zlib_gzip_decompress_file(lua_State* L) {
    const char* src_path = luaL_checkstring(L, 1);
    const char* dst_path = luaL_checkstring(L, 2);
    
    int ret = gzip_decompress_file(src_path, dst_path);
    if (ret != GZIP_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, zlib_error_str(ret));
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/* gzip 模块方法表 */
static const luaL_Reg gzip_lib[] = {
    { "compress",      luaopen_zlib_gzip_compress },
    { "decompress",    luaopen_zlib_gzip_decompress },
    { "compress_file", luaopen_zlib_gzip_compress_file },
    { "decompress_file", luaopen_zlib_gzip_decompress_file },
    { NULL, NULL }
};

/*===========================================================
 * ZIP 模块 (zip)
 *===========================================================*/

/**
 * @brief zip.decompress_file(zip_path, dest_dir)
 * @string zip_path ZIP 文件路径
 * @string dest_dir 目标解压目录
 * @return bool 成功返回 true
 */
static int luaopen_zlib_zip_decompress_file(lua_State* L) {
    const char* zip_path = luaL_checkstring(L, 1);
    const char* dest_dir = luaL_checkstring(L, 2);
    
    int ret = zip_decompress_file(zip_path, dest_dir);
    if (ret != ZIP_OK) {
        lua_pushboolean(L, 0);
        /* ZIP 错误码单独处理 */
        const char* err_msg;
        switch (ret) {
            case ZIP_ERR_MEM:       err_msg = "memory error"; break;
            case ZIP_ERR_FORMAT:    err_msg = "format error"; break;
            case ZIP_ERR_CRC:       err_msg = "crc error"; break;
            case ZIP_ERR_FILE:      err_msg = "file error"; break;
            case ZIP_ERR_NOT_FOUND: err_msg = "not found"; break;
            default:                err_msg = "unknown error"; break;
        }
        lua_pushstring(L, err_msg);
        return 2;
    }
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief zip.compress_file(zip_path, files [, level])
 * @string zip_path 目标 ZIP 文件路径
 * @table files 要压缩的文件路径数组
 * @int level 压缩级别（可选）
 * @return bool 成功返回 true
 */
static int luaopen_zlib_zip_compress_file(lua_State* L) {
    const char* zip_path = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    int level = (int)luaL_optinteger(L, 3, 6);
    
    /* 获取文件列表 */
    int file_count = (int)lua_rawlen(L, 2);
    if (file_count == 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "no files to compress");
        return 2;
    }
    
    const char** files = (const char**)iot_malloc(sizeof(const char*) * file_count);
    if (!files) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "memory error");
        return 2;
    }
    
    for (int i = 0; i < file_count; i++) {
        lua_rawgeti(L, 2, i + 1);
        files[i] = luaL_checkstring(L, -1);
        lua_pop(L, 1);
    }
    
    int ret = zip_compress_file(zip_path, files, file_count, level);
    iot_free(files);
    
    if (ret != ZIP_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, zlib_error_str(ret));
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/* zip 模块方法表 */
static const luaL_Reg zip_lib[] = {
    { "decompress_file", luaopen_zlib_zip_decompress_file },
    { "compress_file",   luaopen_zlib_zip_compress_file },
    { NULL, NULL }
};

/*===========================================================
 * TAR 模块 (tar)
 *===========================================================*/

/**
 * @brief tar.decompress_file(src_path, dst_dir)
 * @string src_path 源 TAR/TAR.GZ 文件路径
 * @string dst_dir 目标解压目录
 * @return bool 成功返回 true
 */
static int luaopen_zlib_tar_decompress_file(lua_State* L) {
    const char* src_path = luaL_checkstring(L, 1);
    const char* dst_dir = luaL_checkstring(L, 2);
    
    int ret = tar_decompress_file(src_path, dst_dir);
    if (ret != TAR_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, zlib_error_str(ret));
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief tar.compress_file(src_dir, files, dst_path [, level])
 * @string src_dir 源文件所在目录
 * @table files 要打包的文件名数组
 * @string dst_path 目标文件路径（.tar 或 .tar.gz）
 * @int level 压缩级别（可选，仅对 .tar.gz 有效）
 * @return bool 成功返回 true
 */
static int luaopen_zlib_tar_compress_file(lua_State* L) {
    const char* src_dir = luaL_checkstring(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    const char* dst_path = luaL_checkstring(L, 3);
    int level = (int)luaL_optinteger(L, 4, TAR_COMPRESS_DEFAULT);
    
    /* 获取文件列表 */
    int file_count = (int)lua_rawlen(L, 2);
    if (file_count == 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "no files to compress");
        return 2;
    }
    
    const char** files = (const char**)iot_malloc(sizeof(const char*) * file_count);
    if (!files) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "memory error");
        return 2;
    }
    
    for (int i = 0; i < file_count; i++) {
        lua_rawgeti(L, 2, i + 1);
        files[i] = luaL_checkstring(L, -1);
        lua_pop(L, 1);
    }
    
    int ret = tar_compress_file(src_dir, files, file_count, dst_path, level);
    iot_free(files);
    
    if (ret != TAR_OK) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, zlib_error_str(ret));
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/* tar 模块方法表 */
static const luaL_Reg tar_lib[] = {
    { "decompress_file", luaopen_zlib_tar_decompress_file },
    { "compress_file",   luaopen_zlib_tar_compress_file },
    { NULL, NULL }
};

/*===========================================================
 * 模块注册
 *===========================================================*/

/**
 * @brief zlib 模块初始化
 * @return table 模块表
 *
 * 使用示例：
 * local zlib = require("zlib")
 *
 * -- DEFLATE 压缩
 * local compressed = zlib.deflate.compress("hello world", 6)
 * local original = zlib.deflate.decompress(compressed)
 *
 * -- GZIP 压缩
 * local gzipped = zlib.gzip.compress("hello world")
 * local ungzipped = zlib.gzip.decompress(gzipped)
 *
 * -- 校验
 * local adler = zlib.deflate.adler32("hello")
 * local crc = zlib.deflate.crc32("hello")
 *
 * -- ZIP 文件操作
 * zlib.zip.decompress_file("test.zip", "./output")
 * zlib.zip.compress_file("output.zip", {"file1.txt", "file2.txt"})
 *
 * -- TAR 文件操作
 * zlib.tar.decompress_file("test.tar.gz", "./output")
 * zlib.tar.compress_file("./input", {"file1.txt"}, "output.tar.gz")
 */
LUAMOD_API int luaopen_zlib_register(lua_State* L) {
    /* 创建 zlib 模块表 */
    lua_newtable(L);
    
    /* 添加版本信息 */
    lua_pushstring(L, ZLIB_VERSION);
    lua_setfield(L, -2, "version");
    
    /* 创建子模块 */
    /* deflate 子模块 */
    lua_newtable(L);
    luaL_setfuncs(L, deflate_lib, 0);
    lua_setfield(L, -2, "deflate");
    
    /* gzip 子模块 */
    lua_newtable(L);
    luaL_setfuncs(L, gzip_lib, 0);
    lua_setfield(L, -2, "gzip");
    
    /* zip 子模块 */
    lua_newtable(L);
    luaL_setfuncs(L, zip_lib, 0);
    lua_setfield(L, -2, "zip");
    
    /* tar 子模块 */
    lua_newtable(L);
    luaL_setfuncs(L, tar_lib, 0);
    lua_setfield(L, -2, "tar");
    
    return 1;
}
