/**
 * @file adapter.h
 * @brief 适配器层头文件，定义可配置的内存和文件系统接口宏
 *
 * 本文件提供抽象的内存分配/释放和文件系统操作接口，
 * 方便库移植到不同的嵌入式系统或操作系统。
 * 通过重新定义这些宏，可以适配各种内存管理方案和文件系统API。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef ADAPTER_H
#define ADAPTER_H

#include <stdint.h>
#include <stddef.h>

/* 包含 IoT 核心接口 */
#include "iot_mem.h"
#include "iot_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 动态内存分配宏
 * @param size 要分配的字节数
 * @return 成功返回分配内存的指针，失败返回NULL
 */
#define zlib_malloc(size)       iot_malloc(size)

/**
 * @brief 动态内存释放宏
 * @param ptr 要释放的内存指针
 */
#define zlib_free(ptr)          iot_free(ptr)

/**
 * @brief 动态内存重新分配宏
 * @param ptr 原内存指针
 * @param size 新分配的字节数
 * @return 成功返回重新分配内存的指针，失败返回NULL
 */
#define zlib_realloc(ptr, size) iot_realloc(ptr, size)

/* 文件系统操作使用 iot_fs.h 接口 */

/**
 * @brief 文件系统文件打开宏
 * @param path 文件路径
 * @param mode 打开模式
 * @return 成功返回文件句柄，失败返回NULL
 */
#define zlib_fs_open(path, mode)    iot_fs_open(path, mode)

/**
 * @brief 文件系统文件关闭宏
 * @param fd 文件句柄
 */
#define zlib_fs_close(fd)           iot_fs_close(fd)

/**
 * @brief 文件系统文件读取宏
 * @param fd 文件句柄
 * @param buf 读取数据缓冲区
 * @param size 要读取的字节数
 * @return 成功返回实际读取的字节数，失败返回-1
 */
#define zlib_fs_read(fd, buf, size)     iot_fs_read(fd, buf, size)

/**
 * @brief 文件系统文件写入宏
 * @param fd 文件句柄
 * @param buf 要写入的数据缓冲区
 * @param size 要写入的字节数
 * @return 成功返回实际写入的字节数，失败返回-1
 */
#define zlib_fs_write(fd, buf, size)    iot_fs_write(fd, buf, size)

/**
 * @brief 文件系统文件指针定位宏
 * @param fd 文件句柄
 * @param offset 偏移量
 * @param origin 起始位置（SEEK_SET/SEEK_CUR/SEEK_END）
 * @return 成功返回新位置，失败返回-1
 */
#define zlib_fs_seek(fd, offset, origin)     iot_fs_seek(fd, offset, origin)

/**
 * @brief 文件系统获取文件大小宏
 * @param path 文件路径
 * @return 成功返回文件大小，失败返回-1
 */
#define zlib_fs_filesize(path)              iot_fs_filesize(path)

/**
 * @brief 文件系统创建目录宏
 * @param path 目录路径
 * @return 成功返回0，失败返回-1
 */
#define zlib_fs_mkdir(path)                 iot_fs_mkdir(path)

/* 文件打开模式映射 */
#define ZLIB_FS_RB         IOT_FS_MODE_RB
#define ZLIB_FS_WB         IOT_FS_MODE_WB
#define ZLIB_FS_RB_PLUS    IOT_FS_MODE_R_PLUS
#define ZLIB_FS_WB_PLUS    IOT_FS_MODE_W_PLUS

/* 文件指针定位起始位置 */
#define ZLIB_FS_SEEK_SET   IOT_FS_SEEK_SET
#define ZLIB_FS_SEEK_CUR   IOT_FS_SEEK_CUR
#define ZLIB_FS_SEEK_END   IOT_FS_SEEK_END

typedef iot_fs_file_t zlib_fs_t;

#ifdef __cplusplus
}
#endif

#endif
