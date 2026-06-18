/*
@module  font
@summary 字体渲染模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     FONT
*/

#ifndef __IOT_FONT_H__
#define __IOT_FONT_H__

#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

LUAMOD_API int luaopen_font(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif /* __IOT_FONT_H__ */