/*
@module  lvgl.font
@summary LVGL字体工具
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例
local lvgl = require("lvgl")

-- 获取内置字体
local font12 = lvgl.font.get("montserrat_12")
local font14 = lvgl.font.get("montserrat_14")
local font16 = lvgl.font.get("montserrat_16")
local font24 = lvgl.font.get("montserrat_24")
local font48 = lvgl.font.get("montserrat_48")

-- 使用字体设置样式
lvgl.style.set_text_font(my_style, font16)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== 字体操作 ==================== */

/*
获取字体
@param name 字体名称(如"montserrat_12","montserrat_14","montserrat_16"等)
@return userdata 字体指针
@usage local font = lvgl.font.get("montserrat_14")
*/
static int lvgl_font_get(lua_State* L) {
    const char* name = luaL_checkstring(L, 1);
    const lv_font_t* font = NULL;

    if (0) {
#if LV_FONT_MONTSERRAT_8
    } else if (strcmp(name, "montserrat_8") == 0) {
        font = &lv_font_montserrat_8;
#endif
#if LV_FONT_MONTSERRAT_10
    } else if (strcmp(name, "montserrat_10") == 0) {
        font = &lv_font_montserrat_10;
#endif
#if LV_FONT_MONTSERRAT_12
    } else if (strcmp(name, "montserrat_12") == 0) {
        font = &lv_font_montserrat_12;
#endif
#if LV_FONT_MONTSERRAT_14
    } else if (strcmp(name, "montserrat_14") == 0) {
        font = &lv_font_montserrat_14;
#endif
#if LV_FONT_MONTSERRAT_16
    } else if (strcmp(name, "montserrat_16") == 0) {
        font = &lv_font_montserrat_16;
#endif
#if LV_FONT_MONTSERRAT_18
    } else if (strcmp(name, "montserrat_18") == 0) {
        font = &lv_font_montserrat_18;
#endif
#if LV_FONT_MONTSERRAT_20
    } else if (strcmp(name, "montserrat_20") == 0) {
        font = &lv_font_montserrat_20;
#endif
#if LV_FONT_MONTSERRAT_22
    } else if (strcmp(name, "montserrat_22") == 0) {
        font = &lv_font_montserrat_22;
#endif
#if LV_FONT_MONTSERRAT_24
    } else if (strcmp(name, "montserrat_24") == 0) {
        font = &lv_font_montserrat_24;
#endif
#if LV_FONT_MONTSERRAT_26
    } else if (strcmp(name, "montserrat_26") == 0) {
        font = &lv_font_montserrat_26;
#endif
#if LV_FONT_MONTSERRAT_28
    } else if (strcmp(name, "montserrat_28") == 0) {
        font = &lv_font_montserrat_28;
#endif
#if LV_FONT_MONTSERRAT_30
    } else if (strcmp(name, "montserrat_30") == 0) {
        font = &lv_font_montserrat_30;
#endif
#if LV_FONT_MONTSERRAT_32
    } else if (strcmp(name, "montserrat_32") == 0) {
        font = &lv_font_montserrat_32;
#endif
#if LV_FONT_MONTSERRAT_34
    } else if (strcmp(name, "montserrat_34") == 0) {
        font = &lv_font_montserrat_34;
#endif
#if LV_FONT_MONTSERRAT_36
    } else if (strcmp(name, "montserrat_36") == 0) {
        font = &lv_font_montserrat_36;
#endif
#if LV_FONT_MONTSERRAT_38
    } else if (strcmp(name, "montserrat_38") == 0) {
        font = &lv_font_montserrat_38;
#endif
#if LV_FONT_MONTSERRAT_40
    } else if (strcmp(name, "montserrat_40") == 0) {
        font = &lv_font_montserrat_40;
#endif
#if LV_FONT_MONTSERRAT_42
    } else if (strcmp(name, "montserrat_42") == 0) {
        font = &lv_font_montserrat_42;
#endif
#if LV_FONT_MONTSERRAT_44
    } else if (strcmp(name, "montserrat_44") == 0) {
        font = &lv_font_montserrat_44;
#endif
#if LV_FONT_MONTSERRAT_46
    } else if (strcmp(name, "montserrat_46") == 0) {
        font = &lv_font_montserrat_46;
#endif
#if LV_FONT_MONTSERRAT_48
    } else if (strcmp(name, "montserrat_48") == 0) {
        font = &lv_font_montserrat_48;
#endif
    } else {
        font = LV_FONT_DEFAULT;
    }

    lua_pushlightuserdata(L, (void*)font);
    return 1;
}

/* 注册 font 子模块 */
void lvgl_register_font(lua_State* L) {
    REG_METHOD(L, "get", lvgl_font_get);
}
