/*
@module  lvgl.anim
@summary LVGL动画系统
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例
local lvgl = require("lvgl")

-- 创建动画
local anim = lvgl.anim.init()
lvgl.anim.set_var(anim, some_object)
lvgl.anim.set_values(anim, 0, 100)
lvgl.anim.set_time(anim, 500)
lvgl.anim.set_path_cb(anim, 3)  -- ease_in_out
lvgl.anim.start(anim)

-- 删除动画
lvgl.anim.del(some_object)
lvgl.anim.del_all()
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* ==================== 动画操作 ==================== */

/*
初始化动画
@return userdata 动画指针
@usage local anim = lvgl.anim.init()
*/
static int lvgl_anim_init(lua_State* L) {
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
static int lvgl_anim_delete(lua_State* L) {
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
static int lvgl_anim_set_var(lua_State* L) {
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
static int lvgl_anim_set_values(lua_State* L) {
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
static int lvgl_anim_set_time(lua_State* L) {
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
static int lvgl_anim_set_delay(lua_State* L) {
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
static int lvgl_anim_set_playback_time(lua_State* L) {
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
static int lvgl_anim_set_repeat_count(lua_State* L) {
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
static int lvgl_anim_set_path_cb(lua_State* L) {
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
static int lvgl_anim_start(lua_State* L) {
    lv_anim_t* anim = (lv_anim_t*)luaL_checklightuserdata(L, 1);
    lv_anim_start(anim);
    return 0;
}

/*
删除指定对象的动画
@param var 对象指针
@usage lvgl.anim.del(some_object)
*/
static int lvgl_anim_del(lua_State* L) {
    void* var = (void*)luaL_checklightuserdata(L, 1);
    lv_anim_del(var, NULL);
    return 0;
}

/*
删除所有动画
@usage lvgl.anim.del_all()
*/
static int lvgl_anim_del_all(lua_State* L) {
    lv_anim_del_all();
    return 0;
}

/* 注册 anim 子模块 */
void lvgl_register_anim(lua_State* L) {
    REG_METHOD(L, "init", lvgl_anim_init);
    REG_METHOD(L, "delete", lvgl_anim_delete);
    REG_METHOD(L, "set_var", lvgl_anim_set_var);
    REG_METHOD(L, "set_values", lvgl_anim_set_values);
    REG_METHOD(L, "set_time", lvgl_anim_set_time);
    REG_METHOD(L, "set_delay", lvgl_anim_set_delay);
    REG_METHOD(L, "set_playback_time", lvgl_anim_set_playback_time);
    REG_METHOD(L, "set_repeat_count", lvgl_anim_set_repeat_count);
    REG_METHOD(L, "set_path_cb", lvgl_anim_set_path_cb);
    REG_METHOD(L, "start", lvgl_anim_start);
    REG_METHOD(L, "del", lvgl_anim_del);
    REG_METHOD(L, "del_all", lvgl_anim_del_all);
}
