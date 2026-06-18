/*
@module  lvgl.timer
@summary LVGL定时器操作
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     System
@usage
-- Lua示例
local lvgl = require("lvgl")

-- 注意: LVGL定时器需要设置回调函数
-- 由于Lua回调机制的限制,当前版本仅支持基础的定时器操作

-- 删除定时器
-- lvgl.timer.delete(timer)

-- 暂停定时器
-- lvgl.timer.pause(timer)

-- 恢复定时器
-- lvgl.timer.resume(timer)

-- 设置定时器周期
-- lvgl.timer.set_period(timer, 1000)

-- 重置定时器
-- lvgl.timer.reset(timer)

-- 使定时器立即就绪
-- lvgl.timer.ready(timer)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* ==================== 定时器操作 ==================== */

/*
删除定时器
@param timer 定时器指针
@usage lvgl.timer.delete(timer)
*/
static int lvgl_timer_delete(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_delete(timer);
    return 0;
}

/*
暂停定时器
@param timer 定时器指针
@usage lvgl.timer.pause(timer)
*/
static int lvgl_timer_pause(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_pause(timer);
    return 0;
}

/*
恢复定时器
@param timer 定时器指针
@usage lvgl.timer.resume(timer)
*/
static int lvgl_timer_resume(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_resume(timer);
    return 0;
}

/*
设置定时器周期
@param timer 定时器指针
@param period 周期(毫秒)
@usage lvgl.timer.set_period(timer, 1000)
*/
static int lvgl_timer_set_period(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    uint32_t period = (uint32_t)luaL_checkinteger(L, 2);
    lv_timer_set_period(timer, period);
    return 0;
}

/*
获取定时器周期
@param timer 定时器指针
@return number 周期(毫秒)
@usage local period = lvgl.timer.get_period(timer)
*/
static int lvgl_timer_get_period(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    uint32_t period = lv_timer_get_period(timer);
    lua_pushinteger(L, period);
    return 1;
}

/*
重置定时器
@param timer 定时器指针
@usage lvgl.timer.reset(timer)
*/
static int lvgl_timer_reset(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_reset(timer);
    return 0;
}

/*
使定时器立即就绪
@param timer 定时器指针
@usage lvgl.timer.ready(timer)
*/
static int lvgl_timer_ready(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_ready(timer);
    return 0;
}

/*
获取定时器剩余时间
@param timer 定时器指针
@return number 剩余时间(毫秒)
@usage local time = lvgl.timer.get_time(timer)
*/
static int lvgl_timer_get_time(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    uint32_t time = lv_timer_get_time(timer);
    lua_pushinteger(L, time);
    return 1;
}

/*
设置定时器重复次数
@param timer 定时器指针
@param repeat_count 重复次数(-1表示无限循环)
@usage lvgl.timer.set_repeat_count(timer, -1)
*/
static int lvgl_timer_set_repeat_count(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    int32_t repeat_count = (int32_t)luaL_checkinteger(L, 2);
    lv_timer_set_repeat_count(timer, repeat_count);
    return 0;
}

/*
获取定时器重复次数
@param timer 定时器指针
@return number 重复次数
@usage local count = lvgl.timer.get_repeat_count(timer)
*/
static int lvgl_timer_get_repeat_count(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    int32_t count = lv_timer_get_repeat_count(timer);
    lua_pushinteger(L, count);
    return 1;
}

/* 注册 timer 子模块 */
void lvgl_register_timer(lua_State* L) {
    REG_METHOD(L, "delete", lvgl_timer_delete);
    REG_METHOD(L, "pause", lvgl_timer_pause);
    REG_METHOD(L, "resume", lvgl_timer_resume);
    REG_METHOD(L, "set_period", lvgl_timer_set_period);
    REG_METHOD(L, "get_period", lvgl_timer_get_period);
    REG_METHOD(L, "reset", lvgl_timer_reset);
    REG_METHOD(L, "ready", lvgl_timer_ready);
    REG_METHOD(L, "get_time", lvgl_timer_get_time);
    REG_METHOD(L, "set_repeat_count", lvgl_timer_set_repeat_count);
    REG_METHOD(L, "get_repeat_count", lvgl_timer_get_repeat_count);
}
