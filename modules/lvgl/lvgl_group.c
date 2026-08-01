/**
 * @file iot_lvgl_group.c
 * @brief LVGL对象分组
 *
 * 实现LVGL对象分组的Lua绑定，包括分组创建/删除、添加/移除对象、获取对象数量、获取聚焦对象、设置/获取默认分组等接口，用于焦点导航管理。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

static lv_obj_t* iot_lvgl_group_get_obj_by_index(lv_group_t* group, uint32_t index)
{
    lv_obj_t** obj_i;
    uint32_t i = 0;

    _LV_LL_READ(&group->obj_ll, obj_i) {
        if (i == index) {
            return *obj_i;
        }
        i++;
    }
    return NULL;
}

/* ==================== 对象组操作 ==================== */

static int iot_lvgl_group_create(lua_State* L) {
    lv_group_t* group = lv_group_create();
    lua_pushlightuserdata(L, group);
    return 1;
}

static int iot_lvgl_group_delete(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_checklightuserdata(L, 1);
    lv_group_del(group);
    return 0;
}

static int iot_lvgl_group_add_obj(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_checklightuserdata(L, 1);
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 2);
    lv_group_add_obj(group, obj);
    return 0;
}

static int iot_lvgl_group_remove_obj(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_group_remove_obj(obj);
    return 0;
}

static int iot_lvgl_group_remove_all_objs(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_checklightuserdata(L, 1);
    lv_group_remove_all_objs(group);
    return 0;
}

static int iot_lvgl_group_get_obj(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_checklightuserdata(L, 1);
    uint32_t index = (uint32_t)luaL_checkinteger(L, 2);
    lv_obj_t* obj = iot_lvgl_group_get_obj_by_index(group, index);
    if (obj) {
        lua_pushlightuserdata(L, obj);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

static int iot_lvgl_group_get_obj_count(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_checklightuserdata(L, 1);
    uint32_t count = lv_group_get_obj_count(group);
    lua_pushinteger(L, count);
    return 1;
}

static int iot_lvgl_group_get_focused(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_optlightuserdata(L, 1, lv_group_get_default());
    lv_obj_t* obj = lv_group_get_focused(group);
    lua_pushlightuserdata(L, obj);
    return 1;
}

static int iot_lvgl_group_focus_obj(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_group_focus_obj(obj);
    return 0;
}

static int iot_lvgl_group_focus_next(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_optlightuserdata(L, 1, lv_group_get_default());
    lv_group_focus_next(group);
    return 0;
}

static int iot_lvgl_group_focus_prev(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_optlightuserdata(L, 1, lv_group_get_default());
    lv_group_focus_prev(group);
    return 0;
}

static int iot_lvgl_group_make_obj_focusable(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_CLICK_FOCUSABLE);
    return 0;
}

static int iot_lvgl_group_set_default(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_checklightuserdata(L, 1);
    lv_group_set_default(group);
    return 0;
}

static int iot_lvgl_group_get_default(lua_State* L) {
    lv_group_t* group = lv_group_get_default();
    lua_pushlightuserdata(L, group);
    return 1;
}

static int iot_lvgl_group_set_editing(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_optlightuserdata(L, 1, lv_group_get_default());
    bool edit = lua_toboolean(L, 2);
    lv_group_set_editing(group, edit);
    return 0;
}

static int iot_lvgl_group_set_wrap(lua_State* L) {
    lv_group_t* group = (lv_group_t*)luaL_optlightuserdata(L, 1, lv_group_get_default());
    bool en = lua_toboolean(L, 2);
    lv_group_set_wrap(group, en);
    return 0;
}

/* 注册 group 子模块 */
void iot_lvgl_register_group(lua_State* L) {
    REG_METHOD(L, "create", iot_lvgl_group_create);
    REG_METHOD(L, "delete", iot_lvgl_group_delete);
    REG_METHOD(L, "add_obj", iot_lvgl_group_add_obj);
    REG_METHOD(L, "remove_obj", iot_lvgl_group_remove_obj);
    REG_METHOD(L, "remove_all_objs", iot_lvgl_group_remove_all_objs);
    REG_METHOD(L, "get_obj", iot_lvgl_group_get_obj);
    REG_METHOD(L, "get_obj_count", iot_lvgl_group_get_obj_count);
    REG_METHOD(L, "get_focused", iot_lvgl_group_get_focused);
    REG_METHOD(L, "focus_obj", iot_lvgl_group_focus_obj);
    REG_METHOD(L, "focus_next", iot_lvgl_group_focus_next);
    REG_METHOD(L, "focus_prev", iot_lvgl_group_focus_prev);
    REG_METHOD(L, "make_obj_focusable", iot_lvgl_group_make_obj_focusable);
    REG_METHOD(L, "set_default", iot_lvgl_group_set_default);
    REG_METHOD(L, "get_default", iot_lvgl_group_get_default);
    REG_METHOD(L, "set_editing", iot_lvgl_group_set_editing);
    REG_METHOD(L, "set_wrap", iot_lvgl_group_set_wrap);
}
