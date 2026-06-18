/**
 * @file      nr_micro_shell_config.h
 * @author    Ji Youzhou
 * @version   V0.1
 * @date      28 Oct 2019
 * @brief     [brief]
 * *****************************************************************************
 * @attention
 * 
 * MIT License
 * 
 * Copyright (C) 2019 Ji Youzhou. or its affiliates.  All Rights Reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __nr_micro_shell_config_h
#define __nr_micro_shell_config_h

#ifdef __cplusplus
extern "C"
{
#endif


#include <os_config.h>
#include "os.h"

void osShellSetUart(void *hUart);
void* osShellGetUart(void);
void SHELL_Printf(const char *fmt, ...);
void SHELL_PutChar(const char c);

/* ANSI command line buffer size. */
#define NR_ANSI_LINE_SIZE 100

/* Maximum user name length. */
#define NR_SHELL_USER_NAME_MAX_LENGTH 10

/* Maximum command name length. */
#define NR_SHELL_CMD_NAME_MAX_LENGTH 16

/* Maximum console receive buffer length. */
#define NR_SHELL_MAX_RECVBUF_LENGTH  32

/* Minimum console receive buffer length. */
#define NR_SHELL_ONE_BYTE_LENGTH    1


/* Command line buffer size. */
#define NR_SHELL_CMD_LINE_MAX_LENGTH NR_ANSI_LINE_SIZE

/* The maximum number of parameters in the command. */
#define NR_SHELL_CMD_PARAS_MAX_NUM 10

/* Command stores the most history commands (the maximum number here refers to the maximum number of commands that can be stored. When the history command line cache is full, it will automatically release the earliest command record) */
#define NR_SHELL_MAX_CMD_HISTORY_NUM 3

/* History command cache length */
#define NR_SHELL_CMD_HISTORY_BUF_LENGTH 253

/* The user's name. */
#if defined(_CPU_AP)
#define NR_SHELL_USER_NAME "nr@ap #"
#else
#define NR_SHELL_USER_NAME "nr@cp #"
#endif

/*
0: \n
1: \r
2: \r\n
*/
#define NR_SHELL_END_OF_LINE 1

/* Weather the terminal support all ANSI codes. */
#define NR_SHLL_FULL_ANSI 1

/* Use NR_SHELL_CMD_EXPORT() or not */
#define NR_SHELL_USING_EXPORT_CMD


/* Show logo or not. */
//#define NR_SHELL_SHOW_LOG

/* Use history commands or not */
#define NR_SHELL_HISTORY_COMMAND

/* Use one character or 32 bytes array  */
#define NR_SHELL_RECV_ONE_CHAR


/* If you use RTOS, you may need to do some special processing for printf(). */
// void osPrintf(const char *fmt, ...)   @kservice.c
#define shell_printf                     SHELL_Printf
#define ansi_show_char(x)                SHELL_PutChar(x)


#ifdef __cplusplus
}
#endif

#endif
