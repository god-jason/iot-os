/**
 * @file lvgl_win.c
 * @brief LVGL窗口控件
 *
 * 实现LVGL窗口控件的OO风格Lua绑定，包括窗口创建、设置标题、添加按钮、设置按钮标题、获取内容区域等接口，提供带标题栏和按钮的容器窗口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* win组件的metatable引用 */
static int win_metatable_ref = LUA_NOREF;

static lv_obj_t* lvgl_win_find_title(lv_obj_t* win)
{
    lv_obj_t* header = lv_win_get_header(win);
    uint32_t i;
    uint32_t cnt;

    if (!header) {
        return NULL;
    }
    cnt = lv_obj_get_child_cnt(header);
    for (i = 0; i < cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(header, i);
        if (lv_obj_check_type(child, &lv_label_class)) {
            return child;
        }
    }
    return NULL;
}

/* ==================== 内部创建函数 ==================== */

static int lvgl_win_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_coord_t header_height = (lv_coord_t)luaL_optinteger(L, 2, 40);
    lv_obj_t* win = lv_win_create(parent, header_height);
    lua_pushlightuserdata(L, win);
    return 1;
}

/* ==================== ??OO?? ==================== */

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
    lv_obj_t* label = lvgl_win_find_title(win);
    if (label) {
        lv_label_set_text(label, title);
    } else {
        lv_win_add_title(win, title);
    }
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
    const void* icon = NULL;
    if (!lua_isnoneornil(L, 2)) {
        if (lua_isstring(L, 2)) {
            icon = luaL_checkstring(L, 2);
        } else {
            icon = luaL_checklightuserdata(L, 2);
        }
    }
    lv_coord_t btn_w = (lv_coord_t)luaL_optinteger(L, 3, 40);
    lv_obj_t* btn = lv_win_add_btn(win, icon, btn_w);
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
    (void)win;
    lv_btn_set_text(btn, title);
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
    lv_coord_t height = (lv_coord_t)luaL_checkinteger(L, 2);
    lv_obj_t* header = lv_win_get_header(win);
    if (header) {
        lv_obj_set_height(header, height);
    }
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
    (void)luaL_checkinteger(L, 2);
    /* lv_win_set_flags() was removed in LVGL 8 */
    (void)win;
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
    (void)luaL_checkinteger(L, 2);
    /* lv_win_clear_flags() was removed in LVGL 8 */
    (void)win;
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 win 子模块 */
void lvgl_register_win(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
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

    /* 注册create函数到主表lvgl.win) */
    REG_METHOD(L, "create", lvgl_win_create);
}
