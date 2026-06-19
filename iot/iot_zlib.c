/*
@module  zlib
@summary 压缩解压模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     ZLIB
@usage
-- DEFLATE压缩解压
local data = zlib.compress("hello world", 1)
local original = zlib.decompress(data)

-- GZIP压缩解压
local gz_data = zlib.gzip_compress("hello world", 6)
local gz_orig = zlib.gzip_decompress(gz_data)
zlib.gzip_compress_file("/src.txt", "/dst.gz", 6)
zlib.gzip_decompress_file("/src.gz", "/dst.txt")

-- TAR打包解压
zlib.tar_compress_file("/output.tar.gz", {"file1.txt", "file2.txt"}, 6)
zlib.tar_decompress_file("/input.tar.gz", "/dest/")

-- ZIP压缩解压
zlib.zip_compress_file("/output.zip", {"file1.txt", "file2.txt"}, 6)
zlib.zip_decompress_file("/input.zip", "/dest/")
*/

#include "lua.h"
#include "iot_base.h"
#include "zlib.h"
#include "iot_mem.h"

/* ==================== DEFLATE 接口 ==================== */

/**
 * @brief DEFLATE压缩
 * @api zlib.deflate_compress(data, level)
 * @string data 要压缩的数据
 * @int level 压缩级别 1-9
 * @return string|nil 压缩后的数据，失败返回nil
 */
static int iot_zlib_deflate_compress(lua_State *L) {
    size_t src_len = 0;
    const uint8_t *src = (const uint8_t *)luaL_checklstring(L, 1, &src_len);
    int level = (int)luaL_optinteger(L, 2, 6);

    if (level < 1) level = 1;
    if (level > 9) level = 9;

    size_t dst_len = src_len + (src_len >> 12) + (src_len >> 14) + (src_len >> 25) + 21;
    uint8_t *dst = (uint8_t *)iot_malloc(dst_len);
    if (!dst) {
        lua_pushnil(L);
        return 1;
    }

    size_t compressed_len = dst_len;
    int ret = zlib_deflate_compress(src, src_len, dst, &compressed_len, level);
    if (ret != ZLIB_DEFLATE_OK) {
        iot_free(dst);
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, (const char *)dst, compressed_len);
    iot_free(dst);
    return 1;
}

/**
 * @brief DEFLATE解压
 * @api zlib.deflate_decompress(data)
 * @string data 要解压的数据
 * @return string|nil 解压后的数据，失败返回nil
 */
static int iot_zlib_deflate_decompress(lua_State *L) {
    size_t src_len = 0;
    const uint8_t *src = (const uint8_t *)luaL_checklstring(L, 1, &src_len);

    size_t dst_len = src_len * 100;
    if (dst_len > 32 * 1024 * 1024) dst_len = 32 * 1024 * 1024;

    uint8_t *dst = (uint8_t *)iot_malloc(dst_len);
    if (!dst) {
        lua_pushnil(L);
        return 1;
    }

    int ret = zlib_deflate_decompress(src, src_len, dst, dst_len);
    if (ret != ZLIB_DEFLATE_OK) {
        iot_free(dst);
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, (const char *)dst, dst_len);
    iot_free(dst);
    return 1;
}

/* ==================== GZIP 接口 ==================== */

/**
 * @brief GZIP压缩
 * @api zlib.gzip_compress(data, level)
 * @string data 要压缩的数据
 * @int level 压缩级别 1-9
 * @return string|nil 压缩后的数据，失败返回nil
 */
static int iot_zlib_gzip_compress(lua_State *L) {
    size_t src_len = 0;
    const uint8_t *src = (const uint8_t *)luaL_checklstring(L, 1, &src_len);
    int level = (int)luaL_optinteger(L, 2, 6);

    if (level < 1) level = 1;
    if (level > 9) level = 9;

    size_t dst_len = src_len + (src_len >> 12) + (src_len >> 14) + (src_len >> 25) + 21;
    uint8_t *dst = (uint8_t *)iot_malloc(dst_len);
    if (!dst) {
        lua_pushnil(L);
        return 1;
    }

    size_t compressed_len = dst_len;
    int ret = gzip_compress(src, src_len, dst, &compressed_len, level);
    if (ret != GZIP_OK) {
        iot_free(dst);
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, (const char *)dst, compressed_len);
    iot_free(dst);
    return 1;
}

/**
 * @brief GZIP解压
 * @api zlib.gzip_decompress(data)
 * @string data 要解压的数据
 * @return string|nil 解压后的数据，失败返回nil
 */
static int iot_zlib_gzip_decompress(lua_State *L) {
    size_t src_len = 0;
    const uint8_t *src = (const uint8_t *)luaL_checklstring(L, 1, &src_len);

    size_t dst_len = src_len * 100;
    if (dst_len > 32 * 1024 * 1024) dst_len = 32 * 1024 * 1024;

    uint8_t *dst = (uint8_t *)iot_malloc(dst_len);
    if (!dst) {
        lua_pushnil(L);
        return 1;
    }

    size_t decompressed_len = dst_len;
    int ret = gzip_decompress(src, src_len, dst, &decompressed_len);
    if (ret != GZIP_OK) {
        iot_free(dst);
        lua_pushnil(L);
        return 1;
    }

    lua_pushlstring(L, (const char *)dst, decompressed_len);
    iot_free(dst);
    return 1;
}

/**
 * @brief 压缩文件为GZIP
 * @api zlib.gzip_compress_file(src_path, dst_path, level)
 * @string src_path 源文件路�?
 * @string dst_path 目标GZIP文件路径
 * @int level 压缩级别 1-9
 * @return bool true成功，false失败
 */
static int iot_zlib_gzip_compress_file(lua_State *L) {
    const char *src_path = luaL_checkstring(L, 1);
    const char *dst_path = luaL_checkstring(L, 2);
    int level = (int)luaL_optinteger(L, 3, 6);

    if (level < 1) level = 1;
    if (level > 9) level = 9;

    int ret = gzip_compress_file(src_path, dst_path, level);
    lua_pushboolean(L, ret == GZIP_OK);
    return 1;
}

/**
 * @brief 解压GZIP文件
 * @api zlib.gzip_decompress_file(src_path, dst_path)
 * @string src_path 源GZIP文件路径
 * @string dst_path 目标文件路径
 * @return bool true成功，false失败
 */
static int iot_zlib_gzip_decompress_file(lua_State *L) {
    const char *src_path = luaL_checkstring(L, 1);
    const char *dst_path = luaL_checkstring(L, 2);

    int ret = gzip_decompress_file(src_path, dst_path);
    lua_pushboolean(L, ret == GZIP_OK);
    return 1;
}

/* ==================== TAR 接口 ==================== */

/**
 * @brief 创建TAR.GZ文件
 * @api zlib.tar_compress_file(dest_path, files, level)
 * @string dest_path 目标TAR.GZ文件路径
 * @table files 要打包的文件名数�?
 * @int level 压缩级别 1-9
 * @return bool true成功，false失败
 */
static int iot_zlib_tar_compress_file(lua_State *L) {
    const char *dest_path = luaL_checkstring(L, 1);

    if (!lua_istable(L, 2)) {
        lua_pushboolean(L, false);
        return 1;
    }

    int file_count = (int)lua_rawlen(L, 2);
    if (file_count == 0) {
        lua_pushboolean(L, false);
        return 1;
    }

    int level = (int)luaL_optinteger(L, 3, 6);
    if (level < 1) level = 1;
    if (level > 9) level = 9;

    const char **files = (const char **)iot_malloc(file_count * sizeof(const char *));
    if (!files) {
        lua_pushboolean(L, false);
        return 1;
    }

    for (int i = 0; i < file_count; i++) {
        lua_rawgeti(L, 2, i + 1);
        files[i] = luaL_checkstring(L, -1);
        lua_pop(L, 1);
    }

    int ret = tar_compress_file(".", files, file_count, dest_path, level);
    iot_free(files);

    lua_pushboolean(L, ret == TAR_OK);
    return 1;
}

/**
 * @brief 解压TAR或TAR.GZ文件
 * @api zlib.tar_decompress_file(tar_path, dest_dir)
 * @string tar_path TAR或TAR.GZ文件路径
 * @string dest_dir 目标解压目录
 * @return bool true成功，false失败
 */
static int iot_zlib_tar_decompress_file(lua_State *L) {
    const char *tar_path = luaL_checkstring(L, 1);
    const char *dest_dir = luaL_checkstring(L, 2);

    int ret = tar_decompress_file(tar_path, dest_dir);
    lua_pushboolean(L, ret == TAR_OK);
    return 1;
}

/* ==================== ZIP 接口 ==================== */

/**
 * @brief 创建ZIP文件
 * @api zlib.zip_compress_file(zip_path, files, level)
 * @string zip_path 目标ZIP文件路径
 * @table files 要添加的文件列表（数组）
 * @int level 压缩级别 0-9�?=存储�?-9=压缩�?
 * @return bool true成功，false失败
 */
static int iot_zlib_zip_compress_file(lua_State *L) {
    const char *zip_path = luaL_checkstring(L, 1);

    if (!lua_istable(L, 2)) {
        lua_pushboolean(L, false);
        return 1;
    }

    int level = (int)luaL_optinteger(L, 3, 6);
    if (level < 0) level = 0;
    if (level > 9) level = 9;

    int file_count = (int)lua_rawlen(L, 2);
    if (file_count == 0) {
        lua_pushboolean(L, false);
        return 1;
    }

    const char **files = (const char **)iot_malloc(file_count * sizeof(const char *));
    if (!files) {
        lua_pushboolean(L, false);
        return 1;
    }

    for (int i = 0; i < file_count; i++) {
        lua_rawgeti(L, 2, i + 1);
        files[i] = luaL_checkstring(L, -1);
        lua_pop(L, 1);
    }

    int ret = zip_compress_file(zip_path, files, file_count, level);
    iot_free(files);

    lua_pushboolean(L, ret == ZIP_OK);
    return 1;
}

/**
 * @brief 解压ZIP文件到指定目�?
 * @api zlib.zip_decompress_file(zip_path, dest_dir)
 * @string zip_path ZIP文件路径
 * @string dest_dir 目标解压目录
 * @return bool true成功，false失败
 */
static int iot_zlib_zip_decompress_file(lua_State *L) {
    const char *zip_path = luaL_checkstring(L, 1);
    const char *dest_dir = luaL_checkstring(L, 2);

    int ret = zip_decompress_file(zip_path, dest_dir);
    lua_pushboolean(L, ret == ZIP_OK);
    return 1;
}

/* ==================== 模块注册 ==================== */

static const luaL_Reg reg_zlib[] =
{
    /* DEFLATE */
    {"compress",     iot_zlib_deflate_compress},
    {"decompress",   iot_zlib_deflate_decompress},

    /* GZIP */
    {"gzip_compress",        iot_zlib_gzip_compress},
    {"gzip_decompress",      iot_zlib_gzip_decompress},
    {"gzip_compress_file",   iot_zlib_gzip_compress_file},
    {"gzip_decompress_file", iot_zlib_gzip_decompress_file},

    /* TAR(包含TAR.GZ) */
    {"tar_compress_file",    iot_zlib_tar_compress_file},
    {"tar_decompress_file",  iot_zlib_tar_decompress_file},

    /* ZIP */
    {"zip_compress_file",    iot_zlib_zip_compress_file},
    {"zip_decompress_file",  iot_zlib_zip_decompress_file},

    {NULL, NULL}
};

LUAMOD_API int luaopen_zlib(lua_State *L) {
    luaL_newlibtable(L, reg_zlib);
    luaL_setfuncs(L, reg_zlib, 0);
    return 1;
}
