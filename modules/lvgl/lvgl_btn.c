/**
 * @file iot_lvgl_btn.c
 * @brief LVGL按钮控件
 *
 * 实现LVGL按钮控件的OO风格Lua绑定，包括按钮创建、设置/获取文本、设置可选中/切换、设置布局、设置点击事件、切换状态等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* btn组件的metatable引用 */
static int btn_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

/* 实际创建按钮的函数(被OO包装器调用) */
static int iot_lvgl_btn_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* btn = lv_btn_create(parent);
    lua_pushlightuserdata(L, btn);
    return 1;
}

/* ==================== 按钮OO方法 ==================== */

/*
创建按钮(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的按钮实例
@usage local btn = lvgl.btn.create(scr)
*/
static int iot_lvgl_btn_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_btn_create_internal, btn_metatable_ref);
}

/*
设置按钮文本
@param self 按钮实例或指针
@param text 文本内容
@return self
@usage btn:set_text("OK")
*/
static int iot_lvgl_btn_set_text(lua_State* L) {
    lv_obj_t* btn = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    iot_lv_btn_set_text(btn, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取按钮文本
@param self 按钮实例或指针
@return string 文本内容
@usage local text = btn:get_text()
*/
static int iot_lvgl_btn_get_text(lua_State* L) {
    lv_obj_t* btn = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = iot_lv_btn_get_text(btn);
    lua_pushstring(L, text);
    return 1;
}

/*
设置按钮状态
@param self 按钮实例或指针
@param state 状态值
@return self
@usage btn:set_state(lvgl.BTN_STATE_PRESSED)
*/
static int iot_lvgl_btn_set_state(lua_State* L) {
    lv_obj_t* btn = iot_lvgl_get_obj_ptr(L, 1);
    iot_lv_btn_state_t state = (iot_lv_btn_state_t)luaL_checkinteger(L, 2);
    iot_lv_btn_set_state(btn, state);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取按钮状态
@param self 按钮实例或指针
@return integer 状态值
@usage local state = btn:get_state()
*/
static int iot_lvgl_btn_get_state(lua_State* L) {
    lv_obj_t* btn = iot_lvgl_get_obj_ptr(L, 1);
    iot_lv_btn_state_t state = iot_lv_btn_get_state(btn);
    lua_pushinteger(L, state);
    return 1;
}

/*
切换按钮状态
@param self 按钮实例或指针
@return self
@usage btn:toggle()
*/
static int iot_lvgl_btn_toggle(lua_State* L) {
    lv_obj_t* btn = iot_lvgl_get_obj_ptr(L, 1);
    iot_lv_btn_toggle(btn);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置是否可切换
@param self 按钮实例或指针
@param en 是否可切换
@return self
@usage btn:set_checkable(true)
*/
static int iot_lvgl_btn_set_checkable(lua_State* L) {
    lv_obj_t* btn = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    iot_lv_btn_set_checkable(btn, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置按钮布局
@param self 按钮实例或指针
@param layout 布局类型
@return self
@usage btn:set_layout(lvgl.LAYOUT_CENTER)
*/
static int iot_lvgl_btn_set_layout(lua_State* L) {
    lv_obj_t* btn = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t layout = (uint32_t)luaL_checkinteger(L, 2);
    iot_lv_btn_set_layout(btn, layout);
    lua_pushvalue(L, 1);
    return 1;
}

/*
清除按钮布局
@param self 按钮实例或指针
@return self
@usage btn:clear_layout()
*/
static int iot_lvgl_btn_clear_layout(lua_State* L) {
    lv_obj_t* btn = iot_lvgl_get_obj_ptr(L, 1);
    iot_lv_btn_set_layout(btn, LV_LAYOUT_NONE);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 btn 子模块 */
void iot_lvgl_register_btn(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);
    
    /* 注册OO风格方法(可以instance:method()调用) */
    REG_METHOD(L, "set_text", iot_lvgl_btn_set_text);
    REG_METHOD(L, "get_text", iot_lvgl_btn_get_text);
    REG_METHOD(L, "set_state", iot_lvgl_btn_set_state);
    REG_METHOD(L, "get_state", iot_lvgl_btn_get_state);
    REG_METHOD(L, "toggle", iot_lvgl_btn_toggle);
    REG_METHOD(L, "set_checkable", iot_lvgl_btn_set_checkable);
    REG_METHOD(L, "set_layout", iot_lvgl_btn_set_layout);
    REG_METHOD(L, "clear_layout", iot_lvgl_btn_clear_layout);

    /* 保存组件metatable引用(用于继承) */
    btn_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.btn.set_text(btn, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, btn_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.btn) */
    REG_METHOD(L, "create", iot_lvgl_btn_create);
}
