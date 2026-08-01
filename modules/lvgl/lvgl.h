/**
 * @file lvgl.h
 * @brief LVGL图形库Lua绑定主头文件
 *
 * 定义LVGL Lua模块的注册函数声明，包括主模块入口、各子模块（按钮、标签、图片、线条、弧形、进度条、滑块、复选框、开关、下拉菜单、滚轮、文本区、图表、表格、仪表盘、列表、菜单、窗口、选项卡、平铺视图、数值框、日历、色轮、键盘、消息框、旋转器、样式、动画、颜色、字体、显示、输入设备、定时器、主题、文件系统、画布、布局、分组等）的注册函数，以及常量定义函数。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#ifndef IOT_LVGL_H

#define IOT_LVGL_H



#include "lua.h"

#include "lauxlib.h"

#include "lvgl_port.h"

#include "lvgl_obj.h"



/* 主模块注册函数 */

LUAMOD_API int luaopen_lvgl(lua_State* L);



/* 子模块注册函数 */

void iot_lvgl_register_btn(lua_State* L);

void iot_lvgl_register_label(lua_State* L);

void iot_lvgl_register_img(lua_State* L);

void iot_lvgl_register_line(lua_State* L);

void iot_lvgl_register_arc(lua_State* L);

void iot_lvgl_register_bar(lua_State* L);

void iot_lvgl_register_slider(lua_State* L);

void iot_lvgl_register_checkbox(lua_State* L);

void iot_lvgl_register_switch(lua_State* L);

void iot_lvgl_register_dropdown(lua_State* L);

void iot_lvgl_register_roller(lua_State* L);

void iot_lvgl_register_textarea(lua_State* L);

void iot_lvgl_register_chart(lua_State* L);

void iot_lvgl_register_table(lua_State* L);

void iot_lvgl_register_meter(lua_State* L);

void iot_lvgl_register_list(lua_State* L);

void iot_lvgl_register_menu(lua_State* L);

void iot_lvgl_register_win(lua_State* L);

void iot_lvgl_register_tabview(lua_State* L);

void iot_lvgl_register_tileview(lua_State* L);

void iot_lvgl_register_spinbox(lua_State* L);

void iot_lvgl_register_calendar(lua_State* L);

void iot_lvgl_register_colorwheel(lua_State* L);

void iot_lvgl_register_keyboard(lua_State* L);

void iot_lvgl_register_msgbox(lua_State* L);

void iot_lvgl_register_spinner(lua_State* L);

void iot_lvgl_register_style(lua_State* L);

void iot_lvgl_register_anim(lua_State* L);

void iot_lvgl_register_color(lua_State* L);

void iot_lvgl_register_font(lua_State* L);

void iot_lvgl_register_disp(lua_State* L);

void iot_lvgl_register_indev(lua_State* L);

void iot_lvgl_register_timer(lua_State* L);

int iot_lvgl_register_theme(lua_State* L);

int iot_lvgl_register_fs(lua_State* L);

void iot_lvgl_register_canvas(lua_State* L);

void iot_lvgl_register_layout(lua_State* L);

void iot_lvgl_register_group(lua_State* L);



/* 常量定义函数 */

void iot_lvgl_define_constants(lua_State* L);



/* 注意：iot_lvgl_define_constants定义在lvgl_constants.c中 */



#endif /* IOT_LVGL_H */

