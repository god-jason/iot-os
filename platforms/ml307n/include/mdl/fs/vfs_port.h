/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        vfs_port.h
 *
 * @brief       文件系统待移植接口定义.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-04-21     ict team          创建
 ************************************************************************************
 */


#ifndef _OS_VFS_PORT_H_
#define _OS_VFS_PORT_H_

/************************************************************************************
 *                                 头文件
 ************************************************************************************/
#include <os.h>
#include <vfs.h>

/************************************************************************************
 *                                 配置开关
 ************************************************************************************/
//#define VFS_USING_RECYCLE

/************************************************************************************
 *                                 接口实现
 ************************************************************************************/
#define VFS_GET_ERRNO()                     osGetErrno()
#define VFS_SET_ERRNO(err)                  osSetErrno(err)

#define VFS_ASSERT(exp)                     OS_ASSERT((exp))

#define VFS_PRINTF(fmt, ...)                osPrintf(fmt, ##__VA_ARGS__)

#define VFS_SPRINTF(buf, fmt, ...)          osSprintf(buf, fmt, ##__VA_ARGS__)
#define VFS_SNPRINTF(buf, size, fmt, ...)   osSnprintf(buf, size, fmt, ##__VA_ARGS__)

/************************************************************************************
 *                                 宏定义
 ************************************************************************************/
#ifdef VFS_THREADSAFE
#define LFS_THREADSAFE
#endif

#ifdef VFS_USING_RECYCLE
#define LFS_USING_RECYCLE
#endif

#ifdef VFS_NAME_MAX
#define LFS_NAME_MAX    VFS_NAME_MAX
#endif

/**
 * 内存分配
 */
#ifndef VFS_MALLOC
#define VFS_MALLOC(n)           malloc((n))
#endif

/**
 * 内存分配
 */
#ifndef VFS_CALLOC
#define VFS_CALLOC(n, size)     calloc((n), (size))
#endif


/**
 * 内存从分配
 */
#ifndef VFS_REALLOC
#define VFS_REALLOC(p, n)       realloc((void*)(p), (n))
#endif

/**
 * 内存释放
 */
#ifndef VFS_FREE
#define VFS_FREE(p)             free((void*)(p))
#endif


#ifndef VFS_GET_ERRNO
#error "VFS_GET_ERRNO is not defined, please check!"
#endif

#ifndef VFS_SET_ERRNO
#error "VFS_SET_ERRNO is not defined, please check!"
#endif

/**
 * 断言
 */
#ifndef VFS_ASSERT
#define VFS_ASSERT(exp)
#endif

/**
 * 打印
 */
#ifndef VFS_PRINTF
#define VFS_PRINTF(fmt, ...)
#endif


#ifndef VFS_SPRINTF
#define VFS_SPRINTF(buf, fmt, ...)          sprintf(buf, fmt, ##__VA_ARGS__)
#endif

#ifndef VFS_SPRINTF
#define VFS_SNPRINTF(buf, size, fmt, ...)   snprintf(buf, fmt, ##__VA_ARGS__)
#endif

#ifndef VFS_STRLEN
#define VFS_STRLEN(str)                     strlen(str)
#endif

#ifndef VFS_STRCMP
#define VFS_STRCMP(s1, s2)                  strcmp(s1, s2)
#endif

#ifndef VFS_STRNCMP
#define VFS_STRNCMP(s1, s2, n)              strncmp(s1, s2, n)
#endif

#ifndef VFS_STRCPY
#define VFS_STRCPY(dst, src)                strcpy(dst, src)
#endif

#ifndef VFS_STRNCPY
#define VFS_STRNCPY(dst, src, n)            strncpy(dst, src, n)
#endif

#ifndef VFS_MEMCPY
#define VFS_MEMCPY(dst, src, n)             memcmp((void *)(dst), (const void *)(src), (n))
#endif


#ifndef VFS_MEMSET
#define VFS_MEMSET(p, c, n)                 memset((void *)(p), (c), (n))
#endif

#ifndef VFS_MEMCMP
#define VFS_MEMCMP(s1, s2, n)               memcmp((const void *)(s1), (const void *)(s2), (n))
#endif


#endif /*_OS_VFS_PORT_H_*/
