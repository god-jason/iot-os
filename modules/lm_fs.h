/**
 * @file iot_fs.h
 * @brief 文件系统
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_FS_H
#define IOT_FS_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_fs(lua_State* L);

#endif /* IOT_FS_H */
