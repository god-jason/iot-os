/**
 * @file iot_lvgl_lottie.c
 * @brief LVGL Lottie动画控件
 *
 * 实现LVGL Lottie动画控件的OO风格Lua绑定，包括Lottie创建、设置缓冲区、设置绘制缓冲区、
 * 设置源（数据/文件）、获取动画对象等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

#if LV_USE_LOTTIE

/* lottie组件的metatable引用 */
static int lottie_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_lottie_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* lottie = lv_lottie_create(parent);
    lua_pushlightuserdata(L, lottie);
    return 1;
}

/* ==================== Lottie OO方法 ==================== */

/*
创建Lottie动画控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的Lottie实例
@usage local lottie = lvgl.lottie.create(scr)
*/
static int iot_lvgl_lottie_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_lottie_create_internal, lottie_metatable_ref);
}

/*
设置Lottie缓冲区（同时定义动画尺寸）
@param self Lottie实例或指针
@param w 缓冲区宽度
@param h 缓冲区高度
@param buf 缓冲区指针(lightuserdata)
@return self
@usage lottie:set_buffer(200, 200, buf)
*/
static int iot_lvgl_lottie_set_buffer(lua_State* L) {
    lv_obj_t* lottie = iot_lvgl_get_obj_ptr(L, 1);
    int32_t w = (int32_t)luaL_checkinteger(L, 2);
    int32_t h = (int32_t)luaL_checkinteger(L, 3);
    void* buf = lua_touserdata(L, 4);
    if (!buf) {
        return luaL_error(L, "lottie:set_buffer - invalid buffer pointer");
    }
    lv_lottie_set_buffer(lottie, w, h, buf);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置Lottie绘制缓冲区（同时定义动画尺寸）
@param self Lottie实例或指针
@param draw_buf 绘制缓冲区指针(lightuserdata)
@return self
@usage lottie:set_draw_buf(draw_buf)
*/
static int iot_lvgl_lottie_set_draw_buf(lua_State* L) {
    lv_obj_t* lottie = iot_lvgl_get_obj_ptr(L, 1);
    lv_draw_buf_t* draw_buf = (lv_draw_buf_t*)lua_touserdata(L, 2);
    if (!draw_buf) {
        return luaL_error(L, "lottie:set_draw_buf - invalid draw_buf pointer");
    }
    lv_lottie_set_draw_buf(lottie, draw_buf);
    lua_pushvalue(L, 1);
    return 1;
}

/*
从数据数组设置Lottie动画源
@param self Lottie实例或指针
@param src 动画数据指针(lightuserdata)
@param src_size 数据大小(字节)
@return self
@usage lottie:set_src_data(data_ptr, data_size)
*/
static int iot_lvgl_lottie_set_src_data(lua_State* L) {
    lv_obj_t* lottie = iot_lvgl_get_obj_ptr(L, 1);
    const void* src = lua_touserdata(L, 2);
    size_t src_size = (size_t)luaL_checkinteger(L, 3);
    if (!src) {
        return luaL_error(L, "lottie:set_src_data - invalid source pointer");
    }
    lv_lottie_set_src_data(lottie, src, src_size);
    lua_pushvalue(L, 1);
    return 1;
}

/*
从文件路径设置Lottie动画源
@param self Lottie实例或指针
@param src JSON文件路径
@return self
@usage lottie:set_src_file("S:/anim/data.json")
*/
static int iot_lvgl_lottie_set_src_file(lua_State* L) {
    lv_obj_t* lottie = iot_lvgl_get_obj_ptr(L, 1);
    const char* src = luaL_checkstring(L, 2);
    lv_lottie_set_src_file(lottie, src);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取Lottie动画对象（LVGL动画对象）
@param self Lottie实例或指针
@return userdata LVGL动画对象指针
@usage local anim = lottie:get_anim()
*/
static int iot_lvgl_lottie_get_anim(lua_State* L) {
    lv_obj_t* lottie = iot_lvgl_get_obj_ptr(L, 1);
    lv_anim_t* anim = lv_lottie_get_anim(lottie);
    lua_pushlightuserdata(L, anim);
    return 1;
}

/* 注册 lottie 子模块 */
void iot_lvgl_register_lottie(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_buffer", iot_lvgl_lottie_set_buffer);
    REG_METHOD(L, "set_draw_buf", iot_lvgl_lottie_set_draw_buf);
    REG_METHOD(L, "set_src_data", iot_lvgl_lottie_set_src_data);
    REG_METHOD(L, "set_src_file", iot_lvgl_lottie_set_src_file);
    REG_METHOD(L, "get_anim", iot_lvgl_lottie_get_anim);

    /* 保存组件metatable引用(用于继承) */
    lottie_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.lottie.set_src_file(lottie, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, lottie_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.lottie) */
    REG_METHOD(L, "create", iot_lvgl_lottie_create);
}

#else /* LV_USE_LOTTIE */

void iot_lvgl_register_lottie(lua_State* L) {
    /* Lottie未启用时为空操作（子表已由调用方创建，无需额外操作） */
    (void)L;
}

#endif /* LV_USE_LOTTIE */