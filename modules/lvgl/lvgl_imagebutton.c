/**
 * @file iot_lvgl_imagebutton.c
 * @brief LVGL图片按钮控件
 *
 * 实现LVGL图片按钮控件的OO风格Lua绑定，包括图片按钮创建、设置/获取各状态下的左中右三部分图片源、设置状态等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"
#include "src/widgets/imagebutton/lv_imagebutton.h"

/* imagebutton组件的metatable引用 */
static int imagebutton_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_imagebutton_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* imgbtn = lv_imagebutton_create(parent);
    lua_pushlightuserdata(L, imgbtn);
    return 1;
}

/* ==================== 图片按钮OO方法 ==================== */

/*
创建图片按钮(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的图片按钮实例
@usage local imgbtn = lvgl.imagebutton.create(scr)
*/
static int iot_lvgl_imagebutton_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_imagebutton_create_internal, imagebutton_metatable_ref);
}

/*
设置图片按钮在指定状态下的三部分图片源
@param self 图片按钮实例或指针
@param state 状态(lvgl.IMAGEBUTTON_STATE_RELEASED 等)
@param src_left 左侧图片源(路径字符串)
@param src_mid 中间图片源(路径字符串,理想为1像素宽)
@param src_right 右侧图片源(路径字符串)
@return self
@usage imgbtn:set_src(lvgl.IMAGEBUTTON_STATE_RELEASED, "btn_left.png", "btn_mid.png", "btn_right.png")
*/
static int iot_lvgl_imagebutton_set_src(lua_State* L) {
    lv_obj_t* imgbtn = iot_lvgl_get_obj_ptr(L, 1);
    lv_imagebutton_state_t state = (lv_imagebutton_state_t)luaL_checkinteger(L, 2);
    const char* src_left = luaL_checkstring(L, 3);
    const char* src_mid = luaL_checkstring(L, 4);
    const char* src_right = luaL_checkstring(L, 5);
    lv_imagebutton_set_src(imgbtn, state, src_left, src_mid, src_right);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片按钮在指定状态下的左侧图片源
@param self 图片按钮实例或指针
@param state 状态
@param src_left 左侧图片源(路径字符串)
@return self
@usage imgbtn:set_src_left(lvgl.IMAGEBUTTON_STATE_RELEASED, "btn_left.png")
*/
static int iot_lvgl_imagebutton_set_src_left(lua_State* L) {
    lv_obj_t* imgbtn = iot_lvgl_get_obj_ptr(L, 1);
    lv_imagebutton_state_t state = (lv_imagebutton_state_t)luaL_checkinteger(L, 2);
    const char* src_left = luaL_checkstring(L, 3);
    lv_imagebutton_set_src_left(imgbtn, state, src_left);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片按钮在指定状态下的中间图片源
@param self 图片按钮实例或指针
@param state 状态
@param src_mid 中间图片源(路径字符串,理想为1像素宽)
@return self
@usage imgbtn:set_src_mid(lvgl.IMAGEBUTTON_STATE_RELEASED, "btn_mid.png")
*/
static int iot_lvgl_imagebutton_set_src_mid(lua_State* L) {
    lv_obj_t* imgbtn = iot_lvgl_get_obj_ptr(L, 1);
    lv_imagebutton_state_t state = (lv_imagebutton_state_t)luaL_checkinteger(L, 2);
    const char* src_mid = luaL_checkstring(L, 3);
    lv_imagebutton_set_src_mid(imgbtn, state, src_mid);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片按钮在指定状态下的右侧图片源
@param self 图片按钮实例或指针
@param state 状态
@param src_right 右侧图片源(路径字符串)
@return self
@usage imgbtn:set_src_right(lvgl.IMAGEBUTTON_STATE_RELEASED, "btn_right.png")
*/
static int iot_lvgl_imagebutton_set_src_right(lua_State* L) {
    lv_obj_t* imgbtn = iot_lvgl_get_obj_ptr(L, 1);
    lv_imagebutton_state_t state = (lv_imagebutton_state_t)luaL_checkinteger(L, 2);
    const char* src_right = luaL_checkstring(L, 3);
    lv_imagebutton_set_src_right(imgbtn, state, src_right);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片按钮状态
@param self 图片按钮实例或指针
@param state 状态(lvgl.IMAGEBUTTON_STATE_RELEASED 等)
@return self
@usage imgbtn:set_state(lvgl.IMAGEBUTTON_STATE_PRESSED)
*/
static int iot_lvgl_imagebutton_set_state(lua_State* L) {
    lv_obj_t* imgbtn = iot_lvgl_get_obj_ptr(L, 1);
    lv_imagebutton_state_t state = (lv_imagebutton_state_t)luaL_checkinteger(L, 2);
    lv_imagebutton_set_state(imgbtn, state);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取指定状态下的左侧图片源
@param self 图片按钮实例或指针
@param state 状态
@return string 左侧图片源路径
@usage local src = imgbtn:get_src_left(lvgl.IMAGEBUTTON_STATE_RELEASED)
*/
static int iot_lvgl_imagebutton_get_src_left(lua_State* L) {
    lv_obj_t* imgbtn = iot_lvgl_get_obj_ptr(L, 1);
    lv_imagebutton_state_t state = (lv_imagebutton_state_t)luaL_checkinteger(L, 2);
    const void* src = lv_imagebutton_get_src_left(imgbtn, state);
    lua_pushstring(L, (const char*)src);
    return 1;
}

/*
获取指定状态下的中间图片源
@param self 图片按钮实例或指针
@param state 状态
@return string 中间图片源路径
@usage local src = imgbtn:get_src_mid(lvgl.IMAGEBUTTON_STATE_RELEASED)
*/
static int iot_lvgl_imagebutton_get_src_mid(lua_State* L) {
    lv_obj_t* imgbtn = iot_lvgl_get_obj_ptr(L, 1);
    lv_imagebutton_state_t state = (lv_imagebutton_state_t)luaL_checkinteger(L, 2);
    const void* src = lv_imagebutton_get_src_middle(imgbtn, state);
    lua_pushstring(L, (const char*)src);
    return 1;
}

/*
获取指定状态下的右侧图片源
@param self 图片按钮实例或指针
@param state 状态
@return string 右侧图片源路径
@usage local src = imgbtn:get_src_right(lvgl.IMAGEBUTTON_STATE_RELEASED)
*/
static int iot_lvgl_imagebutton_get_src_right(lua_State* L) {
    lv_obj_t* imgbtn = iot_lvgl_get_obj_ptr(L, 1);
    lv_imagebutton_state_t state = (lv_imagebutton_state_t)luaL_checkinteger(L, 2);
    const void* src = lv_imagebutton_get_src_right(imgbtn, state);
    lua_pushstring(L, (const char*)src);
    return 1;
}

/* 注册 imagebutton 子模块 */
void iot_lvgl_register_imagebutton(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法(可以instance:method()调用) */
    REG_METHOD(L, "set_src", iot_lvgl_imagebutton_set_src);
    REG_METHOD(L, "set_src_left", iot_lvgl_imagebutton_set_src_left);
    REG_METHOD(L, "set_src_mid", iot_lvgl_imagebutton_set_src_mid);
    REG_METHOD(L, "set_src_right", iot_lvgl_imagebutton_set_src_right);
    REG_METHOD(L, "set_state", iot_lvgl_imagebutton_set_state);
    REG_METHOD(L, "get_src_left", iot_lvgl_imagebutton_get_src_left);
    REG_METHOD(L, "get_src_mid", iot_lvgl_imagebutton_get_src_mid);
    REG_METHOD(L, "get_src_right", iot_lvgl_imagebutton_get_src_right);

    /* 保存组件metatable引用(用于继承) */
    imagebutton_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.imagebutton.set_src(imgbtn, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, imagebutton_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.imagebutton) */
    REG_METHOD(L, "create", iot_lvgl_imagebutton_create);
}