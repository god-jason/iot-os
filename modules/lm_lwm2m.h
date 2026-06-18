/*
@module  lwm2m
@summary LwM2M物联网协议模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     LWM2M
*/

#ifndef __IOT_LWM2M_H__
#define __IOT_LWM2M_H__

#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

LUAMOD_API int luaopen_lwm2m(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif /* __IOT_LWM2M_H__ */