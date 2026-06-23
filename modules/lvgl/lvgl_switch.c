/*
@module  lvgl.switch
@summary LVGL开关控�?
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建开�?
local sw = lvgl.switch.create(scr)
sw:set_pos(50, 50)

-- 开关状�?
sw:on(1)     -- 打开(有动�?
sw:off(0)    -- 关闭(无动�?
sw:toggle(1)  -- 切换状�?

-- 获取状�?
if sw:get_state() then
    print("开关已打开")
else
    print("开关已关闭")
end

-- 链式调用
local sw2 = lvgl.switch.create(scr):set_pos(50, 100):on()
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* switch组件的metatable引用 */
static int switch_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_switch_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* sw = lv_switch_create(parent);
    lua_pushlightuserdata(L, sw);
    return 1;
}

/* ==================== 开关OO方法 ==================== */

static int lvgl_switch_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_switch_create_internal, switch_metatable_ref);
}

static int lvgl_switch_on(lua_State* L) {
    lv_obj_t* sw = lvgl_get_obj_ptr(L, 1);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 2, LV_ANIM_ON);
    lv_switch_on(sw, anim);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_switch_off(lua_State* L) {
    lv_obj_t* sw = lvgl_get_obj_ptr(L, 1);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 2, LV_ANIM_ON);
    lv_switch_off(sw, anim);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_switch_toggle(lua_State* L) {
    lv_obj_t* sw = lvgl_get_obj_ptr(L, 1);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 2, LV_ANIM_ON);
    lv_switch_toggle(sw, anim);
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_switch_get_state(lua_State* L) {
    lv_obj_t* sw = lvgl_get_obj_ptr(L, 1);
    bool state = lv_switch_get_state(sw);
    lua_pushboolean(L, state);
    return 1;
}

/* 注册 switch 子模�?*/
void lvgl_register_switch(lua_State* L) {
    /* 创建组件方法�?用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "on", lvgl_switch_on);
    REG_METHOD(L, "off", lvgl_switch_off);
    REG_METHOD(L, "toggle", lvgl_switch_toggle);
    REG_METHOD(L, "get_state", lvgl_switch_get_state);

    /* 保存组件metatable引用(用于继承) */
    switch_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.switch.on(sw, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, switch_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主�?lvgl.switch) */
    REG_METHOD(L, "create", lvgl_switch_create);
}
