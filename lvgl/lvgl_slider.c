/*
@module  lvgl.slider
@summary LVGL滑块控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建滑块
local slider = lvgl.slider.create(scr)
slider:set_size(200, 20)
slider:set_pos(50, 50)

-- 设置范围和值
slider:set_range(0, 100)
slider:set_value(50, 0)  -- 设置为50,无动画

-- 设置模式
slider:set_mode(lvgl.SLIDER_MODE_NORMAL)

-- 获取值
local value = slider:get_value()

-- 检查是否正在拖动
local dragging = slider:is_dragged()

-- 链式调用
local slider2 = lvgl.slider.create(scr):set_size(200, 20):set_pos(50, 100):set_range(0, 100):set_value(25)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* slider组件的metatable引用 */
static int slider_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_slider_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* slider = lv_slider_create(parent);
    lua_pushlightuserdata(L, slider);
    return 1;
}

/* ==================== 滑块OO方法 ==================== */

/*
创建滑块控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的滑块实例
@usage local slider = lvgl.slider.create(scr)
*/
static int lvgl_slider_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_slider_create_internal, slider_metatable_ref);
}

/*
设置滑块值
@param self 滑块实例或指针
@param value 值
@param anim 动画使能(可选,默认0=无动画)
@return self
@usage slider:set_value(50, 0)
*/
static int lvgl_slider_set_value(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    int32_t value = (int32_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_slider_set_value(slider, value, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置滑块范围
@param self 滑块实例或指针
@param min 最小值
@param max 最大值
@return self
@usage slider:set_range(0, 100)
*/
static int lvgl_slider_set_range(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    int32_t max = (int32_t)luaL_checkinteger(L, 3);
    lv_slider_set_range(slider, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置滑块模式
@param self 滑块实例或指针
@param mode 模式: SLIDER_MODE_NORMAL(0), SLIDER_MODE_SYMMETRICAL(1), SLIDER_MODE_REVERSE(2)
@return self
@usage slider:set_mode(lvgl.SLIDER_MODE_NORMAL)
*/
static int lvgl_slider_set_mode(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    lv_slider_mode_t mode = (lv_slider_mode_t)luaL_checkinteger(L, 2);
    lv_slider_set_mode(slider, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取滑块值
@param self 滑块实例或指针
@return integer 当前值
@usage local value = slider:get_value()
*/
static int lvgl_slider_get_value(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    int32_t value = lv_slider_get_value(slider);
    lua_pushinteger(L, value);
    return 1;
}

/*
检查滑块是否正在拖动
@param self 滑块实例或指针
@return boolean 是否拖动中
@usage local dragging = slider:is_dragged()
*/
static int lvgl_slider_is_dragged(lua_State* L) {
    lv_obj_t* slider = lvgl_get_obj_ptr(L, 1);
    bool dragged = lv_slider_is_dragged(slider);
    lua_pushboolean(L, dragged);
    return 1;
}

/* 注册 slider 子模块 */
void lvgl_register_slider(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_value", lvgl_slider_set_value);
    REG_METHOD(L, "get_value", lvgl_slider_get_value);
    REG_METHOD(L, "set_range", lvgl_slider_set_range);
    REG_METHOD(L, "set_mode", lvgl_slider_set_mode);
    REG_METHOD(L, "is_dragged", lvgl_slider_is_dragged);

    /* 保存组件metatable引用(用于继承) */
    slider_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.slider.set_value(slider, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, slider_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.slider) */
    REG_METHOD(L, "create", lvgl_slider_create);
}
