/**
 * @file iot_camera.h
 * @brief 相机模块
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_CAMERA_H
#define IOT_CAMERA_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_camera(lua_State* L);

#endif /* IOT_CAMERA_H */
