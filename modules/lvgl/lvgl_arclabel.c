/**
 * @file iot_lvgl_arclabel.c
 * @brief LVGL弧形标签控件
 *
 * 实现LVGL弧形标签控件的OO风格Lua绑定，包括弧形标签创建、设置/获取文本、设置/获取起始角度、设置/获取角度跨度、设置方向、设置/获取颜色重绘等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"
#include "widgets/arclabel/lv_arclabel_private.h"

/* arclabel组件的metatable引用 */
static int arclabel_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_arclabel_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* arclabel = lv_arclabel_create(parent);
    lua_pushlightuserdata(L, arclabel);
    return 1;
}

/* ==================== 弧形标签OO方法 ==================== */

/*
创建弧形标签控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的弧形标签实例
@usage local label = lvgl.arclabel.create(scr)
*/
static int iot_lvgl_arclabel_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_arclabel_create_internal, arclabel_metatable_ref);
}

/*
设置弧形标签文本
@param self 弧形标签实例或指针
@param text 文本内容
@return self
@usage label:set_text("Hello Arc")
*/
static int iot_lvgl_arclabel_set_text(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_arclabel_set_text(obj, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取弧形标签文本
@param self 弧形标签实例或指针
@return string 文本内容
@usage local text = label:get_text()
*/
static int iot_lvgl_arclabel_get_text(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_t* arclabel = (lv_arclabel_t*)obj;
    lua_pushstring(L, arclabel->text ? arclabel->text : "");
    return 1;
}

/*
设置弧形起始角度
@param self 弧形标签实例或指针
@param angle 起始角度(0度=右侧,90度=底部)
@return self
@usage label:set_angle_start(0)
*/
static int iot_lvgl_arclabel_set_angle_start(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t angle = (lv_value_precise_t)luaL_checknumber(L, 2);
    lv_arclabel_set_angle_start(obj, angle);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取弧形起始角度
@param self 弧形标签实例或指针
@return number 起始角度
@usage local angle = label:get_angle_start()
*/
static int iot_lvgl_arclabel_get_angle_start(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t angle = lv_arclabel_get_angle_start(obj);
    lua_pushnumber(L, (lua_Number)angle);
    return 1;
}

/*
设置弧形角度跨度
@param self 弧形标签实例或指针
@param size 角度跨度(0-360)
@return self
@usage label:set_angle_size(180)
*/
static int iot_lvgl_arclabel_set_angle_size(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t size = (lv_value_precise_t)luaL_checknumber(L, 2);
    lv_arclabel_set_angle_size(obj, size);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取弧形角度跨度
@param self 弧形标签实例或指针
@return number 角度跨度
@usage local size = label:get_angle_size()
*/
static int iot_lvgl_arclabel_get_angle_size(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t size = lv_arclabel_get_angle_size(obj);
    lua_pushnumber(L, (lua_Number)size);
    return 1;
}

/*
设置弧形方向
@param self 弧形标签实例或指针
@param dir 方向(LV_ARCLABEL_DIR_CLOCKWISE=0, LV_ARCLABEL_DIR_COUNTER_CLOCKWISE=1)
@return self
@usage label:set_dir(lvgl.ARCLABEL_DIR_CLOCKWISE)
*/
static int iot_lvgl_arclabel_set_dir(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_dir_t dir = (lv_arclabel_dir_t)luaL_checkinteger(L, 2);
    lv_arclabel_set_dir(obj, dir);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置是否启用颜色重绘
@param self 弧形标签实例或指针
@param en true启用/false禁用
@return self
@usage label:set_recolor(true)
*/
static int iot_lvgl_arclabel_set_recolor(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_arclabel_set_recolor(obj, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取是否启用颜色重绘
@param self 弧形标签实例或指针
@return boolean 是否启用颜色重绘
@usage local en = label:get_recolor()
*/
static int iot_lvgl_arclabel_get_recolor(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lv_arclabel_get_recolor(obj);
    lua_pushboolean(L, en);
    return 1;
}

/* 注册 arclabel 子模块 */
void iot_lvgl_register_arclabel(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法(可以instance:method()调用) */
    REG_METHOD(L, "set_text", iot_lvgl_arclabel_set_text);
    REG_METHOD(L, "get_text", iot_lvgl_arclabel_get_text);
    REG_METHOD(L, "set_angle_start", iot_lvgl_arclabel_set_angle_start);
    REG_METHOD(L, "get_angle_start", iot_lvgl_arclabel_get_angle_start);
    REG_METHOD(L, "set_angle_size", iot_lvgl_arclabel_set_angle_size);
    REG_METHOD(L, "get_angle_size", iot_lvgl_arclabel_get_angle_size);
    REG_METHOD(L, "set_dir", iot_lvgl_arclabel_set_dir);
    REG_METHOD(L, "set_recolor", iot_lvgl_arclabel_set_recolor);
    REG_METHOD(L, "get_recolor", iot_lvgl_arclabel_get_recolor);

    /* 保存组件metatable引用(用于继承) */
    arclabel_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.arclabel.set_text(label, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, arclabel_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.arclabel) */
    REG_METHOD(L, "create", iot_lvgl_arclabel_create);
}