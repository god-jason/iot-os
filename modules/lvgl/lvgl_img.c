/*
@module  lvgl.img
@summary LVGL图片控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建图片控件
local img = lvgl.img.create(scr)
img:set_size(100, 100)
img:set_pos(50, 50)
img:set_src("/空.bin")

-- 图片变换
img:set_zoom(200)      -- 放大2倍
img:set_angle(900)    -- 旋转90度
img:set_pivot(50, 50) -- 设置旋转中心

-- 图片偏移(用于大图滚动)
img:set_offset_x(0)
img:set_offset_y(0)

-- 抗锯齿设置
img:set_antialias(true)
img:set_size_mode(lvgl.IMG_SIZE_MODE_REAL)

-- 链式调用
local img2 = lvgl.img.create(scr):set_pos(160, 50):set_size(80, 80):set_src("/图标.bin"):set_zoom(128)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* img组件的metatable引用 */
static int img_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_img_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* img = lv_img_create(parent);
    lua_pushlightuserdata(L, img);
    return 1;
}

/* ==================== 图片OO方法 ==================== */

/*
创建图片控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的图片实例
@usage local img = lvgl.img.create(scr)
*/
static int lvgl_img_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_img_create_internal, img_metatable_ref);
}

/*
设置图片源
@param self 图片实例或指针
@param src 图片路径或符号名称
@return self
@usage img:set_src("/空.bin")
*/
static int lvgl_img_set_src(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    const char* src = luaL_checkstring(L, 2);
    lv_img_set_src(img, src);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片X轴偏移
@param self 图片实例或指针
@param offset X轴偏移量
@return self
@usage img:set_offset_x(10)
*/
static int lvgl_img_set_offset_x(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    int32_t offset = (int32_t)luaL_checkinteger(L, 2);
    lv_img_set_offset_x(img, offset);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片Y轴偏移
@param self 图片实例或指针
@param offset Y轴偏移量
@return self
@usage img:set_offset_y(10)
*/
static int lvgl_img_set_offset_y(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    int32_t offset = (int32_t)luaL_checkinteger(L, 2);
    lv_img_set_offset_y(img, offset);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片缩放
@param self 图片实例或指针
@param zoom 缩放值(128=1倍,256=2倍,64=0.5倍)
@return self
@usage img:set_zoom(128)  -- 原始大小
*/
static int lvgl_img_set_zoom(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    uint32_t zoom = (uint32_t)luaL_checkinteger(L, 2);
    lv_img_set_zoom(img, zoom);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片旋转角度
@param self 图片实例或指针
@param angle 角度值(0-3600,实际为角度*10)
@return self
@usage img:set_angle(900)  -- 90度
*/
static int lvgl_img_set_angle(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    int32_t angle = (int32_t)luaL_checkinteger(L, 2);
    lv_img_set_angle(img, angle);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片旋转中心点
@param self 图片实例或指针
@param x 中心点X坐标
@param y 中心点Y坐标
@return self
@usage img:set_pivot(50, 50)
*/
static int lvgl_img_set_pivot(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_img_set_pivot(img, x, y);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置抗锯齿
@param self 图片实例或指针
@param en 是否启用抗锯齿
@return self
@usage img:set_antialias(true)
*/
static int lvgl_img_set_antialias(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_img_set_antialias(img, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片尺寸模式
@param self 图片实例或指针
@param mode 尺寸模式
@return self
@usage img:set_size_mode(lvgl.IMG_SIZE_MODE_REAL)
*/
static int lvgl_img_set_size_mode(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    lv_img_size_mode_t mode = (lv_img_size_mode_t)luaL_checkinteger(L, 2);
    lv_img_set_size_mode(img, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 img 子模块 */
void lvgl_register_img(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_src", lvgl_img_set_src);
    REG_METHOD(L, "set_offset_x", lvgl_img_set_offset_x);
    REG_METHOD(L, "set_offset_y", lvgl_img_set_offset_y);
    REG_METHOD(L, "set_zoom", lvgl_img_set_zoom);
    REG_METHOD(L, "set_angle", lvgl_img_set_angle);
    REG_METHOD(L, "set_pivot", lvgl_img_set_pivot);
    REG_METHOD(L, "set_antialias", lvgl_img_set_antialias);
    REG_METHOD(L, "set_size_mode", lvgl_img_set_size_mode);

    /* 保存组件metatable引用(用于继承) */
    img_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.img.set_src(img, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, img_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.img) */
    REG_METHOD(L, "create", lvgl_img_create);
}
