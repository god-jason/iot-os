/**
 * @file lvgl.c
 * @brief LVGL图形库主入口模块实现
 *
 * 实现LVGL Lua绑定的主入口功能，包括活动屏幕获取、屏幕加载、时钟节拍递增、任务处理、刷新就绪、立即刷新、版本信息获取、时钟获取、默认显示获取、活动屏幕获取等核心接口，以及主模块注册函数luaopen_lvgl和子模块注册的调度。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl.h"
#include "lvgl_obj.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef LV_USE_SDL_DRV
#include "lv_sdl_drv.h"
#endif



static int lvgl_scr_act(lua_State* L) {

    lv_obj_t* scr = lv_scr_act();

    lua_pushlightuserdata(L, scr);

    return 1;

}



static int lvgl_scr_load(lua_State* L) {

    lv_obj_t* scr = lvgl_get_obj_ptr(L, 1);

    lv_scr_load(scr);

    return 0;

}



static int lvgl_tick_inc(lua_State* L) {

    uint32_t tick_ms = (uint32_t)luaL_checkinteger(L, 1);

    lv_tick_inc(tick_ms);

    return 0;

}



static int lvgl_task_handler(lua_State* L) {

#ifdef LV_USE_SDL_DRV
    /* 桌面平台：先处理 SDL 事件，窗口关闭则退出 */
    if (lv_sdl_drv_is_inited()) {
        if (!lv_sdl_drv_loop()) {
            lv_sdl_drv_deinit();
            exit(0);
        }
    }
#endif

    lv_task_handler();

    return 0;

}



static int lvgl_flush_ready(lua_State* L) {

    lv_disp_t* disp = lv_disp_get_default();

    if (disp && disp->driver) {

        lv_disp_flush_ready(disp->driver);

    }

    (void)L;

    return 0;

}



static int lvgl_refr_now(lua_State* L) {

    lv_refr_now((lv_disp_t*)luaL_optlightuserdata(L, 1, NULL));

    return 0;

}



static int lvgl_debug_info_print(lua_State* L) {

    /* lv_debug_info_print() was removed in LVGL 8 */

    (void)L;

    return 0;

}



static int lvgl_version_get(lua_State* L) {

    lua_newtable(L);

    lua_pushinteger(L, lv_version_major());

    lua_setfield(L, -2, "major");

    lua_pushinteger(L, lv_version_minor());

    lua_setfield(L, -2, "minor");

    lua_pushinteger(L, lv_version_patch());

    lua_setfield(L, -2, "patch");

    lua_pushstring(L, lv_version_info());

    lua_setfield(L, -2, "info");

    return 1;

}



static int lvgl_tick_get(lua_State* L) {

    lua_pushinteger(L, lv_tick_get());

    return 1;

}



static int lvgl_disp_get_default(lua_State* L) {

    lua_pushlightuserdata(L, lv_disp_get_default());

    return 1;

}



static int lvgl_disp_get_scr_act(lua_State* L) {

    lv_disp_t* disp = (lv_disp_t*)luaL_optlightuserdata(L, 1, NULL);

    lua_pushlightuserdata(L, lv_disp_get_scr_act(disp));

    return 1;

}



static int lvgl_disp_load_scr(lua_State* L) {

    lv_obj_t* scr = lvgl_get_obj_ptr(L, 1);

    lv_disp_load_scr(scr);

    return 0;

}



static int lvgl_indev_get_default(lua_State* L) {

    lua_pushlightuserdata(L, lv_indev_get_default());

    return 1;

}



static int lvgl_pct(lua_State* L) {
    int x = (int)luaL_checkinteger(L, 1);
    lua_pushinteger(L, LV_PCT(x));
    return 1;
}

/*
初始化 LVGL 显示与输入驱动
@param width 窗口宽度（像素，可选，默认 800）
@param height 窗口高度（像素，可选，默认 480）
@return boolean 成功返回 true
@usage lvgl.init(800, 480)
@usage lvgl.init()  -- 使用默认 800x480
*/
static int lvgl_init(lua_State* L) {
    int hor_res = (int)luaL_optinteger(L, 1, 800);
    int ver_res = (int)luaL_optinteger(L, 2, 480);

    /* lv_init 已在 luaopen_lvgl 中完成，此处幂等保证 */
    lv_init();

#ifdef LV_USE_SDL_DRV
    if (!lv_sdl_drv_is_inited()) {
        fprintf(stderr, "[lvgl] init: creating SDL window %dx%d\n", hor_res, ver_res);
        fflush(stderr);
        if (!lv_sdl_drv_init(hor_res, ver_res)) {
            return luaL_error(L, "SDL driver init failed: %dx%d", hor_res, ver_res);
        }
        fprintf(stderr, "[lvgl] SDL window created\n");
        fflush(stderr);
    } else {
        /* 已初始化，幂等返回成功 */
        fprintf(stderr, "[lvgl] init: already initialized, skip\n");
        fflush(stderr);
    }
#else
    /* 非 SDL 平台：留作扩展点，目前无操作 */
    (void)hor_res;
    (void)ver_res;
#endif

    lua_pushboolean(L, 1);
    return 1;
}

/* 主模块注册 */

LUAMOD_API int luaopen_lvgl(lua_State* L) {

    /* 初始化 LVGL 核心（幂等，重复调用安全）
     * 注意：SDL 显示驱动不再在此隐式启动，需由 lvgl.init(w, h) 显式创建，
     *       以便 Lua 层控制窗口尺寸 */
    lv_init();
    fprintf(stderr, "[lvgl] lv_init done\n"); fflush(stderr);

    /* 创建主表 */

    lua_createtable(L, 0, 0);



    /* 注册基础对象系统(必须在组件之前) */

    lvgl_register_obj(L);

    lua_setfield(L, -2, "obj");



    /* 注册基础功能 */

    lua_newtable(L);  /* style 子表 */

    lvgl_register_style(L);

    lua_setfield(L, -2, "style");



    lua_newtable(L);  /* anim 子表 */

    lvgl_register_anim(L);

    lua_setfield(L, -2, "anim");



    lua_newtable(L);  /* color 子表 */

    lvgl_register_color(L);

    lua_setfield(L, -2, "color");



    lua_newtable(L);  /* font 子表 */

    lvgl_register_font(L);

    lua_setfield(L, -2, "font");



    lua_newtable(L);  /* disp 子表 */

    lvgl_register_disp(L);

    lua_setfield(L, -2, "disp");



    lua_newtable(L);  /* indev 子表 */

    lvgl_register_indev(L);

    lua_setfield(L, -2, "indev");



    lua_newtable(L);  /* timer 子表 */

    lvgl_register_timer(L);

    lua_setfield(L, -2, "timer");



    lua_newtable(L);  /* theme 子表 */

    lvgl_register_theme(L);

    lua_setfield(L, -2, "theme");



    lua_newtable(L);  /* fs 子表 */

    lvgl_register_fs(L);

    lua_setfield(L, -2, "fs");



    /* 注册基础控件 */

    lua_newtable(L);  /* btn 子表 */

    lvgl_register_btn(L);

    lua_setfield(L, -2, "btn");



    lua_newtable(L);  /* label 子表 */

    lvgl_register_label(L);

    lua_setfield(L, -2, "label");



    lua_newtable(L);  /* img 子表 */

    lvgl_register_img(L);

    lua_setfield(L, -2, "img");



    lua_newtable(L);  /* line 子表 */

    lvgl_register_line(L);

    lua_setfield(L, -2, "line");



    lua_newtable(L);  /* arc 子表 */

    lvgl_register_arc(L);

    lua_setfield(L, -2, "arc");



    lua_newtable(L);  /* bar 子表 */

    lvgl_register_bar(L);

    lua_setfield(L, -2, "bar");



    /* 注册输入控件 */

    lua_newtable(L);  /* slider 子表 */

    lvgl_register_slider(L);

    lua_setfield(L, -2, "slider");



    lua_newtable(L);  /* checkbox 子表 */

    lvgl_register_checkbox(L);

    lua_setfield(L, -2, "checkbox");



    lua_newtable(L);  /* switch 子表 */

    lvgl_register_switch(L);

    lua_setfield(L, -2, "switch");



    lua_newtable(L);  /* dropdown 子表 */

    lvgl_register_dropdown(L);

    lua_setfield(L, -2, "dropdown");



    lua_newtable(L);  /* roller 子表 */

    lvgl_register_roller(L);

    lua_setfield(L, -2, "roller");



    lua_newtable(L);  /* textarea 子表 */

    lvgl_register_textarea(L);

    lua_setfield(L, -2, "textarea");



    lua_newtable(L);  /* keyboard 子表 */

    lvgl_register_keyboard(L);

    lua_setfield(L, -2, "keyboard");



    lua_newtable(L);  /* spinbox 子表 */

    lvgl_register_spinbox(L);

    lua_setfield(L, -2, "spinbox");



    /* 注册高级控件 */

    lua_newtable(L);  /* chart 子表 */

    lvgl_register_chart(L);

    lua_setfield(L, -2, "chart");



    lua_newtable(L);  /* table 子表 */

    lvgl_register_table(L);

    lua_setfield(L, -2, "table");



    lua_newtable(L);  /* meter 子表 */

    lvgl_register_meter(L);

    lua_setfield(L, -2, "meter");



    lua_newtable(L);  /* list 子表 */

    lvgl_register_list(L);

    lua_setfield(L, -2, "list");



    lua_newtable(L);  /* menu 子表 */

    lvgl_register_menu(L);

    lua_setfield(L, -2, "menu");



    lua_newtable(L);  /* win 子表 */

    lvgl_register_win(L);

    lua_setfield(L, -2, "win");



    lua_newtable(L);  /* tabview 子表 */

    lvgl_register_tabview(L);

    lua_setfield(L, -2, "tabview");



    lua_newtable(L);  /* tileview 子表 */

    lvgl_register_tileview(L);

    lua_setfield(L, -2, "tileview");



    lua_newtable(L);  /* calendar 子表 */

    lvgl_register_calendar(L);

    lua_setfield(L, -2, "calendar");



    lua_newtable(L);  /* colorwheel 子表 */

    lvgl_register_colorwheel(L);

    lua_setfield(L, -2, "colorwheel");



    lua_newtable(L);  /* msgbox 子表 */

    lvgl_register_msgbox(L);

    lua_setfield(L, -2, "msgbox");



    lua_newtable(L);  /* spinner 子表 */

    lvgl_register_spinner(L);

    lua_setfield(L, -2, "spinner");



    lua_newtable(L);  /* canvas 子表 */

    lvgl_register_canvas(L);

    lua_setfield(L, -2, "canvas");



    lua_newtable(L);  /* layout 子表 */

    lvgl_register_layout(L);

    lua_setfield(L, -2, "layout");



    lua_newtable(L);  /* group 子表 */

    lvgl_register_group(L);

    lua_setfield(L, -2, "group");



    /* 定义常量 */

    lvgl_define_constants(L);



    REG_METHOD(L, "init", lvgl_init);

    REG_METHOD(L, "scr_act", lvgl_scr_act);

    REG_METHOD(L, "scr_load", lvgl_scr_load);

    REG_METHOD(L, "tick_inc", lvgl_tick_inc);

    REG_METHOD(L, "task_handler", lvgl_task_handler);

    REG_METHOD(L, "flush_ready", lvgl_flush_ready);

    REG_METHOD(L, "refr_now", lvgl_refr_now);

    REG_METHOD(L, "debug_info_print", lvgl_debug_info_print);

    REG_METHOD(L, "version_get", lvgl_version_get);

    REG_METHOD(L, "tick_get", lvgl_tick_get);

    REG_METHOD(L, "disp_get_default", lvgl_disp_get_default);

    REG_METHOD(L, "disp_get_scr_act", lvgl_disp_get_scr_act);

    REG_METHOD(L, "disp_load_scr", lvgl_disp_load_scr);

    REG_METHOD(L, "indev_get_default", lvgl_indev_get_default);

    REG_METHOD(L, "pct", lvgl_pct);

    return 1;
}

