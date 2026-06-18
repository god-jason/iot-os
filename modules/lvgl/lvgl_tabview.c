/*
@module  lvgl.tabview
@summary LVGL标签视图控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建标签视图
local tv = lvgl.tabview.create(scr)
tv:set_size(320, 240)
tv:set_pos(0, 0)

-- 设置标签位置
tv:set_tab_bar_position(lvgl.TABVIEW_TAB_POS_TOP)

-- 添加标签
local tab1 = tv:add_tab("页面1")
local tab2 = tv:add_tab("页面2")
local tab3 = tv:add_tab("页面3")

-- 获取标签页面
local page0 = tv:get_tab(0)
local page1 = tv:get_tab(1)

-- 在页面中添加内容
local label1 = lvgl.label.create(page0)
label1:set_text("这是页面1的内容")

-- 设置当前标签
tv:set_active(0, 0)  -- 无动画切换到第1个标签

-- 获取当前标签索引
local active = tv:get_active()

-- 链式调用
local tv2 = lvgl.tabview.create(scr):set_size(240, 320):set_pos(40, 10):add_tab("Tab1")
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* tabview组件的metatable引用 */
static int tabview_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_tabview_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* tv = lv_tabview_create(parent);
    lua_pushlightuserdata(L, tv);
    return 1;
}

/* ==================== 标签视图OO方法 ==================== */

/*
创建标签视图控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的标签视图实例
@usage local tv = lvgl.tabview.create(scr)
*/
static int lvgl_tabview_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_tabview_create_internal, tabview_metatable_ref);
}

/*
添加标签页
@param self 标签视图实例或指针
@param name 标签名称
@return userdata 标签页对象
@usage local tab1 = tv:add_tab("页面1")
*/
static int lvgl_tabview_add_tab(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    const char* name = luaL_checkstring(L, 2);
    lv_obj_t* tab = lv_tabview_add_tab(tv, name);
    lua_pushlightuserdata(L, tab);
    return 1;
}

/*
设置标签栏位置
@param self 标签视图实例或指针
@param pos 标签栏位置
@return self
@usage tv:set_tab_bar_position(lvgl.TABVIEW_TAB_POS_TOP)
*/
static int lvgl_tabview_set_tab_bar_position(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    lv_tabview_tab_pos_t pos = (lv_tabview_tab_pos_t)luaL_checkinteger(L, 2);
    lv_tabview_set_tab_bar_position(tv, pos);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置标签栏宽度
@param self 标签视图实例或指针
@param width 宽度值
@return self
@usage tv:set_tab_bar_width(100)
*/
static int lvgl_tabview_set_tab_bar_width(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    uint16_t width = (uint16_t)luaL_checkinteger(L, 2);
    lv_tabview_set_tab_bar_width(tv, width);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取标签页
@param self 标签视图实例或指针
@param idx 标签索引
@return userdata 标签页对象
@usage local page0 = tv:get_tab(0)
*/
static int lvgl_tabview_get_tab(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    uint8_t idx = (uint8_t)luaL_checkinteger(L, 2);
    lv_obj_t* tab = lv_tabview_get_tab(tv, idx);
    lua_pushlightuserdata(L, tab);
    return 1;
}

/*
设置当前激活标签
@param self 标签视图实例或指针
@param idx 标签索引
@param anim 是否动画(可选)
@return self
@usage tv:set_active(0, 0)
*/
static int lvgl_tabview_set_active(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    uint8_t idx = (uint8_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_tabview_set_active(tv, idx, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取当前激活标签索引
@param self 标签视图实例或指针
@return integer 标签索引
@usage local active = tv:get_active()
*/
static int lvgl_tabview_get_active(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    uint16_t active = lv_tabview_get_active(tv);
    lua_pushinteger(L, active);
    return 1;
}

/*
获取标签数量
@param self 标签视图实例或指针
@return integer 标签数量
@usage local count = tv:get_tab_count()
*/
static int lvgl_tabview_get_tab_count(lua_State* L) {
    lv_obj_t* tv = lvgl_get_obj_ptr(L, 1);
    uint16_t count = lv_tabview_get_tab_count(tv);
    lua_pushinteger(L, count);
    return 1;
}

/* 注册 tabview 子模块 */
void lvgl_register_tabview(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "add_tab", lvgl_tabview_add_tab);
    REG_METHOD(L, "set_tab_bar_position", lvgl_tabview_set_tab_bar_position);
    REG_METHOD(L, "set_tab_bar_width", lvgl_tabview_set_tab_bar_width);
    REG_METHOD(L, "get_tab", lvgl_tabview_get_tab);
    REG_METHOD(L, "set_active", lvgl_tabview_set_active);
    REG_METHOD(L, "get_active", lvgl_tabview_get_active);
    REG_METHOD(L, "get_tab_count", lvgl_tabview_get_tab_count);

    /* 保存组件metatable引用(用于继承) */
    tabview_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.tabview.add_tab(tv, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, tabview_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.tabview) */
    REG_METHOD(L, "create", lvgl_tabview_create);
}
