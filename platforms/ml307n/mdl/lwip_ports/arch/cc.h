/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#if !NO_SYS
#include <os.h>
#include "net_pub.h"
#endif

#define X8_F  "x"
#define U16_F "u"
#define S16_F "d"
#define X16_F "x"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"

#ifdef OS_USING_LIBC_ADAPTER
#if defined(__CC_ARM) || defined(__CLANG_ARM) || defined(__IAR_SYSTEMS_ICC__)
#include <sys/errno.h>
#else
#include <errno.h>
/* some errno not defined in newlib */
#define ENSRNOTFOUND 163 /* Domain name not found */
/* WARNING: ESHUTDOWN also not defined in newlib. We chose
            180 here because the number "108" which is used
            in arch.h has been assigned to another error code. */
#define ESHUTDOWN    180
#endif /* __CC_ARM/__IAR_SYSTEMS_ICC__ */
#else
#define LWIP_PROVIDE_ERRNO
#endif

#if defined(OS_USING_LIBC_ADAPTER)
#include <sys/time.h>
#define LWIP_TIMEVAL_PRIVATE 0
#else
#define LWIP_TIMEVAL_PRIVATE 1
#endif

#if defined(__CC_ARM) /* ARMCC compiler */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT   __attribute__((__packed__))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#elif defined(__IAR_SYSTEMS_ICC__) /* IAR Compiler */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_USE_INCLUDES
#elif defined(__GNUC__) /* GNU GCC Compiler */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT   __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#elif defined(_MSC_VER)
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#endif

void sys_arch_assert(const char* file, int line);

#define LWIP_PLATFORM_DIAG(x)                                                                                          \
    do                                                                                                                 \
        {                                                                                                              \
            MID_NET_PRINT_INFO x;                                                                                      \
        }                                                                                                              \
    while (0)

#define LWIP_PLATFORM_ASSERT(x)                                                                                        \
    do                                                                                                                 \
        {                                                                                                              \
            MID_NET_PRINT_INFO("\r\nLWIP_ASSERT: "x"\r\n");                                                            \
            osAssertHandler(__FILE__, __LINE__);                                                                       \
        }                                                                                                              \
    while (0)

#define SYS_ARCH_PROTECT(level)   osKernelLock()
#define SYS_ARCH_UNPROTECT(level) osKernelUnlock()
#define SYS_ARCH_DECL_PROTECT(level)


#endif /* __ARCH_CC_H__ */
