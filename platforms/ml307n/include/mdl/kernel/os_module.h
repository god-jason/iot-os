/**
 *************************************************************************************
 * @copyright 版权所有 (C) 2023, 芯昇科技有限公司
 *            保留所有权利。
 *
 * @file      os_module.h
 *
 * @brief     os模块动态加载文件.
 *
 * @revision  1.0
 *
 * @par 修改日志:
 * <table>
 * <tr><th>Date        <th>Version   <th>Author     <th>Description
 * <tr><td>2023-06-21  <td>1.0       <td>ICT Team   <td>初始版本
 * </table>
 ************************************************************************************
 */

#ifndef __OS_MODULE_H__
#define __OS_MODULE_H__

#include <os_def.h>

struct osModuleSymtab
{
    void       *addr;
    const char *name;
};

#ifdef OS_USING_MODULE
#if defined(_MSC_VER)
#pragma section("RTMSymTab$f",read)
#define RTM_EXPORT(symbol)                                              \
__declspec(allocate("RTMSymTab$f"))const char __rtmsym_##symbol##_name[] = "__vs_rtm_"#symbol;
#define RTM_EXPORT_ALIAS(func, symbol)
#pragma comment(linker, "/merge:RTMSymTab=mytext")

#elif defined(__MINGW32__)
#define RTM_EXPORT(symbol)
#define RTM_EXPORT_ALIAS(func, symbol)

#else
#define RTM_EXPORT(symbol)                                              \
const char __rtmsym_##symbol##_name[] SECTION("RTMSymName") = #symbol;\
const struct osModuleSymtab __rtmsym_##symbol SECTION("RTMSymTab") =     \
{                                                                       \
    (void *)&symbol,                                                    \
    __rtmsym_##symbol##_name                                            \
};

#define RTM_EXPORT_ALIAS(func, symbol)                                  \
const char __rtmsym_##func##_name[] SECTION("RTMSymName") = #func;    \
const struct osModuleSymtab __rtmsym_##func SECTION("RTMSymTab") =       \
{                                                                       \
    (void *)&symbol,                                                    \
    __rtmsym_##func##_name                                              \
};

#endif

#else
#define RTM_EXPORT(symbol)
#endif

#endif
