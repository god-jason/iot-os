/**
 *************************************************************************************
 * 版权所有 (C) 2023, 芯昇科技有限公司
 * 保留所有权利。
 *
 * @file        oss_types.h
 *
 * @brief       数据类型扩展头文件.
 *
 * @revision
 *
 * 日期           作者              修改内容
 * 2023-07-07   ICT Team        创建
 ************************************************************************************
 */

#ifndef _OSS_TYPES_H_
#define _OSS_TYPES_H_

#include  <stdint.h>

#ifndef _OS_WIN
#ifndef FALSE
#define FALSE   (0)
#endif
#ifndef TRUE
#define TRUE    (1)
#endif
#endif


#ifndef OSS_SUCCESS
#define OSS_SUCCESS              0x0
#endif
#ifndef OSS_ERROR
#define OSS_ERROR                0xffffffff
#endif


#ifdef WIN32
typedef unsigned int        BOOL;
#else
typedef unsigned char                 BOOL;
#endif

#ifndef VOID
typedef void                VOID;
#endif
/* char不同编译器的符号类型不一样，所以char和CHAR只能用于字符串，否则可能会有问题 */
typedef char                CHAR;

typedef unsigned char       BYTE;     //8bits

typedef signed char         SINT8;
typedef unsigned char       UINT8;
typedef signed short        SINT16;
typedef unsigned short      UINT16;
typedef signed int          SINT32;
typedef unsigned int        UINT32;
typedef signed long long    SINT64;
typedef unsigned long long  UINT64;

typedef unsigned int        SIGSELECT;
typedef unsigned int        OSBUFSIZE;
typedef unsigned int        POINTER32;

typedef float               FLOAT;
typedef double              DOUBLE;

#endif
