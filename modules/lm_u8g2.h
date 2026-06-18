/*
@module  u8g2
@summary 轻量级图形库封装
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     U8G2
*/

#ifndef __IOT_U8G2_H__
#define __IOT_U8G2_H__

#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

LUAMOD_API int luaopen_u8g2(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif /* __IOT_U8G2_H__ */