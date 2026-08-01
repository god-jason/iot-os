/**
 * @file iot_lvgl_checkbox.c
 * @brief LVGL复选框控件
 *
 * 实现LVGL复选框控件的OO风格Lua绑定，包括复选框创建、设置/获取文本、设置/获取状态（未选中/选中/不确定）、设置静态文本等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* checkbox组件的metatable引用 */
static int checkbox_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_checkbox_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* cb = lv_checkbox_create(parent);
    lua_pushlightuserdata(L, cb);
    return 1;
}

/* ==================== 复选框OO方法 ==================== */

/*
创建复选框控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的复选框实例
@usage local cb = lvgl.checkbox.create(scr)
*/
static int iot_lvgl_checkbox_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_checkbox_create_internal, checkbox_metatable_ref);
}

/*
设置复选框文本
@param self 复选框实例或指针
@param text 文本内容
@return self
@usage cb:set_text("同意协议")
*/
static int iot_lvgl_checkbox_set_text(lua_State* L) {
    lv_obj_t* cb = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_checkbox_set_text(cb, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置复选框静态文本(不拷贝字符串)
@param self 复选框实例或指针
@param text 文本内容(必须保持有效)
@return self
@usage cb:set_text_static("选项")
*/
static int iot_lvgl_checkbox_set_text_static(lua_State* L) {
    lv_obj_t* cb = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_checkbox_set_text_static(cb, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置复选框选中状态
@param self 复选框实例或指针
@param checked 是否选中
@return self
@usage cb:set_checked(true)
*/
static int iot_lvgl_checkbox_set_checked(lua_State* L) {
    lv_obj_t* cb = iot_lvgl_get_obj_ptr(L, 1);
    bool checked = lua_toboolean(L, 2);
    iot_lv_checkbox_set_checked(cb, checked);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置复选框状态
@param self 复选框实例或指针
@param state 状态值: CHECKBOX_STATE_UNCHECKED, CHECKBOX_STATE_CHECKED, CHECKBOX_STATE_TRISTATE
@return self
@usage cb:set_state(lvgl.CHECKBOX_STATE_CHECKED)
*/
static int iot_lvgl_checkbox_set_state(lua_State* L) {
    lv_obj_t* cb = iot_lvgl_get_obj_ptr(L, 1);
    iot_lv_checkbox_state_t state = (iot_lv_checkbox_state_t)luaL_checkinteger(L, 2);
    iot_lv_checkbox_set_state(cb, state);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取复选框文本
@param self 复选框实例或指针
@return string 文本内容
@usage local text = cb:get_text()
*/
static int iot_lvgl_checkbox_get_text(lua_State* L) {
    lv_obj_t* cb = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = lv_checkbox_get_text(cb);
    lua_pushstring(L, text ? text : "");
    return 1;
}

/*
检查复选框是否选中
@param self 复选框实例或指针
@return boolean 是否选中
@usage local checked = cb:is_checked()
*/
static int iot_lvgl_checkbox_is_checked(lua_State* L) {
    lv_obj_t* cb = iot_lvgl_get_obj_ptr(L, 1);
    bool checked = iot_lv_checkbox_is_checked(cb);
    lua_pushboolean(L, checked);
    return 1;
}

/*
获取复选框状态
@param self 复选框实例或指针
@return integer 状态值
@usage local state = cb:get_state()
*/
static int iot_lvgl_checkbox_get_state(lua_State* L) {
    lv_obj_t* cb = iot_lvgl_get_obj_ptr(L, 1);
    iot_lv_checkbox_state_t state = iot_lv_checkbox_get_state(cb);
    lua_pushinteger(L, state);
    return 1;
}

/* 注册 checkbox 子模块 */
void iot_lvgl_register_checkbox(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_text", iot_lvgl_checkbox_set_text);
    REG_METHOD(L, "set_text_static", iot_lvgl_checkbox_set_text_static);
    REG_METHOD(L, "set_checked", iot_lvgl_checkbox_set_checked);
    REG_METHOD(L, "set_state", iot_lvgl_checkbox_set_state);
    REG_METHOD(L, "get_text", iot_lvgl_checkbox_get_text);
    REG_METHOD(L, "is_checked", iot_lvgl_checkbox_is_checked);
    REG_METHOD(L, "get_state", iot_lvgl_checkbox_get_state);

    checkbox_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, checkbox_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.checkbox) */
    REG_METHOD(L, "create", iot_lvgl_checkbox_create);
}
