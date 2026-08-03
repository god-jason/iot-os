/**
 * @file iot_lvgl_keyboard.c
 * @brief LVGL键盘控件
 *
 * 实现LVGL键盘控件的OO风格Lua绑定，包括键盘创建、设置关联文本区、设置键盘模式（文本/数字/用户）、获取关联文本区等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* keyboard组件的metatable引用 */
static int keyboard_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_keyboard_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* kb = lv_keyboard_create(parent);
    lua_pushlightuserdata(L, kb);
    return 1;
}

/* ==================== 键盘OO方法 ==================== */

/*
创建键盘控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的键盘实例
@usage local kb = lvgl.keyboard.create(scr)
*/
static int iot_lvgl_keyboard_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_keyboard_create_internal, keyboard_metatable_ref);
}

/*
设置关联的文本区域
@param self 键盘实例或指针
@param ta 文本区域对象
@return self
@usage kb:set_textarea(ta)
*/
static int iot_lvgl_keyboard_set_textarea(lua_State* L) {
    lv_obj_t* kb = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* ta = NULL;
    if (!lua_isnoneornil(L, 2)) {
        ta = (lv_obj_t*)iot_lvgl_get_obj_ptr(L, 2);
    }
    lv_keyboard_set_textarea(kb, ta);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置键盘模式
@param self 键盘实例或指针
@param mode 键盘模式(lvgl.KEYBOARD_MODE_TEXT等)
@return self
@usage kb:set_mode(lvgl.KEYBOARD_MODE_TEXT)
*/
static int iot_lvgl_keyboard_set_mode(lua_State* L) {
    lv_obj_t* kb = iot_lvgl_get_obj_ptr(L, 1);
    lv_keyboard_mode_t mode = (lv_keyboard_mode_t)luaL_checkinteger(L, 2);
    lv_keyboard_set_mode(kb, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置键盘映射(暂未实现)
@param self 键盘实例或指针
@return self
@usage kb:set_map(map)
*/
static int iot_lvgl_keyboard_set_map(lua_State* L) {
    lv_obj_t* kb = iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取关联的文本区域
@param self 键盘实例或指针
@return userdata 文本区域对象
@usage local ta = kb:get_textarea()
*/
static int iot_lvgl_keyboard_get_textarea(lua_State* L) {
    lv_obj_t* kb = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* ta = lv_keyboard_get_textarea(kb);
    lua_pushlightuserdata(L, ta);
    return 1;
}

/*
获取键盘模式
@param self 键盘实例或指针
@return integer 键盘模式
@usage local mode = kb:get_mode()
*/
static int iot_lvgl_keyboard_get_mode(lua_State* L) {
    lv_obj_t* kb = iot_lvgl_get_obj_ptr(L, 1);
    lv_keyboard_mode_t mode = lv_keyboard_get_mode(kb);
    lua_pushinteger(L, mode);
    return 1;
}

/*
设置弹出提示
@param self 键盘实例或指针
@param en 是否启用
@return self
@usage kb:set_popovers(true)
*/
static int iot_lvgl_keyboard_set_popovers(lua_State* L) {
    lv_obj_t* kb = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_keyboard_set_popovers(kb, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取弹出提示状态
@param self 键盘实例或指针
@return boolean 是否启用弹出提示
@usage local popovers = kb:get_popovers()
*/
static int iot_lvgl_keyboard_get_popovers(lua_State* L) {
    lv_obj_t* kb = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lv_keyboard_get_popovers(kb);
    lua_pushboolean(L, en);
    return 1;
}

/*
获取选中的按钮索引
@param self 键盘实例或指针
@return integer 按钮索引
@usage local btn = kb:get_selected_button()
*/
static int iot_lvgl_keyboard_get_selected_button(lua_State* L) {
    lv_obj_t* kb = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t btn = lv_keyboard_get_selected_button(kb);
    lua_pushinteger(L, btn);
    return 1;
}

/*
获取按钮文本
@param self 键盘实例或指针
@param btn_id 按钮索引
@return string 按钮文本
@usage local txt = kb:get_button_text(0)
*/
static int iot_lvgl_keyboard_get_button_text(lua_State* L) {
    lv_obj_t* kb = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t btn_id = (uint32_t)luaL_checkinteger(L, 2);
    const char* txt = lv_keyboard_get_button_text(kb, btn_id);
    lua_pushstring(L, txt ? txt : "");
    return 1;
}

/* 注册 keyboard 子模块 */
void iot_lvgl_register_keyboard(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_textarea", iot_lvgl_keyboard_set_textarea);
    REG_METHOD(L, "set_mode", iot_lvgl_keyboard_set_mode);
    REG_METHOD(L, "set_map", iot_lvgl_keyboard_set_map);
    REG_METHOD(L, "get_textarea", iot_lvgl_keyboard_get_textarea);
    REG_METHOD(L, "get_mode", iot_lvgl_keyboard_get_mode);
    REG_METHOD(L, "set_popovers", iot_lvgl_keyboard_set_popovers);
    REG_METHOD(L, "get_popovers", iot_lvgl_keyboard_get_popovers);
    REG_METHOD(L, "get_selected_button", iot_lvgl_keyboard_get_selected_button);
    REG_METHOD(L, "get_button_text", iot_lvgl_keyboard_get_button_text);

    /* 保存组件metatable引用(用于继承) */
    keyboard_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.keyboard.set_textarea(kb, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, keyboard_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.keyboard) */
    REG_METHOD(L, "create", iot_lvgl_keyboard_create);
}
