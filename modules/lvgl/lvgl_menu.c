/**
 * @file lvgl_menu.c
 * @brief LVGL菜单控件
 *
 * 实现LVGL菜单控件的OO风格Lua绑定，包括菜单创建、页面创建、添加菜单项、设置子页面、获取当前页面等接口，支持多级菜单导航。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* menu组件的metatable引用 */
static int menu_metatable_ref = LUA_NOREF;

static lv_obj_t* lvgl_menu_find_menu(lv_obj_t* obj)
{
    lv_obj_t* cur = obj;
    while (cur) {
        if (lv_obj_check_type(cur, &lv_menu_class)) {
            return cur;
        }
        cur = lv_obj_get_parent(cur);
    }
    return NULL;
}

/* ==================== 内部创建函数 ==================== */

static int lvgl_menu_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* menu = lv_menu_create(parent);
    lua_pushlightuserdata(L, menu);
    return 1;
}

/* ==================== ??OO?? ==================== */

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
    const char* title = lua_isnoneornil(L, 2) ? NULL : luaL_checkstring(L, 2);
    lv_obj_t* page = lv_menu_page_create(menu, title ? (char*)title : NULL);
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
    lv_obj_t* cont = lv_menu_cont_create(page);
    if (icon) {
        lv_obj_set_parent(icon, cont);
    }
    lv_obj_t* label = lv_label_create(cont);
    lv_label_set_text(label, text);
    lua_pushlightuserdata(L, cont);
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
    lv_obj_t* menu = lvgl_menu_find_menu(item);
    if (menu) {
        lv_menu_set_load_page_event(menu, item, page);
    }
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
    lv_obj_t* page = lv_menu_get_cur_main_page(menu);
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
    lv_coord_t width = (lv_coord_t)luaL_checkinteger(L, 2);
    lv_obj_set_width(menu, width);
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
    lv_obj_t* page = lv_menu_get_cur_main_page(menu);
    if (page) {
        lv_menu_page_t* menu_page = (lv_menu_page_t*)page;
        if (menu_page->title) {
            lv_mem_free(menu_page->title);
        }
        menu_page->title = lv_mem_alloc(strlen(title) + 1);
        if (menu_page->title) {
            strcpy(menu_page->title, title);
        }
        lv_event_send(menu, LV_EVENT_VALUE_CHANGED, NULL);
    }
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
    lv_menu_set_page(menu, NULL);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 menu 子模块 */
void lvgl_register_menu(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
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

    /* 注册create函数到主表lvgl.menu) */
    REG_METHOD(L, "create", lvgl_menu_create);
}
