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

#include "cm_mem.h"
#include "cm_fs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 动态内存分配宏
 * @param size 要分配的字节数
 * @return 成功返回分配内存的指针，失败返回NULL
 *
 * 适配到目标系统的内存分配函数。
 */
#define zlib_malloc(size)       cm_malloc(size)

/**
 * @brief 动态内存释放宏
 * @param ptr 要释放的内存指针
 *
 * 适配到目标系统的内存释放函数。
 */
#define zlib_free(ptr)          cm_free(ptr)

/**
 * @brief 动态内存重新分配宏
 * @param ptr 原内存指针
 * @param size 新分配的字节数
 * @return 成功返回重新分配内存的指针，失败返回NULL
 *
 * 适配到目标系统的内存重新分配函数。
 */
#define zlib_realloc(ptr, size) cm_realloc(ptr, size)

/**
 * @brief 文件系统文件打开宏
 * @param path 文件路径
 * @param mode 打开模式（读/写/二进制等）
 * @return 成功返回文件句柄，失败返回NULL
 *
 * 适配到目标系统的文件系统打开函数。
 */
#define zlib_fs_open(path, mode)    cm_fs_open(path, mode)

/**
 * @brief 文件系统文件关闭宏
 * @param fd 文件句柄
 *
 * 适配到目标系统的文件系统关闭函数。
 */
#define zlib_fs_close(fd)           cm_fs_close(fd)

/**
 * @brief 文件系统文件读取宏
 * @param fd 文件句柄
 * @param buf 读取数据缓冲区
 * @param size 要读取的字节数
 * @return 成功返回实际读取的字节数，失败返回-1
 *
 * 适配到目标系统的文件系统读取函数。
 */
#define zlib_fs_read(fd, buf, size)     cm_fs_read(fd, buf, size)

/**
 * @brief 文件系统文件写入宏
 * @param fd 文件句柄
 * @param buf 要写入的数据缓冲区
 * @param size 要写入的字节数
 * @return 成功返回实际写入的字节数，失败返回-1
 *
 * 适配到目标系统的文件系统写入函数。
 */
#define zlib_fs_write(fd, buf, size)    cm_fs_write(fd, buf, size)

/**
 * @brief 文件系统文件指针定位宏
 * @param fd 文件句柄
 * @param offset 偏移量
 * @param origin 起始位置（SEEK_SET/SEEK_CUR/SEEK_END）
 * @return 成功返回新位置，失败返回-1
 *
 * 适配到目标系统的文件系统定位函数。
 */
#define zlib_fs_seek(fd, offset, origin)     cm_fs_seek(fd, offset, origin)

/**
 * @brief 文件系统获取文件大小宏
 * @param path 文件路径
 * @return 成功返回文件大小，失败返回-1
 *
 * 适配到目标系统的获取文件大小函数。
 */
#define zlib_fs_filesize(path)              cm_fs_filesize(path)

/**
 * @brief 文件系统创建目录宏
 * @param path 目录路径
 * @return 成功返回0，失败返回-1
 *
 * 适配到目标系统的创建目录函数。
 */
#define zlib_fs_mkdir(path)                 cm_fs_mkdir(path)

/* 文件打开模式（ML307N cm_fs 使用整型 flag，非 fopen 风格字符串） */
#define ZLIB_FS_RB         CM_FS_RB
#define ZLIB_FS_WB         CM_FS_WB
#define ZLIB_FS_RB_PLUS    CM_FS_RBPLUS
#define ZLIB_FS_WB_PLUS    CM_FS_WBPLUS

/* 文件指针定位起始位置 */
#define ZLIB_FS_SEEK_SET   CM_FS_SEEK_SET
#define ZLIB_FS_SEEK_CUR   CM_FS_SEEK_CUR
#define ZLIB_FS_SEEK_END   CM_FS_SEEK_END

typedef cm_fs_t zlib_fs_t;

#ifdef __cplusplus
}
#endif

#endif
