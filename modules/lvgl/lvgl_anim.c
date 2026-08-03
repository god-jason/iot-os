/**
 * @file iot_lvgl_anim.c
 * @brief LVGL动画系统
 *
 * 实现LVGL动画系统的Lua绑定，包括动画初始化、删除、变量绑定、起止值设置、时间设置、路径回调设置、动画启动/停止/删除等接口，支持多种动画缓动效果（线性、ease-in、ease-out、ease-in-out、overshoot、bounce、step）。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== 动画操作 ==================== */

/*
初始化动画
@return userdata 动画指针
@usage local anim = lvgl.anim.init()
*/
static int iot_lvgl_anim_init(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)cm_malloc(sizeof(lv_anim_t));
    if (!anim) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    lv_anim_init(anim);
    lua_pushlightuserdata(L, anim);
    return 1;
}

/*
删除动画(仅释放内存)
@param anim 动画指针
@usage lvgl.anim.delete(anim)
*/
static int iot_lvgl_anim_delete(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    cm_free(anim);
    return 0;
}

/*
设置动画关联的对象
@param anim 动画指针
@param var 对象指针
@usage lvgl.anim.set_var(anim, btn)
*/
static int iot_lvgl_anim_set_var(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    void* var = (void*)luaL_checklightuserdata(L, 2);
    lv_anim_set_var(anim, var);
    return 0;
}

/*
设置动画值范围
@param anim 动画指针
@param start 起始值
@param end 结束值
@usage lvgl.anim.set_values(anim, 0, 100)
*/
static int iot_lvgl_anim_set_values(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    int32_t start = (int32_t)luaL_checkinteger(L, 2);
    int32_t end = (int32_t)luaL_checkinteger(L, 3);
    lv_anim_set_values(anim, start, end);
    return 0;
}

/*
设置动画持续时间
@param anim 动画指针
@param duration 持续时间(毫秒)
@usage lvgl.anim.set_time(anim, 500)
*/
static int iot_lvgl_anim_set_time(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    uint32_t duration = (uint32_t)luaL_checkinteger(L, 2);
    lv_anim_set_time(anim, duration);
    return 0;
}

/*
设置动画延迟开始时间
@param anim 动画指针
@param delay 延迟时间(毫秒)
@usage lvgl.anim.set_delay(anim, 1000)
*/
static int iot_lvgl_anim_set_delay(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    uint32_t delay = (uint32_t)luaL_checkinteger(L, 2);
    lv_anim_set_delay(anim, delay);
    return 0;
}

/*
设置动画回放时间
@param anim 动画指针
@param duration 回放持续时间(毫秒)
@usage lvgl.anim.set_playback_time(anim, 300)
*/
static int iot_lvgl_anim_set_playback_time(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    uint32_t duration = (uint32_t)luaL_checkinteger(L, 2);
    lv_anim_set_playback_time(anim, duration);
    return 0;
}

/*
设置动画重复次数
@param anim 动画指针
@param count 重复次数(LV_ANIM_REPEAT_INFINITE表示无限)
@usage lvgl.anim.set_repeat_count(anim, 3)
*/
static int iot_lvgl_anim_set_repeat_count(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    uint16_t count = (uint16_t)luaL_checkinteger(L, 2);
    lv_anim_set_repeat_count(anim, count);
    return 0;
}

/*
设置动画路径类型
@param anim 动画指针
@param path_type 路径类型: 0-linear, 1-ease_in, 2-ease_out, 3-ease_in_out, 4-overshoot, 5-bounce, 6-step
@usage lvgl.anim.set_path_cb(anim, 3)  -- ease_in_out
*/
static int iot_lvgl_anim_set_path_cb(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    int path_type = (int)luaL_checkinteger(L, 2);

    lv_anim_path_cb_t path_cb;
    switch (path_type) {
        case 0: path_cb = lv_anim_path_linear; break;
        case 1: path_cb = lv_anim_path_ease_in; break;
        case 2: path_cb = lv_anim_path_ease_out; break;
        case 3: path_cb = lv_anim_path_ease_in_out; break;
        case 4: path_cb = lv_anim_path_overshoot; break;
        case 5: path_cb = lv_anim_path_bounce; break;
        case 6: path_cb = lv_anim_path_step; break;
        default: path_cb = lv_anim_path_linear; break;
    }
    lv_anim_set_path_cb(anim, path_cb);
    return 0;
}

/*
启动动画
@param anim 动画指针
@usage lvgl.anim.start(anim)
*/
static int iot_lvgl_anim_start(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    lv_anim_start(anim);
    return 0;
}

/*
删除指定对象的动画
@param var 对象指针
@usage lvgl.anim.del(some_object)
*/
static int iot_lvgl_anim_del(lua_State* L) {
    void* var = (void*)luaL_checklightuserdata(L, 1);
    lv_anim_del(var, NULL);
    return 0;
}

/*
删除所有动画
@usage lvgl.anim.del_all()
*/
static int iot_lvgl_anim_del_all(lua_State* L) {
    lv_anim_del_all();
    return 0;
}

/*
设置动画重复延迟
@param anim 动画指针
@param delay_ms 重复延迟时间(毫秒)
@usage lvgl.anim.set_repeat_delay(anim, 500)
*/
static int iot_lvgl_anim_set_repeat_delay(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    uint32_t delay = (uint32_t)luaL_checkinteger(L, 2);
    lv_anim_set_repeat_delay(anim, delay);
    return 0;
}

/*
设置动画回放延迟
@param anim 动画指针
@param delay_ms 回放延迟时间(毫秒)
@usage lvgl.anim.set_playback_delay(anim, 300)
*/
static int iot_lvgl_anim_set_playback_delay(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    uint32_t delay = (uint32_t)luaL_checkinteger(L, 2);
    lv_anim_set_playback_delay(anim, delay);
    return 0;
}

/*
设置动画反向播放
@param anim 动画指针
@param reverse_bool 是否反向播放(true/false)
@usage lvgl.anim.set_reverse(anim, true)
*/
static int iot_lvgl_anim_set_reverse(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    int reverse = lua_toboolean(L, 2);
    lv_anim_set_reverse_duration(anim, reverse ? anim->duration : 0);
    return 0;
}

/*
设置是否立即应用起始值
@param anim 动画指针
@param early_apply_bool 是否立即应用(true/false)
@usage lvgl.anim.set_early_apply(anim, true)
*/
static int iot_lvgl_anim_set_early_apply(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    int early_apply = lua_toboolean(L, 2);
    lv_anim_set_early_apply(anim, early_apply ? true : false);
    return 0;
}

/*
获取动画当前值
@param anim 动画指针
@return integer 当前值
@usage local val = lvgl.anim.get_value(anim)
*/
static int iot_lvgl_anim_get_value(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    lua_pushinteger(L, anim->current_value);
    return 1;
}

/*
检查动画是否正在运行
@param anim 动画指针
@return boolean 是否正在运行
@usage local running = lvgl.anim.is_running(anim)
*/
static int iot_lvgl_anim_is_running(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    lua_pushboolean(L, anim->act_time >= 0);
    return 1;
}

/* 注册 anim 子模块 */
void iot_lvgl_register_anim(lua_State* L) {
    REG_METHOD(L, "init", iot_lvgl_anim_init);
    REG_METHOD(L, "delete", iot_lvgl_anim_delete);
    REG_METHOD(L, "set_var", iot_lvgl_anim_set_var);
    REG_METHOD(L, "set_values", iot_lvgl_anim_set_values);
    REG_METHOD(L, "set_time", iot_lvgl_anim_set_time);
    REG_METHOD(L, "set_delay", iot_lvgl_anim_set_delay);
    REG_METHOD(L, "set_playback_time", iot_lvgl_anim_set_playback_time);
    REG_METHOD(L, "set_repeat_count", iot_lvgl_anim_set_repeat_count);
    REG_METHOD(L, "set_path_cb", iot_lvgl_anim_set_path_cb);
    REG_METHOD(L, "start", iot_lvgl_anim_start);
    REG_METHOD(L, "del", iot_lvgl_anim_del);
    REG_METHOD(L, "del_all", iot_lvgl_anim_del_all);
    REG_METHOD(L, "set_repeat_delay", iot_lvgl_anim_set_repeat_delay);
    REG_METHOD(L, "set_playback_delay", iot_lvgl_anim_set_playback_delay);
    REG_METHOD(L, "set_reverse", iot_lvgl_anim_set_reverse);
    REG_METHOD(L, "set_early_apply", iot_lvgl_anim_set_early_apply);
    REG_METHOD(L, "get_value", iot_lvgl_anim_get_value);
    REG_METHOD(L, "is_running", iot_lvgl_anim_is_running);
}
