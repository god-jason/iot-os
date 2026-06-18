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

#include "lvgl.h"
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

    /* 根据名称获取内置字体 */
    if (strcmp(name, "montserrat_12") == 0) {
        font = &lv_font_montserrat_12;
    } else if (strcmp(name, "montserrat_14") == 0) {
        font = &lv_font_montserrat_14;
    } else if (strcmp(name, "montserrat_16") == 0) {
        font = &lv_font_montserrat_16;
    } else if (strcmp(name, "montserrat_18") == 0) {
        font = &lv_font_montserrat_18;
    } else if (strcmp(name, "montserrat_20") == 0) {
        font = &lv_font_montserrat_20;
    } else if (strcmp(name, "montserrat_22") == 0) {
        font = &lv_font_montserrat_22;
    } else if (strcmp(name, "montserrat_24") == 0) {
        font = &lv_font_montserrat_24;
    } else if (strcmp(name, "montserrat_26") == 0) {
        font = &lv_font_montserrat_26;
    } else if (strcmp(name, "montserrat_28") == 0) {
        font = &lv_font_montserrat_28;
    } else if (strcmp(name, "montserrat_30") == 0) {
        font = &lv_font_montserrat_30;
    } else if (strcmp(name, "montserrat_32") == 0) {
        font = &lv_font_montserrat_32;
    } else if (strcmp(name, "montserrat_34") == 0) {
        font = &lv_font_montserrat_34;
    } else if (strcmp(name, "montserrat_36") == 0) {
        font = &lv_font_montserrat_36;
    } else if (strcmp(name, "montserrat_38") == 0) {
        font = &lv_font_montserrat_38;
    } else if (strcmp(name, "montserrat_40") == 0) {
        font = &lv_font_montserrat_40;
    } else if (strcmp(name, "montserrat_42") == 0) {
        font = &lv_font_montserrat_42;
    } else if (strcmp(name, "montserrat_44") == 0) {
        font = &lv_font_montserrat_44;
    } else if (strcmp(name, "montserrat_46") == 0) {
        font = &lv_font_montserrat_46;
    } else if (strcmp(name, "montserrat_48") == 0) {
        font = &lv_font_montserrat_48;
    } else {
        /* 默认使用14号字体 */
        font = &lv_font_montserrat_14;
    }

    lua_pushlightuserdata(L, (void*)font);
    return 1;
}

/* 注册 font 子模块 */
void lvgl_register_font(lua_State* L) {
    REG_METHOD(L, "get", lvgl_font_get);
}
