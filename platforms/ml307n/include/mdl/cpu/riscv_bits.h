/**
 ***********************************************************************************************************************
 * Copyright (c) 2022,Nanjing InnoChip Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 *
 * @file        riscv_bits.h
 *
 * @brief       Header of riscv micro defination.
 *
 * @par History:
 * <table>
 * <tr><th>Date                <th> Author              <th> Notes
 * <tr><td>2022-04-06          <td> ICT Team            <td> First version
 * </table>
 *
 ***********************************************************************************************************************
 */


#ifndef _RISCV_BITS_H
#define _RISCV_BITS_H

#define likely(x)   __builtin_expect((x), 1)
#define unlikely(x) __builtin_expect((x), 0)

#define ROUNDUP(a, b)   ((((a)-1) / (b) + 1) * (b))
#define ROUNDDOWN(a, b) ((a) / (b) * (b))
#ifndef MAX
#define MAX(a, b)        ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a, b)        ((a) < (b) ? (a) : (b))
#endif
#define CLAMP(a, lo, hi) MIN(MAX(a, lo), hi)

#define EXTRACT_FIELD(val, which)          (((val) & (which)) / ((which) & ~((which)-1)))
#define INSERT_FIELD(val, which, fieldval) (((val) & ~(which)) | ((fieldval) * ((which) & ~((which)-1))))

#define STR(x)  XSTR(x)
#define XSTR(x) #x

#if __riscv_xlen == 64
#define SLL32        sllw
#define STORE        sd
#define LOAD         ld
#define LWU          lwu
#define LOG_REGBYTES 3
#else
#define SLL32        sll
#define STORE        sw
#define LOAD         lw
#define LWU          lw
#define LOG_REGBYTES 2
#endif
#define REGBYTES (1 << LOG_REGBYTES)

#endif
