/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018/08/11     Bernard      the first version
 */

#ifndef RT_DL_MODULE_H__
#define RT_DL_MODULE_H__

#include <os.h>

#define RT_DLMODULE_STAT_INIT       0x00
#define RT_DLMODULE_STAT_RUNNING    0x01
#define RT_DLMODULE_STAT_CLOSING    0x02
#define RT_DLMODULE_STAT_CLOSED     0x03

struct osDlmodule;
typedef void* rt_addr_t;

typedef void (*rt_dlmodule_init_func_t)(struct osDlmodule *module);
typedef void (*rt_dlmodule_cleanup_func_t)(struct osDlmodule *module);
typedef int  (*rt_dlmodule_entry_func_t)(int argc, char** argv);

struct osDlmodule
{
    struct osObject parent;
    osList_t object_list;  /* objects inside this module */

    uint8_t stat;        /* status of module */

    /* main thread of this module */
    uint16_t priority;
    uint32_t stack_size;
    struct osThread *main_thread;
    /* the return code */
    int ret_code;

    /* VMA base address for the first LOAD segment */
    uint32_t vstart_addr;

    /* module entry, OS_NULL for dynamic library */
    rt_dlmodule_entry_func_t  entry_addr;
    char *cmd_line;         /* command line */

    rt_addr_t   mem_space;  /* memory space */
    uint32_t mem_size;   /* sizeof memory space */

    /* init and clean function */
    rt_dlmodule_init_func_t     init_func;
    rt_dlmodule_cleanup_func_t  cleanup_func;

    uint16_t nref;       /* reference count */

    uint16_t nsym;       /* number of symbols in the module */
    struct osModuleSymtab *symtab;    /* module symbol table */
};

struct osDlmodule *dlmodule_create(void);
osStatus_t dlmodule_destroy(struct osDlmodule* module);

struct osDlmodule *dlmodule_self(void);

osStatus_t dlmodule_run(struct osDlmodule *module, const char* cmd, uint32_t stackSize);

struct osDlmodule *dlmodule_load(const char* pgname);
struct osDlmodule *dlmodule_exec(const char* pgname, const char* cmd);

struct osDlmodule *dlimage_load(const char* part, uint32_t offset);
struct osDlmodule *dlimage_exec(const char* part, uint32_t offset, const char* cmd);

void dlmodule_exit(int ret_code);

struct osDlmodule *dlmodule_find(const char *name);

uint32_t dlmodule_symbol_find(const char *sym_str);

#endif
