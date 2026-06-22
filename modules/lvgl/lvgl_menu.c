/*
@module  lvgl.menu
@summary LVGL菜单控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建菜单
local menu = lvgl.menu.create(scr)
menu:set_size(320, 240)
menu:set_pos(0, 0)

-- 创建主页面
local main_page = menu:page_create(nil)
menu:set_page(main_page)

-- 添加主菜单项
local item1 = main_page:add_item(nil, "设置")
local item2 = main_page:add_item(nil, "关于")

-- 创建子页面
local settings_page = menu:page_create("设置")
settings_page:add_item(nil, "WiFi")
settings_page:add_item(nil, "蓝牙")

-- 设置主菜单项的子页面
item1:set_page(settings_page)

-- 获取当前页面
local cur_page = menu:get_cur_page()

-- 链式调用
local m = lvgl.menu.create(scr):set_size(240, 320):set_pos(40, 10):set_title("主菜单")
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* menu组件的metatable引用 */
static int menu_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_menu_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* menu = lv_menu_create(parent);
    lua_pushlightuserdata(L, menu);
    return 1;
}

/* ==================== 菜单OO方法 ==================== */

/*
创建菜单控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的菜单实例
@usage local menu = lvgl.menu.create(scr)
*/
static int lvgl_menu_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_menu_create_internal, menu_metatable_ref);
}

/*
创建菜单页面
@param self 菜单实例或指针
@param title 页面标题(可选)
@return userdata 页面对象
@usage local page = menu:page_create("设置")
*/
static int lvgl_menu_page_create(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    const char* title = (const char*)luaL_optlightuserdata(L, 2, NULL);
    lv_obj_t* page = lv_menu_page_create(menu, title);
    lua_pushlightuserdata(L, page);
    return 1;
}

/*
添加菜单项
@param self 页面实例或指针
@param icon 图标(可选)
@param text 文本内容
@return userdata 菜单项对象
@usage local item = page:add_item(nil, "设置")
*/
static int lvgl_menu_add_item(lua_State* L) {
    lv_obj_t* page = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* icon = (lv_obj_t*)luaL_optlightuserdata(L, 2, NULL);
    const char* text = luaL_checkstring(L, 3);
    lv_obj_t* item = lv_menu_add_item(page, icon, text);
    lua_pushlightuserdata(L, item);
    return 1;
}

/*
设置当前页面
@param self 菜单实例或指针
@param page 页面对象
@return self
@usage menu:set_page(main_page)
*/
static int lvgl_menu_set_page(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* page = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_menu_set_page(menu, page);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置菜单项的子页面
@param self 菜单项实例或指针
@param page 子页面对象
@return self
@usage item:set_page(settings_page)
*/
static int lvgl_menu_set_item_page(lua_State* L) {
    lv_obj_t* item = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* page = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_menu_set_item_page(item, page);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取当前页面
@param self 菜单实例或指针
@return userdata 页面对象
@usage local cur = menu:get_cur_page()
*/
static int lvgl_menu_get_cur_page(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* page = lv_menu_get_cur_page(menu);
    lua_pushlightuserdata(L, page);
    return 1;
}

/*
设置菜单宽度
@param self 菜单实例或指针
@param width 宽度值
@return self
@usage menu:set_width(200)
*/
static int lvgl_menu_set_width(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    uint16_t width = (uint16_t)luaL_checkinteger(L, 2);
    lv_menu_set_width(menu, width);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置菜单标题
@param self 菜单实例或指针
@param title 标题文本
@return self
@usage menu:set_title("主菜单")
*/
static int lvgl_menu_set_title(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    const char* title = luaL_checkstring(L, 2);
    lv_menu_set_title(menu, title);
    lua_pushvalue(L, 1);
    return 1;
}

/*
清除菜单内容
@param self 菜单实例或指针
@return self
@usage menu:clear()
*/
static int lvgl_menu_clear(lua_State* L) {
    lv_obj_t* menu = lvgl_get_obj_ptr(L, 1);
    lv_menu_clear(menu);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 menu 子模块 */
void lvgl_register_menu(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "page_create", lvgl_menu_page_create);
    REG_METHOD(L, "add_item", lvgl_menu_add_item);
    REG_METHOD(L, "set_page", lvgl_menu_set_page);
    REG_METHOD(L, "set_item_page", lvgl_menu_set_item_page);
    REG_METHOD(L, "get_cur_page", lvgl_menu_get_cur_page);
    REG_METHOD(L, "set_width", lvgl_menu_set_width);
    REG_METHOD(L, "set_title", lvgl_menu_set_title);
    REG_METHOD(L, "clear", lvgl_menu_clear);

    /* 保存组件metatable引用(用于继承) */
    menu_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.menu.add_item(menu, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, menu_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.menu) */
    REG_METHOD(L, "create", lvgl_menu_create);
}
