/*
@module  lvgl.obj
@summary LVGL对象系统(OO风格)
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建按钮(返回实例对象)
local btn = lvgl.btn.create(scr)

-- 使用OO风格的方法调用
btn:set_size(100, 40)
btn:set_pos(50, 50)
btn:set_text("Click Me")
btn:set_click(true)
btn:center()

-- 链式调用示例
local label = lvgl.label.create(scr)
label:set_size(200, 30)
label:set_pos(50, 120)
label:set_text("Hello World")
label:set_align(lvgl.TEXT_ALIGN_CENTER)

-- 获取对象指针
local ptr = btn:get_ptr()

-- 添加样式
local style = lvgl.style.create()
style:set_radius(10)
style:set_bg_color(0x3366FF)
btn:add_style(style)

-- 模块方式调用(直接操作)
lvgl.obj.set_x(btn, 10)
lvgl.obj.set_y(btn, 20)
*/

#ifndef LVGL_OBJ_H
#define LVGL_OBJ_H

#include "lua.h"
#include "lauxlib.h"
#include "lvgl_port.h"
#include "iot_callback.h"

/* ==================== 宏定义 ==================== */

/*
注册方法到表
@param L Lua状态
@param name 方法名
@param func C函数
@example
REG_METHOD(L, "set_size", lvgl_obj_set_size)
*/
#define REG_METHOD(L, name, func) \
    lua_pushcfunction(L, func); \
    lua_setfield(L, -2, name)

/*
注册只读属性到表
@param L Lua状态
@param name 属性名
@param value 属性值
*/
#define REG_READONLY(L, name, value) \
    lua_pushinteger(L, value); \
    lua_setfield(L, -2, name)

/*
注册常量到表
@param L Lua状态
@param name 常量名
@param value 常量值
*/
#define REG_CONSTANT(L, name, value) \
    lua_pushinteger(L, value); \
    lua_setfield(L, -2, name)

/* ==================== 核心函数 ==================== */

/*
获取对象的lv_obj_t指针
@param L Lua状态
@param idx 参数索引(1-based),如果是负数则相对于栈顶
@return lv_obj_t* 对象指针,如果是表则返回其obj字段的值,如果是userdata则直接返回
@note 当idx=1时,如果参数是表(OO风格调用如lvgl.btn.create),会跳过表并检查后续参数
@error 获取不到对象时会抛出Lua错误,由虚拟机内部pcall处理
*/
lv_obj_t* lvgl_get_obj_ptr(lua_State* L, int idx);

/*
注册对象系统(创建obj子表和基础metatable)
@param L Lua状态
@return int 基础metatable在注册表中的引用ID
@note 必须在所有组件注册之前调用,创建 lvgl.obj 子表
*/
int lvgl_register_obj(lua_State* L);

/*
获取对象metatable引用
@return int 注册表引用ID
*/
int lvgl_get_obj_metatable_ref(void);

/*
创建对象实例(通用)
@param L Lua状态
@param create_func 实际创建对象的函数
@param metatable_ref 组件专属metatable引用(可以是LUA_NOREF表示只有基础方法)
@return userdata 带方法的对象实例
*/
int lvgl_obj_create_instance(lua_State* L, lua_CFunction create_func, int metatable_ref);

/* ==================== 对象方法(供内部使用) ==================== */

/* 删除对象 */
int lvgl_obj_delete(lua_State* L);
/* 清空对象 */
int lvgl_obj_clean(lua_State* L);
/* 获取父对象 */
int lvgl_obj_get_parent(lua_State* L);
/* 获取子对象数量 */
int lvgl_obj_get_child_cnt(lua_State* L);
/* 设置位置 */
int lvgl_obj_set_pos(lua_State* L);
/* 设置尺寸 */
int lvgl_obj_set_size(lua_State* L);
/* 设置宽度 */
int lvgl_obj_set_width(lua_State* L);
/* 设置高度 */
int lvgl_obj_set_height(lua_State* L);
/* 设置X坐标 */
int lvgl_obj_set_x(lua_State* L);
/* 设置Y坐标 */
int lvgl_obj_set_y(lua_State* L);
/* 对齐对象 */
int lvgl_obj_align(lua_State* L);
/* 相对另一对象对齐 */
int lvgl_obj_align_to(lua_State* L);
/* 居中对齐 */
int lvgl_obj_center(lua_State* L);
/* 设置可点击 */
int lvgl_obj_set_click(lua_State* L);
/* 设置隐藏 */
int lvgl_obj_set_hidden(lua_State* L);
/* 设置用户数据 */
int lvgl_obj_set_user_data(lua_State* L);
/* 添加样式 */
int lvgl_obj_add_style(lua_State* L);
/* 添加到父对象 */
int lvgl_obj_add_to(lua_State* L);
/* 移动到前台 */
int lvgl_obj_move_foreground(lua_State* L);
/* 移动到后台 */
int lvgl_obj_move_background(lua_State* L);
/* 获取对象指针 */
int lvgl_obj_get_ptr(lua_State* L);
/* 获取X坐标 */
int lvgl_obj_get_x(lua_State* L);
/* 获取Y坐标 */
int lvgl_obj_get_y(lua_State* L);
/* 获取宽度 */
int lvgl_obj_get_width(lua_State* L);
/* 获取高度 */
int lvgl_obj_get_height(lua_State* L);
/* 获取位置 */
int lvgl_obj_get_pos(lua_State* L);
/* 是否可见 */
int lvgl_obj_is_visible(lua_State* L);
/* 是否可点击 */
int lvgl_obj_is_clickable(lua_State* L);
/* 获取对象类型 */
int lvgl_obj_get_type(lua_State* L);

/* 添加事件回调 */
int lvgl_obj_add_event_cb(lua_State* L);
/* 删除事件回调 */
int lvgl_obj_remove_event_cb(lua_State* L);
/* 设置事件回调 */
int lvgl_obj_set_event_cb(lua_State* L);

/* 滚动操作 */
int lvgl_obj_scroll_to_x(lua_State* L);
int lvgl_obj_scroll_to_y(lua_State* L);
int lvgl_obj_scroll_to(lua_State* L);
int lvgl_obj_scroll_by(lua_State* L);
int lvgl_obj_scroll_to_view(lua_State* L);
int lvgl_obj_get_scroll_x(lua_State* L);
int lvgl_obj_get_scroll_y(lua_State* L);
int lvgl_obj_set_scrollbar_mode(lua_State* L);
int lvgl_obj_set_scroll_dir(lua_State* L);

/* ==================== 样式相关函数 ==================== */

/*
从表创建样式
@param L Lua状态
@return userdata 样式指针
@example
local style = lvgl.style.create({width=100, height=50, radius=10, bg_color=0x3366FF})
*/
lv_style_t* lvgl_style_create_from_table(lua_State* L);

/*
设置样式属性(通用)
@param style 样式指针
@param prop 属性名
@param value 属性值
*/
void lvgl_style_set_prop(lua_State* L, lv_style_t* style, const char* prop, int value);

#endif /* LVGL_OBJ_H */