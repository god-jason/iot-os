/**
 * @file lvgl_list.c
 * @brief LVGL列表控件
 *
 * 实现LVGL列表控件的OO风格Lua绑定，包括列表创建、添加按钮项、设置方向、获取选中按钮等接口，支持单选模式。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* list组件的metatable引用 */
static int list_metatable_ref = LUA_NOREF;

static lv_obj_t* lvgl_list_find_selected_btn(lv_obj_t* list)
{
    uint32_t i;
    uint32_t cnt = lv_obj_get_child_cnt(list);

    for (i = 0; i < cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(list, i);
        if (lv_obj_check_type(child, &lv_list_btn_class) &&
            lv_obj_has_state(child, LV_STATE_CHECKED)) {
            return child;
        }
    }
    return NULL;
}

static void lvgl_list_clear_selected(lv_obj_t* list)
{
    uint32_t i;
    uint32_t cnt = lv_obj_get_child_cnt(list);

    for (i = 0; i < cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(list, i);
        if (lv_obj_check_type(child, &lv_list_btn_class)) {
            lv_obj_clear_state(child, LV_STATE_CHECKED);
        }
    }
}

static lv_flex_flow_t lvgl_list_dir_to_flex(lv_dir_t dir)
{
    switch (dir) {
        case LV_DIR_LEFT:
            return LV_FLEX_FLOW_ROW;
        case LV_DIR_RIGHT:
            return LV_FLEX_FLOW_ROW_REVERSE;
        case LV_DIR_BOTTOM:
            return LV_FLEX_FLOW_COLUMN_REVERSE;
        case LV_DIR_TOP:
        default:
            return LV_FLEX_FLOW_COLUMN;
    }
}

/* ==================== 内部创建函数 ==================== */

static int lvgl_list_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* list = lv_list_create(parent);
    lua_pushlightuserdata(L, list);
    return 1;
}

/* ==================== ??OO?? ==================== */

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
    lua_pushlightuserdata(L, list);
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
    lv_obj_t* btn = lvgl_list_find_selected_btn(list);
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
    lv_obj_set_flex_flow(list, lvgl_list_dir_to_flex(dir));
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
    lvgl_list_clear_selected(list);
    lv_obj_add_state(sel, LV_STATE_CHECKED);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 list 子模块 */
void lvgl_register_list(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
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

    /* 注册create函数到主表lvgl.list) */
    REG_METHOD(L, "create", lvgl_list_create);
}
