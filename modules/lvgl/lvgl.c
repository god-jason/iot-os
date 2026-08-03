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

#if LV_USE_SDL
#include "drivers/sdl/lv_sdl_window.h"
#include "drivers/sdl/lv_sdl_mouse.h"
#include "drivers/sdl/lv_sdl_mousewheel.h"
#include "drivers/sdl/lv_sdl_keyboard.h"
#endif



static int iot_lvgl_scr_act(lua_State* L) {

    lv_obj_t* scr = lv_screen_active();

    lua_pushlightuserdata(L, scr);

    return 1;

}



static int iot_lvgl_scr_load(lua_State* L) {

    lv_obj_t* scr = iot_lvgl_get_obj_ptr(L, 1);

    lv_screen_load(scr);

    return 0;

}



static int iot_lvgl_tick_inc(lua_State* L) {

    uint32_t tick_ms = (uint32_t)luaL_checkinteger(L, 1);

    lv_tick_inc(tick_ms);

    return 0;

}



static int iot_lvgl_task_handler(lua_State* L) {
    /* LVGL 9 内置 SDL 驱动在 lv_timer_handler 中自动处理 SDL 事件，
     * 窗口关闭时通过 LV_SDL_DIRECT_EXIT 自动 exit(0) */
    lv_timer_handler();
    return 0;
}



static int iot_lvgl_flush_ready(lua_State* L) {
    lv_display_t* disp = lv_display_get_default();
    if (disp) {
        lv_display_flush_ready(disp);
    }
    (void)L;
    return 0;
}



static int iot_lvgl_refr_now(lua_State* L) {

    lv_refr_now((lv_display_t*)luaL_optlightuserdata(L, 1, NULL));

    return 0;

}



static int iot_lvgl_debug_info_print(lua_State* L) {

    /* lv_debug_info_print() was removed in LVGL 8 */

    (void)L;

    return 0;

}



static int iot_lvgl_version_get(lua_State* L) {

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



static int iot_lvgl_tick_get(lua_State* L) {

    lua_pushinteger(L, lv_tick_get());

    return 1;

}



static int iot_lvgl_disp_get_default(lua_State* L) {

    lua_pushlightuserdata(L, lv_display_get_default());

    return 1;

}



static int iot_lvgl_disp_get_scr_act(lua_State* L) {

    lv_display_t* disp = (lv_display_t*)luaL_optlightuserdata(L, 1, NULL);

    lua_pushlightuserdata(L, lv_display_get_screen_active(disp));

    return 1;

}



static int iot_lvgl_disp_load_scr(lua_State* L) {

    lv_obj_t* scr = iot_lvgl_get_obj_ptr(L, 1);

    lv_screen_load(scr);

    return 0;

}



static int iot_lvgl_indev_get_default(lua_State* L) {

    lua_pushlightuserdata(L, lv_indev_get_next(NULL));

    return 1;

}



static int iot_lvgl_pct(lua_State* L) {
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
static int iot_lvgl_init(lua_State* L) {
    int hor_res = (int)luaL_optinteger(L, 1, 800);
    int ver_res = (int)luaL_optinteger(L, 2, 480);

    /* lv_init 已在 luaopen_lvgl 中完成，此处幂等保证 */
    lv_init();

#if LV_USE_SDL
    /* 使用 LVGL 9 内置 SDL 驱动创建窗口和输入设备 */

    lv_display_t* disp = lv_sdl_window_create(hor_res, ver_res);
    if (!disp) {
        return luaL_error(L, "SDL window create failed: %dx%d", hor_res, ver_res);
    }

    /* 创建鼠标、鼠标滚轮和键盘输入设备 */
    lv_sdl_mouse_create();
    lv_sdl_mousewheel_create();
    lv_indev_t* kb_indev = lv_sdl_keyboard_create();

    /* 创建默认 group 并关联键盘 indev，使 textarea 等控件可通过键盘编辑 */
    if (kb_indev) {
        lv_group_t* group = lv_group_create();
        lv_group_set_default(group);
        lv_indev_set_group(kb_indev, group);
    }

#else
    /* 非 SDL 平台：留作扩展点 */
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

    /* 创建主表 */

    lua_createtable(L, 0, 0);



    /* 注册基础对象系统(必须在组件之前) */

    iot_lvgl_register_obj(L);

    lua_setfield(L, -2, "obj");



    /* 注册基础功能 */

    lua_newtable(L);  /* style 子表 */

    iot_lvgl_register_style(L);

    lua_setfield(L, -2, "style");



    lua_newtable(L);  /* anim 子表 */

    iot_lvgl_register_anim(L);

    lua_setfield(L, -2, "anim");



    lua_newtable(L);  /* color 子表 */

    iot_lvgl_register_color(L);

    lua_setfield(L, -2, "color");



    lua_newtable(L);  /* font 子表 */

    iot_lvgl_register_font(L);

    lua_setfield(L, -2, "font");



    lua_newtable(L);  /* disp 子表 */

    iot_lvgl_register_disp(L);

    lua_setfield(L, -2, "disp");



    lua_newtable(L);  /* indev 子表 */

    iot_lvgl_register_indev(L);

    lua_setfield(L, -2, "indev");



    lua_newtable(L);  /* timer 子表 */

    iot_lvgl_register_timer(L);

    lua_setfield(L, -2, "timer");



    lua_newtable(L);  /* theme 子表 */

    iot_lvgl_register_theme(L);

    lua_setfield(L, -2, "theme");



    lua_newtable(L);  /* fs 子表 */

    iot_lvgl_register_fs(L);

    lua_setfield(L, -2, "fs");



    /* 注册基础控件 */

    lua_newtable(L);  /* btn 子表 */

    iot_lvgl_register_btn(L);

    lua_setfield(L, -2, "btn");



    lua_newtable(L);  /* label 子表 */

    iot_lvgl_register_label(L);

    lua_setfield(L, -2, "label");



    lua_newtable(L);  /* img 子表 */

    iot_lvgl_register_img(L);

    lua_setfield(L, -2, "img");



    lua_newtable(L);  /* line 子表 */

    iot_lvgl_register_line(L);

    lua_setfield(L, -2, "line");



    lua_newtable(L);  /* arc 子表 */

    iot_lvgl_register_arc(L);

    lua_setfield(L, -2, "arc");



    lua_newtable(L);  /* bar 子表 */

    iot_lvgl_register_bar(L);

    lua_setfield(L, -2, "bar");



    /* 注册输入控件 */

    lua_newtable(L);  /* slider 子表 */

    iot_lvgl_register_slider(L);

    lua_setfield(L, -2, "slider");



    lua_newtable(L);  /* checkbox 子表 */

    iot_lvgl_register_checkbox(L);

    lua_setfield(L, -2, "checkbox");



    lua_newtable(L);  /* switch 子表 */

    iot_lvgl_register_switch(L);

    lua_setfield(L, -2, "switch");



    lua_newtable(L);  /* dropdown 子表 */

    iot_lvgl_register_dropdown(L);

    lua_setfield(L, -2, "dropdown");



    lua_newtable(L);  /* roller 子表 */

    iot_lvgl_register_roller(L);

    lua_setfield(L, -2, "roller");



    lua_newtable(L);  /* textarea 子表 */

    iot_lvgl_register_textarea(L);

    lua_setfield(L, -2, "textarea");



    lua_newtable(L);  /* keyboard 子表 */

    iot_lvgl_register_keyboard(L);

    lua_setfield(L, -2, "keyboard");



    lua_newtable(L);  /* spinbox 子表 */

    iot_lvgl_register_spinbox(L);

    lua_setfield(L, -2, "spinbox");



    /* 注册高级控件 */

    lua_newtable(L);  /* chart 子表 */

    iot_lvgl_register_chart(L);

    lua_setfield(L, -2, "chart");



    lua_newtable(L);  /* table 子表 */

    iot_lvgl_register_table(L);

    lua_setfield(L, -2, "table");



    /* LVGL 9 移除 lv_meter，由 lv_scale 替代，meter 子模块暂不注册 */
    /* lua_newtable(L); */  /* meter 子表 */

    /* iot_lvgl_register_meter(L); */

    /* lua_setfield(L, -2, "meter"); */



    lua_newtable(L);  /* list 子表 */

    iot_lvgl_register_list(L);

    lua_setfield(L, -2, "list");



    lua_newtable(L);  /* menu 子表 */

    iot_lvgl_register_menu(L);

    lua_setfield(L, -2, "menu");



    lua_newtable(L);  /* win 子表 */

    iot_lvgl_register_win(L);

    lua_setfield(L, -2, "win");



    lua_newtable(L);  /* tabview 子表 */

    iot_lvgl_register_tabview(L);

    lua_setfield(L, -2, "tabview");



    lua_newtable(L);  /* tileview 子表 */

    iot_lvgl_register_tileview(L);

    lua_setfield(L, -2, "tileview");



    lua_newtable(L);  /* calendar 子表 */

    iot_lvgl_register_calendar(L);

    lua_setfield(L, -2, "calendar");



    lua_newtable(L);  /* colorwheel 子表 */

    iot_lvgl_register_colorwheel(L);

    lua_setfield(L, -2, "colorwheel");



    lua_newtable(L);  /* msgbox 子表 */

    iot_lvgl_register_msgbox(L);

    lua_setfield(L, -2, "msgbox");



    lua_newtable(L);  /* spinner 子表 */

    iot_lvgl_register_spinner(L);

    lua_setfield(L, -2, "spinner");



    lua_newtable(L);  /* canvas 子表 */

    iot_lvgl_register_canvas(L);

    lua_setfield(L, -2, "canvas");



    lua_newtable(L);  /* layout 子表 */

    iot_lvgl_register_layout(L);

    lua_setfield(L, -2, "layout");



    lua_newtable(L);  /* group 子表 */

    iot_lvgl_register_group(L);

    lua_setfield(L, -2, "group");



    lua_newtable(L);  /* buttonmatrix 子表 */

    iot_lvgl_register_buttonmatrix(L);

    lua_setfield(L, -2, "buttonmatrix");



    lua_newtable(L);  /* imagebutton 子表 */

    iot_lvgl_register_imagebutton(L);

    lua_setfield(L, -2, "imagebutton");



    lua_newtable(L);  /* animimage 子表 */

    iot_lvgl_register_animimage(L);

    lua_setfield(L, -2, "animimage");



    lua_newtable(L);  /* arclabel 子表 */

    iot_lvgl_register_arclabel(L);

    lua_setfield(L, -2, "arclabel");



    lua_newtable(L);  /* gif 子表 */

    iot_lvgl_register_gif(L);

    lua_setfield(L, -2, "gif");



    lua_newtable(L);  /* ime_pinyin 子表 */

    iot_lvgl_register_ime_pinyin(L);

    lua_setfield(L, -2, "ime_pinyin");



    lua_newtable(L);  /* scale 子表 */

    iot_lvgl_register_scale(L);

    lua_setfield(L, -2, "scale");



    lua_newtable(L);  /* span 子表 */

    iot_lvgl_register_span(L);

    lua_setfield(L, -2, "span");



    lua_newtable(L);  /* 3dtexture 子表 */

    iot_lvgl_register_3dtexture(L);

    lua_setfield(L, -2, "3dtexture");



    lua_newtable(L);  /* lottie 子表 */

    iot_lvgl_register_lottie(L);

    lua_setfield(L, -2, "lottie");



    /* 定义常量 */
    iot_lvgl_define_constants(L);



    REG_METHOD(L, "init", iot_lvgl_init);

    REG_METHOD(L, "scr_act", iot_lvgl_scr_act);

    REG_METHOD(L, "scr_load", iot_lvgl_scr_load);

    REG_METHOD(L, "tick_inc", iot_lvgl_tick_inc);

    REG_METHOD(L, "task_handler", iot_lvgl_task_handler);

    REG_METHOD(L, "flush_ready", iot_lvgl_flush_ready);

    REG_METHOD(L, "refr_now", iot_lvgl_refr_now);

    REG_METHOD(L, "debug_info_print", iot_lvgl_debug_info_print);

    REG_METHOD(L, "version_get", iot_lvgl_version_get);

    REG_METHOD(L, "tick_get", iot_lvgl_tick_get);

    REG_METHOD(L, "disp_get_default", iot_lvgl_disp_get_default);

    REG_METHOD(L, "disp_get_scr_act", iot_lvgl_disp_get_scr_act);

    REG_METHOD(L, "disp_load_scr", iot_lvgl_disp_load_scr);

    REG_METHOD(L, "indev_get_default", iot_lvgl_indev_get_default);

    REG_METHOD(L, "pct", iot_lvgl_pct);

    return 1;
}
