/*
@module  lvgl.keyboard
@summary LVGL键盘控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建键盘
local kb = lvgl.keyboard.create(scr)
kb:set_size(300, 200)
kb:set_pos(0, 200)

-- 设置关联的文本区域
local ta = lvgl.textarea.create(scr)
ta:set_size(280, 50)
ta:set_pos(10, 10)
kb:set_textarea(ta)

-- 设置键盘模式
kb:set_mode(lvgl.KEYBOARD_MODE_TEXT)

-- 获取关联的文本区域
local ta2 = kb:get_textarea()
local mode = kb:get_mode()

-- 链式调用
local kb2 = lvgl.keyboard.create(scr):set_size(300, 150):set_pos(0, 250):set_mode(lvgl.KEYBOARD_MODE_NUMBER)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* keyboard组件的metatable引用 */
static int keyboard_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_keyboard_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* kb = lv_keyboard_create(parent);
    lua_pushlightuserdata(L, kb);
    return 1;
}

/* ==================== 键盘OO方法 ==================== */

/*
创建键盘控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的键盘实例
@usage local kb = lvgl.keyboard.create(scr)
*/
static int lvgl_keyboard_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_keyboard_create_internal, keyboard_metatable_ref);
}

/*
设置关联的文本区域
@param self 键盘实例或指针
@param ta 文本区域对象
@return self
@usage kb:set_textarea(ta)
*/
static int lvgl_keyboard_set_textarea(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* ta = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_keyboard_set_textarea(kb, ta);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置键盘模式
@param self 键盘实例或指针
@param mode 键盘模式(lvgl.KEYBOARD_MODE_TEXT等)
@return self
@usage kb:set_mode(lvgl.KEYBOARD_MODE_TEXT)
*/
static int lvgl_keyboard_set_mode(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lv_keyboard_mode_t mode = (lv_keyboard_mode_t)luaL_checkinteger(L, 2);
    lv_keyboard_set_mode(kb, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置键盘映射(暂未实现)
@param self 键盘实例或指针
@return self
@usage kb:set_map(map)
*/
static int lvgl_keyboard_set_map(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取关联的文本区域
@param self 键盘实例或指针
@return userdata 文本区域对象
@usage local ta = kb:get_textarea()
*/
static int lvgl_keyboard_get_textarea(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* ta = lv_keyboard_get_textarea(kb);
    lua_pushlightuserdata(L, ta);
    return 1;
}

/*
获取键盘模式
@param self 键盘实例或指针
@return integer 键盘模式
@usage local mode = kb:get_mode()
*/
static int lvgl_keyboard_get_mode(lua_State* L) {
    lv_obj_t* kb = lvgl_get_obj_ptr(L, 1);
    lv_keyboard_mode_t mode = lv_keyboard_get_mode(kb);
    lua_pushinteger(L, mode);
    return 1;
}

/* 注册 keyboard 子模块 */
void lvgl_register_keyboard(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_textarea", lvgl_keyboard_set_textarea);
    REG_METHOD(L, "set_mode", lvgl_keyboard_set_mode);
    REG_METHOD(L, "set_map", lvgl_keyboard_set_map);
    REG_METHOD(L, "get_textarea", lvgl_keyboard_get_textarea);
    REG_METHOD(L, "get_mode", lvgl_keyboard_get_mode);

    /* 保存组件metatable引用(用于继承) */
    keyboard_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.keyboard.set_textarea(kb, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, keyboard_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.keyboard) */
    REG_METHOD(L, "create", lvgl_keyboard_create);
}
