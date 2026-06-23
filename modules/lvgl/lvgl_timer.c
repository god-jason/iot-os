/*
@module  lvgl.timer
@summary LVGL??????
@version 2.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
@tag     System
@usage
-- Lua??
local lvgl = require("lvgl")

-- ??: LVGL????????????
-- ??Lua???????????????????????

-- ??????
-- lvgl.timer.delete(timer)

-- ??????
-- lvgl.timer.pause(timer)

-- ??????
-- lvgl.timer.resume(timer)

-- ????????
-- lvgl.timer.set_period(timer, 1000)

-- ??????
-- lvgl.timer.reset(timer)

-- ????????
-- lvgl.timer.ready(timer)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== ??????==================== */

/*
??????
@param timer ??????
@usage lvgl.timer.delete(timer)
*/
static int lvgl_timer_delete(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_del(timer);
    return 0;
}

/*
??????
@param timer ??????
@usage lvgl.timer.pause(timer)
*/
static int lvgl_timer_pause(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_pause(timer);
    return 0;
}

/*
??????
@param timer ??????
@usage lvgl.timer.resume(timer)
*/
static int lvgl_timer_resume(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_resume(timer);
    return 0;
}

/*
????????
@param timer ??????
@param period ??(??)
@usage lvgl.timer.set_period(timer, 1000)
*/
static int lvgl_timer_set_period(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    uint32_t period = (uint32_t)luaL_checkinteger(L, 2);
    lv_timer_set_period(timer, period);
    return 0;
}

/*
????????
@param timer ??????
@return number ??(??)
@usage local period = lvgl.timer.get_period(timer)
*/
static int lvgl_timer_get_period(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lua_pushinteger(L, timer->period);
    return 1;
}

/*
??????
@param timer ??????
@usage lvgl.timer.reset(timer)
*/
static int lvgl_timer_reset(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_reset(timer);
    return 0;
}

/*
????????
@param timer ??????
@usage lvgl.timer.ready(timer)
*/
static int lvgl_timer_ready(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lv_timer_ready(timer);
    return 0;
}

/*
??????????
@param timer ??????
@return number ????(??)
@usage local time = lvgl.timer.get_time(timer)
*/
static int lvgl_timer_get_time(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    uint32_t elapsed = lv_tick_elaps(timer->last_run);
    uint32_t remaining = (elapsed >= timer->period) ? 0 : (timer->period - elapsed);
    lua_pushinteger(L, remaining);
    return 1;
}

/*
??????????
@param timer ??????
@param repeat_count ????(-1??????)
@usage lvgl.timer.set_repeat_count(timer, -1)
*/
static int lvgl_timer_set_repeat_count(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    int32_t repeat_count = (int32_t)luaL_checkinteger(L, 2);
    lv_timer_set_repeat_count(timer, repeat_count);
    return 0;
}

/*
??????????
@param timer ??????
@return number ????
@usage local count = lvgl.timer.get_repeat_count(timer)
*/
static int lvgl_timer_get_repeat_count(lua_State* L) {
    lv_timer_t* timer = (lv_timer_t*)luaL_checklightuserdata(L, 1);
    lua_pushinteger(L, timer->repeat_count);
    return 1;
}

/* ?? timer ????*/
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
