/**
 * @file iot_pack.h
 * @brief 数据打包/解包模块
 *
 * 本模块提供Lua字符串与C结构体之间的二进制数据打包/解包功能，
 * 支持大小端编码、多种数据类型（整数、浮点、字符串等）的格式化处理。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_PACK_H
#define IOT_PACK_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_pack_register(lua_State* L);

#endif /* IOT_PACK_H */
