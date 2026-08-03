/**
 * @file iot_lvgl_animimage.c
 * @brief LVGL动画图片控件
 *
 * 实现LVGL动画图片控件的OO风格Lua绑定，包括动画图片创建、设置图片源序列、启动动画、设置/获取时长、设置/获取重复次数等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* animimage组件的metatable引用 */
static int animimage_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_animimage_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* animimg = lv_animimg_create(parent);
    lua_pushlightuserdata(L, animimg);
    return 1;
}

/* ==================== 动画图片OO方法 ==================== */

/*
创建动画图片控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的动画图片实例
@usage local anim = lvgl.animimage.create(scr)
*/
static int iot_lvgl_animimage_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_animimage_create_internal, animimage_metatable_ref);
}

/*
设置动画图片源序列
@param self 动画图片实例或指针
@param ... 图片源路径列表(可变参数)或包含图片路径的表
@return self
@usage anim:set_src("img1.bin", "img2.bin", "img3.bin")
@usage anim:set_src({"img1.bin", "img2.bin", "img3.bin"})
*/
static int iot_lvgl_animimage_set_src(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    const void* dsc[128];
    size_t num = 0;
    int n;

    if (lua_istable(L, 2)) {
        /* 参数为表：遍历表收集图片源 */
        lua_len(L, 2);
        n = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);
        if (n > 128) n = 128;
        for (int i = 0; i < n; i++) {
            lua_rawgeti(L, 2, i + 1);
            dsc[i] = (const void*)luaL_checkstring(L, -1);
            lua_pop(L, 1);
        }
        num = (size_t)n;
    } else {
        /* 参数为可变字符串列表 */
        n = lua_gettop(L) - 1;
        if (n > 128) n = 128;
        for (int i = 0; i < n; i++) {
            dsc[i] = (const void*)luaL_checkstring(L, i + 2);
        }
        num = (size_t)n;
    }

    lv_animimg_set_src(animimg, dsc, num);
    lua_pushvalue(L, 1);
    return 1;
}

/*
启动动画
@param self 动画图片实例或指针
@return self
@usage anim:start()
*/
static int iot_lvgl_animimage_start(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    lv_animimg_start(animimg);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置每帧显示时长
@param self 动画图片实例或指针
@param duration 时长(毫秒)
@return self
@usage anim:set_duration(100)
*/
static int iot_lvgl_animimage_set_duration(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t duration = (uint32_t)luaL_checkinteger(L, 2);
    lv_animimg_set_duration(animimg, duration);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置动画重复次数
@param self 动画图片实例或指针
@param count 重复次数(LV_ANIM_REPEAT_INFINITE=无限循环)
@return self
@usage anim:set_repeat_count(LV_ANIM_REPEAT_INFINITE)
*/
static int iot_lvgl_animimage_set_repeat_count(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t count = (uint32_t)luaL_checkinteger(L, 2);
    lv_animimg_set_repeat_count(animimg, count);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取动画图片源序列指针
@param self 动画图片实例或指针
@return lightuserdata 图片源序列指针
@usage local src = anim:get_src()
*/
static int iot_lvgl_animimage_get_src(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    const void** src = lv_animimg_get_src(animimg);
    lua_pushlightuserdata(L, (void*)src);
    return 1;
}

/*
获取每帧显示时长
@param self 动画图片实例或指针
@return integer 时长(毫秒)
@usage local duration = anim:get_duration()
*/
static int iot_lvgl_animimage_get_duration(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t duration = lv_animimg_get_duration(animimg);
    lua_pushinteger(L, duration);
    return 1;
}

/*
获取动画重复次数
@param self 动画图片实例或指针
@return integer 重复次数
@usage local count = anim:get_repeat_count()
*/
static int iot_lvgl_animimage_get_repeat_count(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t count = lv_animimg_get_repeat_count(animimg);
    lua_pushinteger(L, count);
    return 1;
}

/*
设置反向播放的图片源序列
@param self 动画图片实例或指针
@param ... 图片源路径列表(可变参数)或包含图片路径的表
@return self
@usage anim:set_src_reverse("img3.bin", "img2.bin", "img1.bin")
*/
static int iot_lvgl_animimage_set_src_reverse(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    const void* dsc[128];
    size_t num = 0;
    int n;

    if (lua_istable(L, 2)) {
        lua_len(L, 2);
        n = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);
        if (n > 128) n = 128;
        for (int i = 0; i < n; i++) {
            lua_rawgeti(L, 2, i + 1);
            dsc[i] = (const void*)luaL_checkstring(L, -1);
            lua_pop(L, 1);
        }
        num = (size_t)n;
    } else {
        n = lua_gettop(L) - 1;
        if (n > 128) n = 128;
        for (int i = 0; i < n; i++) {
            dsc[i] = (const void*)luaL_checkstring(L, i + 2);
        }
        num = (size_t)n;
    }

    lv_animimg_set_src_reverse(animimg, dsc, num);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置反向播放时长
@param self 动画图片实例或指针
@param duration 时长(毫秒)
@return self
@usage anim:set_reverse_duration(100)
*/
static int iot_lvgl_animimage_set_reverse_duration(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t duration = (uint32_t)luaL_checkinteger(L, 2);
    lv_animimg_set_reverse_duration(animimg, duration);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置反向播放延迟
@param self 动画图片实例或指针
@param delay 延迟(毫秒)
@return self
@usage anim:set_reverse_delay(200)
*/
static int iot_lvgl_animimage_set_reverse_delay(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t duration = (uint32_t)luaL_checkinteger(L, 2);
    lv_animimg_set_reverse_delay(animimg, duration);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取图片源数量
@param self 动画图片实例或指针
@return integer 图片源数量
@usage local count = anim:get_src_count()
*/
static int iot_lvgl_animimage_get_src_count(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    uint8_t count = lv_animimg_get_src_count(animimg);
    lua_pushinteger(L, count);
    return 1;
}

/*
获取底层动画对象
@param self 动画图片实例或指针
@return lightuserdata 动画对象指针
@usage local anim = anim:get_anim()
*/
static int iot_lvgl_animimage_get_anim(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    lv_anim_t* anim = lv_animimg_get_anim(animimg);
    lua_pushlightuserdata(L, anim);
    return 1;
}

/*
删除动画
@param self 动画图片实例或指针
@return boolean 是否成功删除
@usage anim:delete()
*/
static int iot_lvgl_animimage_delete(lua_State* L) {
    lv_obj_t* animimg = iot_lvgl_get_obj_ptr(L, 1);
    bool ret = lv_animimg_delete(animimg);
    lua_pushboolean(L, ret);
    return 1;
}

/* 注册 animimage 子模块 */
void iot_lvgl_register_animimage(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法(可以instance:method()调用) */
    REG_METHOD(L, "set_src", iot_lvgl_animimage_set_src);
    REG_METHOD(L, "start", iot_lvgl_animimage_start);
    REG_METHOD(L, "set_duration", iot_lvgl_animimage_set_duration);
    REG_METHOD(L, "set_repeat_count", iot_lvgl_animimage_set_repeat_count);
    REG_METHOD(L, "get_src", iot_lvgl_animimage_get_src);
    REG_METHOD(L, "get_duration", iot_lvgl_animimage_get_duration);
    REG_METHOD(L, "get_repeat_count", iot_lvgl_animimage_get_repeat_count);
    REG_METHOD(L, "set_src_reverse", iot_lvgl_animimage_set_src_reverse);
    REG_METHOD(L, "set_reverse_duration", iot_lvgl_animimage_set_reverse_duration);
    REG_METHOD(L, "set_reverse_delay", iot_lvgl_animimage_set_reverse_delay);
    REG_METHOD(L, "get_src_count", iot_lvgl_animimage_get_src_count);
    REG_METHOD(L, "get_anim", iot_lvgl_animimage_get_anim);
    REG_METHOD(L, "delete", iot_lvgl_animimage_delete);

    /* 保存组件metatable引用(用于继承) */
    animimage_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.animimage.set_src(anim, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, animimage_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.animimage) */
    REG_METHOD(L, "create", iot_lvgl_animimage_create);
}