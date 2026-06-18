/*
 * Copyright (C) 2019 Innochip-tech Co,. Ltd
 * Author  : geanfeng
 * Date    : 2017.10.12
 * Version : $Revision: 1.0
 * Purpose : Describe the main process.
 */

#ifndef __RISCV_CACHE_H_
#define __RISCV_CACHE_H_

#define CONFIG_USING_L1CACHE

extern void CleanAllDCache (void);
extern void enable_caches(void);
extern void cleanup_before_start(void);
extern void InvalidAllDCache (void);
extern void CleanAndInvalidAllDCache (void);
extern void InvalidAllICache (void);

#endif /* __CACHE_H_ */
