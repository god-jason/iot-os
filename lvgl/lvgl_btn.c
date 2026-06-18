/*
@module  lvgl.btn
@summary LVGL按钮控件
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

-- 设置可选中/切换
btn:set_checkable(true)
btn:toggle()

-- 设置布局
btn:set_layout(lvgl.LAYOUT_CENTER)

-- 链式调用
local btn2 = lvgl.btn.create(scr):set_size(100, 40):set_pos(160, 50):set_text("Button 2")
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* btn组件的metatable引用 */
static int btn_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

/* 实际创建按钮的函数(被OO包装器调用) */
static int lvgl_btn_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* btn = lv_btn_create(parent);
    lua_pushlightuserdata(L, btn);
    return 1;
}

/* ==================== 按钮OO方法 ==================== */

/*
创建按钮(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的按钮实例
@usage local btn = lvgl.btn.create(scr)
*/
static int lvgl_btn_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_btn_create_internal, btn_metatable_ref);
}

/*
设置按钮文本
@param self 按钮实例或指针
@param text 文本内容
@return self
@usage btn:set_text("OK")
*/
static int lvgl_btn_set_text(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_btn_set_text(btn, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取按钮文本
@param self 按钮实例或指针
@return string 文本内容
@usage local text = btn:get_text()
*/
static int lvgl_btn_get_text(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    const char* text = lv_btn_get_text(btn);
    lua_pushstring(L, text);
    return 1;
}

/*
设置按钮状态
@param self 按钮实例或指针
@param state 状态值
@return self
@usage btn:set_state(lvgl.BTN_STATE_PRESSED)
*/
static int lvgl_btn_set_state(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    lv_btn_state_t state = (lv_btn_state_t)luaL_checkinteger(L, 2);
    lv_btn_set_state(btn, state);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取按钮状态
@param self 按钮实例或指针
@return integer 状态值
@usage local state = btn:get_state()
*/
static int lvgl_btn_get_state(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    lv_btn_state_t state = lv_btn_get_state(btn);
    lua_pushinteger(L, state);
    return 1;
}

/*
切换按钮状态
@param self 按钮实例或指针
@return self
@usage btn:toggle()
*/
static int lvgl_btn_toggle(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    lv_btn_toggle(btn);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置是否可切换
@param self 按钮实例或指针
@param en 是否可切换
@return self
@usage btn:set_checkable(true)
*/
static int lvgl_btn_set_checkable(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_btn_set_checkable(btn, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置按钮布局
@param self 按钮实例或指针
@param layout 布局类型
@return self
@usage btn:set_layout(lvgl.LAYOUT_CENTER)
*/
static int lvgl_btn_set_layout(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    uint32_t layout = (uint32_t)luaL_checkinteger(L, 2);
    lv_btn_set_layout(btn, layout);
    lua_pushvalue(L, 1);
    return 1;
}

/*
清除按钮布局
@param self 按钮实例或指针
@return self
@usage btn:clear_layout()
*/
static int lvgl_btn_clear_layout(lua_State* L) {
    lv_obj_t* btn = lvgl_get_obj_ptr(L, 1);
    lv_btn_set_layout(btn, LV_LAYOUT_NONE);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 btn 子模块 */
void lvgl_register_btn(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);
    
    /* 注册OO风格方法(可以instance:method()调用) */
    REG_METHOD(L, "set_text", lvgl_btn_set_text);
    REG_METHOD(L, "get_text", lvgl_btn_get_text);
    REG_METHOD(L, "set_state", lvgl_btn_set_state);
    REG_METHOD(L, "get_state", lvgl_btn_get_state);
    REG_METHOD(L, "toggle", lvgl_btn_toggle);
    REG_METHOD(L, "set_checkable", lvgl_btn_set_checkable);
    REG_METHOD(L, "set_layout", lvgl_btn_set_layout);
    REG_METHOD(L, "clear_layout", lvgl_btn_clear_layout);

    /* 保存组件metatable引用(用于继承) */
    btn_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.btn.set_text(btn, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, btn_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.btn) */
    REG_METHOD(L, "create", lvgl_btn_create);
}
