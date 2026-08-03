/**
 * @file iot_lvgl_3dtexture.c
 * @brief LVGL 3D纹理控件
 *
 * 实现LVGL 3D纹理控件的OO风格Lua绑定，包括3D纹理创建、设置纹理源、设置翻转等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

#if LV_USE_3DTEXTURE

/* 3dtexture组件的metatable引用 */
static int texture3d_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_3dtexture_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* obj = lv_3dtexture_create(parent);
    lua_pushlightuserdata(L, obj);
    return 1;
}

/* ==================== 3DTexture OO方法 ==================== */

/*
创建3D纹理控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的3DTexture实例
@usage local tex = lvgl.texture3d.create(scr)
*/
static int iot_lvgl_3dtexture_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_3dtexture_create_internal, texture3d_metatable_ref);
}

/*
设置3D纹理源
@param self 3DTexture实例或指针
@param id 纹理ID(unsigned int)
@return self
@usage tex:set_src(100)
*/
static int iot_lvgl_3dtexture_set_src(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_3dtexture_id_t id = (lv_3dtexture_id_t)luaL_checkinteger(L, 2);
    lv_3dtexture_set_src(obj, id);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置3D纹理翻转
@param self 3DTexture实例或指针
@param h_flip 水平翻转
@param v_flip 垂直翻转
@return self
@usage tex:set_flip(true, false)
*/
static int iot_lvgl_3dtexture_set_flip(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    bool h_flip = lua_toboolean(L, 2);
    bool v_flip = lua_toboolean(L, 3);
    lv_3dtexture_set_flip(obj, h_flip, v_flip);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 3dtexture 子模块 */
void iot_lvgl_register_3dtexture(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_src", iot_lvgl_3dtexture_set_src);
    REG_METHOD(L, "set_flip", iot_lvgl_3dtexture_set_flip);

    /* 保存组件metatable引用(用于继承) */
    texture3d_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.texture3d.set_src(tex, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, texture3d_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.texture3d) */
    REG_METHOD(L, "create", iot_lvgl_3dtexture_create);
}

#else /* LV_USE_3DTEXTURE */

/* 3DTexture未启用时为空操作 */
void iot_lvgl_register_3dtexture(lua_State* L) {
    (void)L;
    lua_newtable(L);
}

#endif /* LV_USE_3DTEXTURE */