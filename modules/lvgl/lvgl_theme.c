/*
@module  lvgl.theme
@summary LVGL????
@version 1.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua??
local lvgl = require("lvgl")

-- ????????? primary, secondary, dark?
local theme = lvgl.theme.create(0x3366FF, 0x6633FF, false)

-- ??????
theme:set_color(lvgl.THEME_COLOR_PRIMARY, 0x3366FF)
theme:set_color(lvgl.THEME_COLOR_SECONDARY, 0x6633FF)

-- ??????????
lvgl.theme.set(theme)

-- ????????
local theme_default = lvgl.theme.default()
lvgl.theme.set(theme_default)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

static lv_color_t lvgl_theme_parse_color(lua_State* L, int idx, lv_color_t fallback)
{
    if (lua_gettop(L) >= idx && !lua_isnil(L, idx)) {
        lv_color_t color;
        color.full = (uint32_t)luaL_checkinteger(L, idx);
        return color;
    }
    return fallback;
}

static lv_disp_t* lvgl_theme_get_disp(lua_State* L, int idx)
{
    if (lua_gettop(L) >= idx && lua_islightuserdata(L, idx)) {
        return (lv_disp_t*)lua_touserdata(L, idx);
    }
    return lv_disp_get_default();
}

/*
??????
@param primary ?????
@param secondary ?????
@param dark ?????????
@return userdata ????
@usage local theme = lvgl.theme.create(0x3366FF, 0x6633FF, false)
*/
static int lvgl_theme_create(lua_State* L)
{
    lv_disp_t* disp = lv_disp_get_default();
    lv_color_t primary = lv_palette_main(LV_PALETTE_BLUE);
    lv_color_t secondary = lv_palette_main(LV_PALETTE_RED);
    bool dark = false;

    primary = lvgl_theme_parse_color(L, 1, primary);
    secondary = lvgl_theme_parse_color(L, 2, secondary);
    if (lua_gettop(L) >= 3) {
        dark = lua_toboolean(L, 3);
    }

    lv_theme_t* theme = lv_theme_default_init(disp, primary, secondary, dark, LV_FONT_DEFAULT);
    lua_pushlightuserdata(L, theme);
    return 1;
}

/*
???????
@param theme ????
@param disp ????????
@usage lvgl.theme.set(theme)
*/
static int lvgl_theme_set(lua_State* L)
{
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    lv_disp_t* disp = lvgl_theme_get_disp(L, 2);
    lv_disp_set_theme(disp, theme);
    return 0;
}

/*
???????
@param disp ????????
@return userdata ??????
@usage local theme = lvgl.theme.get()
*/
static int lvgl_theme_get(lua_State* L)
{
    lv_disp_t* disp = lvgl_theme_get_disp(L, 1);
    lua_pushlightuserdata(L, lv_disp_get_theme(disp));
    return 1;
}

/*
??????
@return userdata ??????
@usage local theme = lvgl.theme.default()
*/
static int lvgl_theme_default(lua_State* L)
{
    lv_theme_t* theme;

    (void)L;
    if (lv_theme_default_is_inited()) {
        theme = lv_theme_default_get();
    } else {
        theme = lv_theme_default_init(lv_disp_get_default(),
                                      lv_palette_main(LV_PALETTE_BLUE),
                                      lv_palette_main(LV_PALETTE_RED),
                                      false, LV_FONT_DEFAULT);
    }

    lua_pushlightuserdata(L, theme);
    return 1;
}

/*
????????
@param obj ????
@usage lvgl.theme.apply(obj)
*/
static int lvgl_theme_apply(lua_State* L)
{
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_theme_apply(obj);
    return 0;
}

static bool lvgl_theme_set_color_field(lv_theme_t* theme, int color_id, lv_color_t color)
{
    switch (color_id) {
    case LV_THEME_COLOR_PRIMARY:
        theme->color_primary = color;
        return true;
    case LV_THEME_COLOR_SECONDARY:
        theme->color_secondary = color;
        return true;
    default:
        return false;
    }
}

static bool lvgl_theme_get_color_field(const lv_theme_t* theme, int color_id, lv_color_t* color)
{
    switch (color_id) {
    case LV_THEME_COLOR_PRIMARY:
        *color = theme->color_primary;
        return true;
    case LV_THEME_COLOR_SECONDARY:
        *color = theme->color_secondary;
        return true;
    default:
        return false;
    }
}

/*
??????
@param theme ????
@param color_id ??ID
@param color ???
@usage theme:set_color(lvgl.THEME_COLOR_PRIMARY, 0x3366FF)
*/
static int lvgl_theme_set_color(lua_State* L)
{
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    int color_id = (int)luaL_checkinteger(L, 2);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 3);

    if (!lvgl_theme_set_color_field(theme, color_id, color)) {
        return luaL_error(L, "unsupported theme color id: %d", color_id);
    }

    lv_obj_report_style_change(NULL);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param theme ????
@param color_id ??ID
@return integer ???
@usage local color = theme:get_color(lvgl.THEME_COLOR_PRIMARY)
*/
static int lvgl_theme_get_color(lua_State* L)
{
    const lv_theme_t* theme = (const lv_theme_t*)luaL_checklightuserdata(L, 1);
    int color_id = (int)luaL_checkinteger(L, 2);
    lv_color_t color;

    if (!lvgl_theme_get_color_field(theme, color_id, &color)) {
        return luaL_error(L, "unsupported theme color id: %d", color_id);
    }

    lua_pushinteger(L, color.full);
    return 1;
}

static bool lvgl_theme_set_font_field(lv_theme_t* theme, int font_id, const lv_font_t* font)
{
    switch (font_id) {
    case LV_THEME_FONT_SMALL:
        theme->font_small = font;
        return true;
    case LV_THEME_FONT_MEDIUM:
        theme->font_normal = font;
        return true;
    case LV_THEME_FONT_LARGE:
    case LV_THEME_FONT_TITLE:
        theme->font_large = font;
        return true;
    default:
        return false;
    }
}

static const lv_font_t* lvgl_theme_get_font_field(const lv_theme_t* theme, int font_id)
{
    switch (font_id) {
    case LV_THEME_FONT_SMALL:
        return theme->font_small;
    case LV_THEME_FONT_MEDIUM:
        return theme->font_normal;
    case LV_THEME_FONT_LARGE:
    case LV_THEME_FONT_TITLE:
        return theme->font_large;
    default:
        return NULL;
    }
}

/*
??????
@param theme ????
@param font_id ??ID
@param font ????
@usage theme:set_font(lvgl.THEME_FONT_SMALL, font)
*/
static int lvgl_theme_set_font(lua_State* L)
{
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    int font_id = (int)luaL_checkinteger(L, 2);
    const lv_font_t* font = (const lv_font_t*)luaL_checklightuserdata(L, 3);

    if (!lvgl_theme_set_font_field(theme, font_id, font)) {
        return luaL_error(L, "unsupported theme font id: %d", font_id);
    }

    lv_obj_report_style_change(NULL);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param theme ????
@param font_id ??ID
@return userdata ????
@usage local font = theme:get_font(lvgl.THEME_FONT_SMALL)
*/
static int lvgl_theme_get_font(lua_State* L)
{
    const lv_theme_t* theme = (const lv_theme_t*)luaL_checklightuserdata(L, 1);
    int font_id = (int)luaL_checkinteger(L, 2);
    const lv_font_t* font = lvgl_theme_get_font_field(theme, font_id);

    if (!font) {
        return luaL_error(L, "unsupported theme font id: %d", font_id);
    }

    lua_pushlightuserdata(L, (void*)font);
    return 1;
}

/*
???????LVGL 8 ??? API????????
@param theme ????
@param size_id ??ID
@param size ???
@usage theme:set_size(lvgl.THEME_SIZE_BUTTON_HEIGHT, 40)
*/
static int lvgl_theme_set_size(lua_State* L)
{
    (void)luaL_checklightuserdata(L, 1);
    (void)luaL_checkinteger(L, 2);
    (void)luaL_checkinteger(L, 3);
    lua_pushvalue(L, 1);
    return 1;
}

/*
???????LVGL 8 ??? API????????
@param theme ????
@param size_id ??ID
@return integer ???
@usage local size = theme:get_size(lvgl.THEME_SIZE_BUTTON_HEIGHT)
*/
static int lvgl_theme_get_size(lua_State* L)
{
    (void)luaL_checklightuserdata(L, 1);
    (void)luaL_checkinteger(L, 2);
    lua_pushinteger(L, 0);
    return 1;
}

/* ?? theme ??? */
int lvgl_register_theme(lua_State* L)
{
    REG_METHOD(L, "create", lvgl_theme_create);
    REG_METHOD(L, "set", lvgl_theme_set);
    REG_METHOD(L, "get", lvgl_theme_get);
    REG_METHOD(L, "default", lvgl_theme_default);
    REG_METHOD(L, "apply", lvgl_theme_apply);
    REG_METHOD(L, "set_color", lvgl_theme_set_color);
    REG_METHOD(L, "get_color", lvgl_theme_get_color);
    REG_METHOD(L, "set_font", lvgl_theme_set_font);
    REG_METHOD(L, "get_font", lvgl_theme_get_font);
    REG_METHOD(L, "set_size", lvgl_theme_set_size);
    REG_METHOD(L, "get_size", lvgl_theme_get_size);
    return 0;
}
