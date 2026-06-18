/*
@module  lvgl
@summary LVGL图形库Lua绑定(OO风格)
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
*/

#ifndef LUA_LVGL_H
#define LUA_LVGL_H

#include "lua.h"
#include "lauxlib.h"
#include "lvgl.h"
#include "lvgl_port.h"
#include "lvgl_obj.h"

/* 主模块注册函数 */
LUAMOD_API int luaopen_lvgl(lua_State* L);

/* 子模块注册函数 */
void lvgl_register_btn(lua_State* L);
void lvgl_register_label(lua_State* L);
void lvgl_register_img(lua_State* L);
void lvgl_register_line(lua_State* L);
void lvgl_register_arc(lua_State* L);
void lvgl_register_bar(lua_State* L);
void lvgl_register_slider(lua_State* L);
void lvgl_register_checkbox(lua_State* L);
void lvgl_register_switch(lua_State* L);
void lvgl_register_dropdown(lua_State* L);
void lvgl_register_roller(lua_State* L);
void lvgl_register_textarea(lua_State* L);
void lvgl_register_chart(lua_State* L);
void lvgl_register_table(lua_State* L);
void lvgl_register_meter(lua_State* L);
void lvgl_register_list(lua_State* L);
void lvgl_register_menu(lua_State* L);
void lvgl_register_win(lua_State* L);
void lvgl_register_tabview(lua_State* L);
void lvgl_register_tileview(lua_State* L);
void lvgl_register_spinbox(lua_State* L);
void lvgl_register_calendar(lua_State* L);
void lvgl_register_colorwheel(lua_State* L);
void lvgl_register_keyboard(lua_State* L);
void lvgl_register_msgbox(lua_State* L);
void lvgl_register_spinner(lua_State* L);
void lvgl_register_style(lua_State* L);
void lvgl_register_anim(lua_State* L);
void lvgl_register_color(lua_State* L);
void lvgl_register_font(lua_State* L);
void lvgl_register_disp(lua_State* L);
void lvgl_register_indev(lua_State* L);
void lvgl_register_timer(lua_State* L);
void lvgl_register_canvas(lua_State* L);
void lvgl_register_layout(lua_State* L);
void lvgl_register_group(lua_State* L);

/* 常量定义函数 */
void lvgl_define_constants(lua_State* L);

/* 注意：lvgl_define_constants定义在lvgl_constants.c中 */

#endif /* LUA_LVGL_H */
