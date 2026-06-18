/**
 * @file iot_socket.h
 * @brief Socket网络
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_SOCKET_H
#define IOT_SOCKET_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_socket(lua_State* L);

#endif /* IOT_SOCKET_H */
