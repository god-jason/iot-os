/*
@module  lvgl.spinner
@summary LVGL旋转器控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建旋转器
local spinner = lvgl.spinner.create(scr, 1000)
spinner:set_size(50, 50)
spinner:set_pos(135, 95)

-- 设置旋转角度
spinner:set_angle(60)

-- 设置类型
spinner:set_type(lvgl.SPINNER_TYPE_SPINNING)

-- 暂停/恢复旋转
spinner:pause()
spinner:resume()

-- 链式调用
local s = lvgl.spinner.create(scr, 1000):set_size(40, 40):set_pos(140, 100):set_angle(60)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* spinner组件的metatable引用 */
static int spinner_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_spinner_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    uint32_t period = (uint32_t)luaL_optinteger(L, 2, 1000);
    lv_obj_t* spinner = lv_spinner_create(parent, period);
    lua_pushlightuserdata(L, spinner);
    return 1;
}

/* ==================== 旋转器OO方法 ==================== */

static int lvgl_spinner_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_spinner_create_internal, spinner_metatable_ref);
}

static int lvgl_spinner_set_angle(lua_State* L) {
    lv_obj_t* spinner = lvgl_get_obj_ptr(L, 1);
    uint16_t angle = (uint16_t)luaL_checkinteger(L, 2);
    lv_spinner_set_angle(spinner, angle);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinner_set_type(lua_State* L) {
    lv_obj_t* spinner = lvgl_get_obj_ptr(L, 1);
    lv_spinner_type_t type = (lv_spinner_type_t)luaL_checkinteger(L, 2);
    lv_spinner_set_type(spinner, type);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinner_pause(lua_State* L) {
    lv_obj_t* spinner = lvgl_get_obj_ptr(L, 1);
    lv_spinner_pause(spinner);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinner_resume(lua_State* L) {
    lv_obj_t* spinner = lvgl_get_obj_ptr(L, 1);
    lv_spinner_resume(spinner);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 spinner 子模块 */
void lvgl_register_spinner(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_angle", lvgl_spinner_set_angle);
    REG_METHOD(L, "set_type", lvgl_spinner_set_type);
    REG_METHOD(L, "pause", lvgl_spinner_pause);
    REG_METHOD(L, "resume", lvgl_spinner_resume);

    /* 保存组件metatable引用(用于继承) */
    spinner_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.spinner.set_angle(sp, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, spinner_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.spinner) */
    REG_METHOD(L, "create", lvgl_spinner_create);
}
