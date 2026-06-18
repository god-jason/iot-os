/*
@module  lcd
@summary LCD显示模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     LCD
*/

#ifndef __IOT_LCD_H__
#define __IOT_LCD_H__

#include "lua.h"

#ifdef __cplusplus
extern "C" {
#endif

LUAMOD_API int luaopen_lcd(lua_State* L);

#ifdef __cplusplus
}
#endif

#endif /* __IOT_LCD_H__ */