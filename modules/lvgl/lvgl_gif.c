/**
 * @file iot_lvgl_gif.c
 * @brief LVGL GIF控件
 *
 * 实现LVGL GIF控件的OO风格Lua绑定，包括GIF创建、设置源、设置颜色格式、播放控制（暂停/恢复/重新启动）、
 * 获取尺寸、帧信息等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* gif组件的metatable引用 */
static int gif_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_gif_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* gif = lv_gif_create(parent);
    lua_pushlightuserdata(L, gif);
    return 1;
}

/* ==================== GIF OO方法 ==================== */

/*
创建GIF控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的GIF实例
@usage local gif = lvgl.gif.create(scr)
*/
static int iot_lvgl_gif_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_gif_create_internal, gif_metatable_ref);
}

/*
设置GIF源
@param self GIF实例或指针
@param src GIF数据描述符指针或文件路径
@return self
@usage gif:set_src("S:/dir/anim.gif")
*/
static int iot_lvgl_gif_set_src(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    const void* src = (const void*)luaL_checkstring(L, 2);
    lv_gif_set_src(gif, src);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置颜色格式
@param self GIF实例或指针
@param color_format 颜色格式(如 LV_COLOR_FORMAT_ARGB8888)
@return self
@usage gif:set_color_format(lvgl.COLOR_FORMAT_ARGB8888)
*/
static int iot_lvgl_gif_set_color_format(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    lv_color_format_t color_format = (lv_color_format_t)luaL_checkinteger(L, 2);
    lv_gif_set_color_format(gif, color_format);
    lua_pushvalue(L, 1);
    return 1;
}

/*
重新启动GIF动画
@param self GIF实例或指针
@return self
@usage gif:restart()
*/
static int iot_lvgl_gif_restart(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    lv_gif_restart(gif);
    lua_pushvalue(L, 1);
    return 1;
}

/*
暂停GIF动画
@param self GIF实例或指针
@return self
@usage gif:pause()
*/
static int iot_lvgl_gif_pause(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    lv_gif_pause(gif);
    lua_pushvalue(L, 1);
    return 1;
}

/*
恢复GIF动画
@param self GIF实例或指针
@return self
@usage gif:resume()
*/
static int iot_lvgl_gif_resume(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    lv_gif_resume(gif);
    lua_pushvalue(L, 1);
    return 1;
}

/*
检查GIF是否加载成功
@param self GIF实例或指针
@return boolean 是否加载成功
@usage local loaded = gif:is_loaded()
*/
static int iot_lvgl_gif_is_loaded(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    bool loaded = lv_gif_is_loaded(gif);
    lua_pushboolean(L, loaded);
    return 1;
}

/*
获取循环次数
@param self GIF实例或指针
@return integer 循环次数
@usage local count = gif:get_loop_count()
*/
static int iot_lvgl_gif_get_loop_count(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    int32_t count = lv_gif_get_loop_count(gif);
    lua_pushinteger(L, count);
    return 1;
}

/*
设置循环次数
@param self GIF实例或指针
@param count 循环次数
@return self
@usage gif:set_loop_count(3)
*/
static int iot_lvgl_gif_set_loop_count(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    int32_t count = (int32_t)luaL_checkinteger(L, 2);
    lv_gif_set_loop_count(gif, count);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置不可见时自动暂停
@param self GIF实例或指针
@param auto_pause true:不可见时自动暂停, false:不自动暂停
@return self
@usage gif:set_auto_pause_invisible(true)
*/
static int iot_lvgl_gif_set_auto_pause_invisible(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    bool auto_pause = lua_toboolean(L, 2);
    lv_gif_set_auto_pause_invisible(gif, auto_pause);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取GIF尺寸(静态函数)
@param src GIF文件路径
@return width, height 成功返回宽高；失败返回nil
@usage local w, h = lvgl.gif.get_size("S:/dir/anim.gif")
*/
static int iot_lvgl_gif_get_size(lua_State* L) {
    const char* src = luaL_checkstring(L, 1);
    uint16_t w = 0;
    uint16_t h = 0;
    bool success = lv_gif_get_size(src, &w, &h);
    if (success) {
        lua_pushinteger(L, w);
        lua_pushinteger(L, h);
        return 2;
    }
    lua_pushnil(L);
    return 1;
}

/*
获取帧数
@param self GIF实例或指针
@return integer 帧数
@usage local count = gif:get_frame_count()
*/
static int iot_lvgl_gif_get_frame_count(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    int32_t count = lv_gif_get_frame_count(gif);
    lua_pushinteger(L, count);
    return 1;
}

/*
获取当前帧索引
@param self GIF实例或指针
@return integer 当前帧索引
@usage local index = gif:get_current_frame_index()
*/
static int iot_lvgl_gif_get_current_frame_index(lua_State* L) {
    lv_obj_t* gif = iot_lvgl_get_obj_ptr(L, 1);
    int32_t index = lv_gif_get_current_frame_index(gif);
    lua_pushinteger(L, index);
    return 1;
}

/* 注册 gif 子模块 */
void iot_lvgl_register_gif(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_src", iot_lvgl_gif_set_src);
    REG_METHOD(L, "set_color_format", iot_lvgl_gif_set_color_format);
    REG_METHOD(L, "restart", iot_lvgl_gif_restart);
    REG_METHOD(L, "pause", iot_lvgl_gif_pause);
    REG_METHOD(L, "resume", iot_lvgl_gif_resume);
    REG_METHOD(L, "is_loaded", iot_lvgl_gif_is_loaded);
    REG_METHOD(L, "get_loop_count", iot_lvgl_gif_get_loop_count);
    REG_METHOD(L, "set_loop_count", iot_lvgl_gif_set_loop_count);
    REG_METHOD(L, "set_auto_pause_invisible", iot_lvgl_gif_set_auto_pause_invisible);
    REG_METHOD(L, "get_frame_count", iot_lvgl_gif_get_frame_count);
    REG_METHOD(L, "get_current_frame_index", iot_lvgl_gif_get_current_frame_index);

    /* 保存组件metatable引用(用于继承) */
    gif_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.gif.set_src(gif, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, gif_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册get_size静态函数到主表(lvgl.gif) */
    REG_METHOD(L, "get_size", iot_lvgl_gif_get_size);

    /* 注册create函数到主表(lvgl.gif) */
    REG_METHOD(L, "create", iot_lvgl_gif_create);
}