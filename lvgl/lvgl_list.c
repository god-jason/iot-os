/*
@module  lvgl.list
@summary LVGL列表控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建列表
local list = lvgl.list.create(scr)
list:set_size(200, 300)
list:set_pos(50, 50)

-- 添加列表项按钮
local btn1 = list:add_btn(nil, "文件")
local btn2 = list:add_btn(nil, "编辑")
local btn3 = list:add_btn(nil, "设置")

-- 设置列表方向
list:set_direction(lvgl.DIR_TOP)

-- 获取选中项
local sel = list:get_selected_btn()
if sel then
    print("选中项:", sel:get_text())
end

-- 链式调用
local list2 = lvgl.list.create(scr):set_size(150, 200):set_pos(280, 50):set_direction(lvgl.DIR_LEFT)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* list组件的metatable引用 */
static int list_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_list_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* list = lv_list_create(parent);
    lua_pushlightuserdata(L, list);
    return 1;
}

/* ==================== 列表OO方法 ==================== */

/*
创建列表控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的列表实例
@usage local list = lvgl.list.create(scr)
*/
static int lvgl_list_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_list_create_internal, list_metatable_ref);
}

/*
添加列表项按钮
@param self 列表实例或指针
@param img 图标(可选)
@param txt 按钮文本
@return userdata 按钮对象
@usage local btn = list:add_btn(nil, "选项")
*/
static int lvgl_list_add_btn(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* img = (lv_obj_t*)luaL_optlightuserdata(L, 2, NULL);
    const char* txt = luaL_checkstring(L, 3);
    lv_obj_t* btn = lv_list_add_btn(list, img, txt);
    lua_pushlightuserdata(L, btn);
    return 1;
}

/*
获取列表容器
@param self 列表实例或指针
@return userdata 容器对象
@usage local cont = list:get_container()
*/
static int lvgl_list_get_container(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* cont = lv_list_get_container(list);
    lua_pushlightuserdata(L, cont);
    return 1;
}

/*
获取选中的按钮
@param self 列表实例或指针
@return userdata 按钮对象或nil
@usage local sel = list:get_selected_btn()
*/
static int lvgl_list_get_selected_btn(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* btn = lv_list_get_selected_btn(list);
    if (btn) {
        lua_pushlightuserdata(L, btn);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/*
设置列表方向
@param self 列表实例或指针
@param dir 方向(lvgl.DIR_TOP等)
@return self
@usage list:set_direction(lvgl.DIR_TOP)
*/
static int lvgl_list_set_direction(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lv_dir_t dir = (lv_dir_t)luaL_checkinteger(L, 2);
    lv_list_set_direction(list, dir);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置选中项
@param self 列表实例或指针
@param sel 按钮对象
@return self
@usage list:set_selected(btn)
*/
static int lvgl_list_set_selected(lua_State* L) {
    lv_obj_t* list = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* sel = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_list_set_selected(list, sel);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 list 子模块 */
void lvgl_register_list(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "add_btn", lvgl_list_add_btn);
    REG_METHOD(L, "get_container", lvgl_list_get_container);
    REG_METHOD(L, "get_selected_btn", lvgl_list_get_selected_btn);
    REG_METHOD(L, "set_direction", lvgl_list_set_direction);
    REG_METHOD(L, "set_selected", lvgl_list_set_selected);

    /* 保存组件metatable引用(用于继承) */
    list_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.list.add_btn(list, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, list_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.list) */
    REG_METHOD(L, "create", lvgl_list_create);
}
