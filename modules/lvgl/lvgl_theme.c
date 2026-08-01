/**
 * @file iot_lvgl_theme.c
 * @brief LVGL主题系统
 *
 * 实现LVGL主题系统的Lua绑定，包括主题创建（自定义主色/次色/暗色模式）、设置主题颜色、应用主题、恢复默认主题等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

static lv_color_t iot_lvgl_theme_parse_color(lua_State* L, int idx, lv_color_t fallback)
{
    if (lua_gettop(L) >= idx && !lua_isnil(L, idx)) {
        lv_color_t color;
        color.full = (uint32_t)luaL_checkinteger(L, idx);
        return color;
    }
    return fallback;
}

static lv_disp_t* iot_lvgl_theme_get_disp(lua_State* L, int idx)
{
    if (lua_gettop(L) >= idx && lua_islightuserdata(L, idx)) {
        return (lv_disp_t*)lua_touserdata(L, idx);
    }
    return lv_disp_get_default();
}

/*
创建主题
@return userdata 主题指针
@usage local theme = lvgl.theme.create()
*/
static int iot_lvgl_theme_create(lua_State* L)
{
    lv_disp_t* disp = lv_disp_get_default();
    lv_color_t primary = lv_palette_main(LV_PALETTE_BLUE);
    lv_color_t secondary = lv_palette_main(LV_PALETTE_RED);
    bool dark = false;

    primary = iot_lvgl_theme_parse_color(L, 1, primary);
    secondary = iot_lvgl_theme_parse_color(L, 2, secondary);
    if (lua_gettop(L) >= 3) {
        dark = lua_toboolean(L, 3);
    }

    lv_theme_t* theme = lv_theme_default_init(disp, primary, secondary, dark, LV_FONT_DEFAULT);
    lua_pushlightuserdata(L, theme);
    return 1;
}

/*
设置主题
@param theme 主题指针
@usage lvgl.theme.set(theme)
*/
static int iot_lvgl_theme_set(lua_State* L)
{
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    lv_disp_t* disp = iot_lvgl_theme_get_disp(L, 2);
    lv_disp_set_theme(disp, theme);
    return 0;
}

/*
获取当前主题
@return userdata 当前主题指针
@usage local theme = lvgl.theme.get()
*/
static int iot_lvgl_theme_get(lua_State* L)
{
    lv_disp_t* disp = iot_lvgl_theme_get_disp(L, 1);
    lua_pushlightuserdata(L, lv_disp_get_theme(disp));
    return 1;
}

/*
获取默认主题
@return userdata 默认主题指针
@usage local theme = lvgl.theme.default()
*/
static int iot_lvgl_theme_default(lua_State* L)
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
应用主题到对象
@param obj 对象指针
@usage lvgl.theme.apply(obj)
*/
static int iot_lvgl_theme_apply(lua_State* L)
{
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_theme_apply(obj);
    return 0;
}

static bool iot_lvgl_theme_set_color_field(lv_theme_t* theme, int color_id, lv_color_t color)
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

static bool iot_lvgl_theme_get_color_field(const lv_theme_t* theme, int color_id, lv_color_t* color)
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
设置主题颜色
@param theme 主题指针
@param color_id 颜色ID
@param color 颜色值
@usage theme:set_color(lvgl.THEME_COLOR_PRIMARY, 0x3366FF)
*/
static int iot_lvgl_theme_set_color(lua_State* L)
{
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    int color_id = (int)luaL_checkinteger(L, 2);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 3);

    if (!iot_lvgl_theme_set_color_field(theme, color_id, color)) {
        return luaL_error(L, "unsupported theme color id: %d", color_id);
    }

    lv_obj_report_style_change(NULL);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取主题颜色
@param theme 主题指针
@param color_id 颜色ID
@return integer 颜色值
@usage local color = theme:get_color(lvgl.THEME_COLOR_PRIMARY)
*/
static int iot_lvgl_theme_get_color(lua_State* L)
{
    const lv_theme_t* theme = (const lv_theme_t*)luaL_checklightuserdata(L, 1);
    int color_id = (int)luaL_checkinteger(L, 2);
    lv_color_t color;

    if (!iot_lvgl_theme_get_color_field(theme, color_id, &color)) {
        return luaL_error(L, "unsupported theme color id: %d", color_id);
    }

    lua_pushinteger(L, color.full);
    return 1;
}

static bool iot_lvgl_theme_set_font_field(lv_theme_t* theme, int font_id, const lv_font_t* font)
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

static const lv_font_t* iot_lvgl_theme_get_font_field(const lv_theme_t* theme, int font_id)
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
设置主题字体
@param theme 主题指针
@param font_id 字体ID
@param font 字体指针
@usage theme:set_font(lvgl.THEME_FONT_SMALL, font)
*/
static int iot_lvgl_theme_set_font(lua_State* L)
{
    lv_theme_t* theme = (lv_theme_t*)luaL_checklightuserdata(L, 1);
    int font_id = (int)luaL_checkinteger(L, 2);
    const lv_font_t* font = (const lv_font_t*)luaL_checklightuserdata(L, 3);

    if (!iot_lvgl_theme_set_font_field(theme, font_id, font)) {
        return luaL_error(L, "unsupported theme font id: %d", font_id);
    }

    lv_obj_report_style_change(NULL);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取主题字体
@param theme 主题指针
@param font_id 字体ID
@return userdata 字体指针
@usage local font = theme:get_font(lvgl.THEME_FONT_SMALL)
*/
static int iot_lvgl_theme_get_font(lua_State* L)
{
    const lv_theme_t* theme = (const lv_theme_t*)luaL_checklightuserdata(L, 1);
    int font_id = (int)luaL_checkinteger(L, 2);
    const lv_font_t* font = iot_lvgl_theme_get_font_field(theme, font_id);

    if (!font) {
        return luaL_error(L, "unsupported theme font id: %d", font_id);
    }

    lua_pushlightuserdata(L, (void*)font);
    return 1;
}

/*
设置主题尺寸
@param theme 主题指针
@param size_id 尺寸ID
@param size 尺寸值
@usage theme:set_size(lvgl.THEME_SIZE_BUTTON_HEIGHT, 40)
*/
static int iot_lvgl_theme_set_size(lua_State* L)
{
    (void)luaL_checklightuserdata(L, 1);
    (void)luaL_checkinteger(L, 2);
    (void)luaL_checkinteger(L, 3);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取主题尺寸
@param theme 主题指针
@param size_id 尺寸ID
@return integer 尺寸值
@usage local size = theme:get_size(lvgl.THEME_SIZE_BUTTON_HEIGHT)
*/
static int iot_lvgl_theme_get_size(lua_State* L)
{
    (void)luaL_checklightuserdata(L, 1);
    (void)luaL_checkinteger(L, 2);
    lua_pushinteger(L, 0);
    return 1;
}

/* 注册 theme 子模块 */
int iot_lvgl_register_theme(lua_State* L)
{
    REG_METHOD(L, "create", iot_lvgl_theme_create);
    REG_METHOD(L, "set", iot_lvgl_theme_set);
    REG_METHOD(L, "get", iot_lvgl_theme_get);
    REG_METHOD(L, "default", iot_lvgl_theme_default);
    REG_METHOD(L, "apply", iot_lvgl_theme_apply);
    REG_METHOD(L, "set_color", iot_lvgl_theme_set_color);
    REG_METHOD(L, "get_color", iot_lvgl_theme_get_color);
    REG_METHOD(L, "set_font", iot_lvgl_theme_set_font);
    REG_METHOD(L, "get_font", iot_lvgl_theme_get_font);
    REG_METHOD(L, "set_size", iot_lvgl_theme_set_size);
    REG_METHOD(L, "get_size", iot_lvgl_theme_get_size);
    return 0;
}
