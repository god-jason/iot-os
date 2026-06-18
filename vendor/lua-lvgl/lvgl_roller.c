/*
@module  lvgl.roller
@summary LVGL滚轮控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建滚轮
local roller = lvgl.roller.create(scr)
roller:set_size(100, 150)
roller:set_pos(50, 50)

-- 设置选项
roller:set_options("Option1\nOption2\nOption3\nOption4", lvgl.ROLLER_MODE_NORMAL)

-- 设置选中项
roller:set_selected(1, 0)  -- 选中第二项,无动画

-- 设置可见行数
roller:set_visible_row_count(3)

-- 获取选中项
local sel = roller:get_selected()
local sel_str = roller:get_selected_str()

-- 链式调用
local roller2 = lvgl.roller.create(scr):set_size(100, 120):set_pos(160, 50):set_options("A\nB\nC\nD"):set_selected(0)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* roller组件的metatable引用 */
static int roller_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_roller_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* roller = lv_roller_create(parent);
    lua_pushlightuserdata(L, roller);
    return 1;
}

/* ==================== 滚轮OO方法 ==================== */

/*
创建滚轮控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的滚轮实例
@usage local roller = lvgl.roller.create(scr)
*/
static int lvgl_roller_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_roller_create_internal, roller_metatable_ref);
}

/*
设置滚轮选项
@param self 滚轮实例或指针
@param options 选项字符串(用\n分隔)
@param mode 模式(可选,默认NORMAL)
@return self
@usage roller:set_options("A\nB\nC", lvgl.ROLLER_MODE_NORMAL)
*/
static int lvgl_roller_set_options(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    const char* options = luaL_checkstring(L, 2);
    lv_roller_mode_t mode = (lv_roller_mode_t)luaL_optinteger(L, 3, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_options(roller, options, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置选中项
@param self 滚轮实例或指针
@param sel 选中项索引
@param anim 是否动画(可选)
@return self
@usage roller:set_selected(1, 0)
*/
static int lvgl_roller_set_selected(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    uint16_t sel = (uint16_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_roller_set_selected(roller, sel, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置可见行数
@param self 滚轮实例或指针
@param row_cnt 行数
@return self
@usage roller:set_visible_row_count(3)
*/
static int lvgl_roller_set_visible_row_count(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    uint8_t row_cnt = (uint8_t)luaL_checkinteger(L, 2);
    lv_roller_set_visible_row_count(roller, row_cnt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取选中项索引
@param self 滚轮实例或指针
@return integer 选中项索引
@usage local sel = roller:get_selected()
*/
static int lvgl_roller_get_selected(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    uint16_t sel = lv_roller_get_selected(roller);
    lua_pushinteger(L, sel);
    return 1;
}

static int lvgl_roller_get_selected_str(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    char buf[256];
    uint32_t len = (uint32_t)luaL_optinteger(L, 2, sizeof(buf));
    lv_roller_get_selected_str(roller, buf, len);
    lua_pushstring(L, buf);
    return 1;
}

/* 注册 roller 子模块 */
void lvgl_register_roller(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_options", lvgl_roller_set_options);
    REG_METHOD(L, "set_selected", lvgl_roller_set_selected);
    REG_METHOD(L, "set_visible_row_count", lvgl_roller_set_visible_row_count);
    REG_METHOD(L, "get_selected", lvgl_roller_get_selected);
    REG_METHOD(L, "get_selected_str", lvgl_roller_get_selected_str);

    /* 保存组件metatable引用(用于继承) */
    roller_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.roller.set_options(roller, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, roller_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.roller) */
    REG_METHOD(L, "create", lvgl_roller_create);
}
