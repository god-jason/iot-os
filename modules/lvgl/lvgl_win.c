/*
@module  lvgl.win
@summary LVGL窗口控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建窗口
local win = lvgl.win.create(scr)
win:set_size(300, 200)
win:set_pos(10, 10)

-- 设置窗口标题
win:set_title("设置")

-- 添加标题栏按钮
local btn = win:add_btn(nil)  -- nil表示只有图标
local btn2 = win:add_btn("OK")

-- 设置按钮标题
win:set_btn_title(btn, "X")

-- 获取内容区域
local cont = win:get_content()

-- 添加内容到窗口
local label = lvgl.label.create(cont)
label:set_text("这是一个窗口的内容区域")

-- 链式调用
local w = lvgl.win.create(scr):set_size(280, 180):set_pos(20, 30):set_title("MyWindow")
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* win组件的metatable引用 */
static int win_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_win_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* win = lv_win_create(parent);
    lua_pushlightuserdata(L, win);
    return 1;
}

/* ==================== 窗口OO方法 ==================== */

/*
创建窗口控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的窗口实例
@usage local win = lvgl.win.create(scr)
*/
static int lvgl_win_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_win_create_internal, win_metatable_ref);
}

/*
设置窗口标题
@param self 窗口实例或指针
@param title 标题文本
@return self
@usage win:set_title("设置")
*/
static int lvgl_win_set_title(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    const char* title = luaL_checkstring(L, 2);
    lv_win_set_title(win, title);
    lua_pushvalue(L, 1);
    return 1;
}

/*
添加标题栏按钮
@param self 窗口实例或指针
@param icon 图标(可选)
@return userdata 按钮对象
@usage local btn = win:add_btn(nil)
*/
static int lvgl_win_add_btn(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    const char* icon = (const char*)luaL_optlightuserdata(L, 2, NULL);
    lv_obj_t* btn = lv_win_add_btn(win, icon);
    lua_pushlightuserdata(L, btn);
    return 1;
}

/*
设置按钮标题
@param self 窗口实例或指针
@param btn 按钮对象
@param title 标题文本
@return self
@usage win:set_btn_title(btn, "X")
*/
static int lvgl_win_set_btn_title(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* btn = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    const char* title = luaL_checkstring(L, 3);
    lv_win_set_btn_title(win, btn, title);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取内容区域
@param self 窗口实例或指针
@return userdata 内容区域对象
@usage local cont = win:get_content()
*/
static int lvgl_win_get_content(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* cont = lv_win_get_content(win);
    lua_pushlightuserdata(L, cont);
    return 1;
}

/*
设置标题栏高度
@param self 窗口实例或指针
@param height 高度值
@return self
@usage win:set_title_height(40)
*/
static int lvgl_win_set_title_height(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    uint16_t height = (uint16_t)luaL_checkinteger(L, 2);
    lv_win_set_title_height(win, height);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置窗口标志
@param self 窗口实例或指针
@param flags 标志值
@return self
@usage win:set_flags(lvgl.WIN_FLAG_HEADER)
*/
static int lvgl_win_set_flags(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    lv_win_flags_t flags = (lv_win_flags_t)luaL_checkinteger(L, 2);
    lv_win_set_flags(win, flags);
    lua_pushvalue(L, 1);
    return 1;
}

/*
清除窗口标志
@param self 窗口实例或指针
@param flags 标志值
@return self
@usage win:clear_flags(lvgl.WIN_FLAG_HEADER)
*/
static int lvgl_win_clear_flags(lua_State* L) {
    lv_obj_t* win = lvgl_get_obj_ptr(L, 1);
    lv_win_flags_t flags = (lv_win_flags_t)luaL_checkinteger(L, 2);
    lv_win_clear_flags(win, flags);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 win 子模块 */
void lvgl_register_win(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_title", lvgl_win_set_title);
    REG_METHOD(L, "add_btn", lvgl_win_add_btn);
    REG_METHOD(L, "set_btn_title", lvgl_win_set_btn_title);
    REG_METHOD(L, "get_content", lvgl_win_get_content);
    REG_METHOD(L, "set_title_height", lvgl_win_set_title_height);
    REG_METHOD(L, "set_flags", lvgl_win_set_flags);
    REG_METHOD(L, "clear_flags", lvgl_win_clear_flags);

    /* 保存组件metatable引用(用于继承) */
    win_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.win.set_title(win, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, win_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.win) */
    REG_METHOD(L, "create", lvgl_win_create);
}
