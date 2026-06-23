/*
@module  lvgl.canvas
@summary LVGL????
@version 2.0
@date    2026.06.18
@author  ?? & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua??(OO??)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- ????
local canvas = lvgl.canvas.create(scr)
canvas:set_size(200, 200)
canvas:set_pos(50, 50)

-- ?????????????
-- canvas:set_buffer(buf, 200, 200, LV_CF_TRUE_COLOR)

-- ??????
canvas:set_px_color(10, 10, 0xFF0000)

-- ????
canvas:fill_bg(0xFFFFFF, 255)

-- ????
canvas:draw_rect(10, 10, 50, 50, style)

-- ????
canvas:draw_circle(100, 100, 30, style)

-- ????
canvas:draw_line({{0,0}, {100,100}}, style)

-- ????
canvas:draw_arc(100, 100, 50, 0, 90, style)

-- ????
canvas:draw_text(10, 10, 100, style, "Hello")

-- ????
local c = lvgl.canvas.create(scr):set_size(150, 150):set_pos(80, 30)
*/

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* canvas???metatable?? */
static int canvas_metatable_ref = LUA_NOREF;

static lv_coord_t lvgl_style_get_coord(const lv_style_t* style, lv_style_prop_t prop, lv_coord_t def)
{
    lv_style_value_t v;
    if (style && lv_style_get_prop(style, prop, &v) == LV_STYLE_RES_FOUND) {
        return (lv_coord_t)v.num;
    }
    return def;
}

static lv_opa_t lvgl_style_get_opa(const lv_style_t* style, lv_style_prop_t prop, lv_opa_t def)
{
    lv_style_value_t v;
    if (style && lv_style_get_prop(style, prop, &v) == LV_STYLE_RES_FOUND) {
        return (lv_opa_t)v.num;
    }
    return def;
}

static void lvgl_style_to_rect_dsc(const lv_style_t* style, lv_draw_rect_dsc_t* dsc)
{
    lv_style_value_t v;

    lv_draw_rect_dsc_init(dsc);
    if (!style) {
        return;
    }
    if (lv_style_get_prop(style, LV_STYLE_BG_COLOR, &v) == LV_STYLE_RES_FOUND) {
        dsc->bg_color = v.color;
    }
    if (lv_style_get_prop(style, LV_STYLE_BG_OPA, &v) == LV_STYLE_RES_FOUND) {
        dsc->bg_opa = (lv_opa_t)v.num;
    }
    if (lv_style_get_prop(style, LV_STYLE_BORDER_COLOR, &v) == LV_STYLE_RES_FOUND) {
        dsc->border_color = v.color;
    }
    dsc->border_width = lvgl_style_get_coord(style, LV_STYLE_BORDER_WIDTH, dsc->border_width);
    dsc->border_opa = lvgl_style_get_opa(style, LV_STYLE_BORDER_OPA, dsc->border_opa);
    dsc->radius = lvgl_style_get_coord(style, LV_STYLE_RADIUS, dsc->radius);
}

static void lvgl_style_to_line_dsc(const lv_style_t* style, lv_draw_line_dsc_t* dsc)
{
    lv_style_value_t v;

    lv_draw_line_dsc_init(dsc);
    if (!style) {
        return;
    }
    if (lv_style_get_prop(style, LV_STYLE_LINE_COLOR, &v) == LV_STYLE_RES_FOUND) {
        dsc->color = v.color;
    }
    dsc->width = lvgl_style_get_coord(style, LV_STYLE_LINE_WIDTH, dsc->width);
    dsc->opa = lvgl_style_get_opa(style, LV_STYLE_LINE_OPA, dsc->opa);
    dsc->dash_width = lvgl_style_get_coord(style, LV_STYLE_LINE_DASH_WIDTH, dsc->dash_width);
    dsc->dash_gap = lvgl_style_get_coord(style, LV_STYLE_LINE_DASH_GAP, dsc->dash_gap);
}

static void lvgl_style_to_arc_dsc(const lv_style_t* style, lv_draw_arc_dsc_t* dsc)
{
    lv_style_value_t v;

    lv_draw_arc_dsc_init(dsc);
    if (!style) {
        return;
    }
    if (lv_style_get_prop(style, LV_STYLE_ARC_COLOR, &v) == LV_STYLE_RES_FOUND) {
        dsc->color = v.color;
    } else if (lv_style_get_prop(style, LV_STYLE_LINE_COLOR, &v) == LV_STYLE_RES_FOUND) {
        dsc->color = v.color;
    }
    dsc->width = lvgl_style_get_coord(style, LV_STYLE_ARC_WIDTH,
                                      lvgl_style_get_coord(style, LV_STYLE_LINE_WIDTH, dsc->width));
    dsc->opa = lvgl_style_get_opa(style, LV_STYLE_ARC_OPA,
                                  lvgl_style_get_opa(style, LV_STYLE_LINE_OPA, dsc->opa));
}

static void lvgl_style_to_label_dsc(const lv_style_t* style, lv_draw_label_dsc_t* dsc)
{
    lv_style_value_t v;

    lv_draw_label_dsc_init(dsc);
    if (!style) {
        return;
    }
    if (lv_style_get_prop(style, LV_STYLE_TEXT_COLOR, &v) == LV_STYLE_RES_FOUND) {
        dsc->color = v.color;
    }
    if (lv_style_get_prop(style, LV_STYLE_TEXT_FONT, &v) == LV_STYLE_RES_FOUND) {
        dsc->font = v.ptr;
    }
    dsc->opa = lvgl_style_get_opa(style, LV_STYLE_TEXT_OPA, dsc->opa);
    dsc->letter_space = lvgl_style_get_coord(style, LV_STYLE_TEXT_LETTER_SPACE, dsc->letter_space);
    dsc->line_space = lvgl_style_get_coord(style, LV_STYLE_TEXT_LINE_SPACE, dsc->line_space);
}

/* ==================== ?????? ==================== */

static int lvgl_canvas_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* canvas = lv_canvas_create(parent);
    lua_pushlightuserdata(L, canvas);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
??????(OO??)
@param self ???????
@return userdata ?metatable??????
@usage local canvas = lvgl.canvas.create(scr)
*/
static int lvgl_canvas_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_canvas_create_internal, canvas_metatable_ref);
}

/*
????????
@param self ????????
@param w ??
@param h ??
@param buf ??????
@param stride ??????????w)
@return self
@usage canvas:set_buffer(buf, 200, 200)
*/
static int lvgl_canvas_set_buffer(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t w = (int32_t)luaL_checkinteger(L, 2);
    int32_t h = (int32_t)luaL_checkinteger(L, 3);
    lv_color_t* buf = (lv_color_t*)luaL_checklightuserdata(L, 4);
    lv_img_cf_t cf = (lv_img_cf_t)luaL_optinteger(L, 5, LV_IMG_CF_TRUE_COLOR);
    lv_canvas_set_buffer(canvas, buf, w, h, cf);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param x X??
@param y Y??
@param color ????
@return self
@usage canvas:set_px_color(10, 10, 0xFF0000)
*/
static int lvgl_canvas_set_px_color(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 4);
    lv_canvas_set_px_color(canvas, x, y, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param x X??
@param y Y??
@param opa ?????0-255)
@return self
@usage canvas:set_px_opa(10, 10, 128)
*/
static int lvgl_canvas_set_px_opa(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_opa_t opa = (lv_opa_t)luaL_checkinteger(L, 4);
    lv_canvas_set_px_opa(canvas, x, y, opa);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param x X??
@param y Y??
@return integer ????
@usage local color = canvas:get_px_color(10, 10)
*/
static int lvgl_canvas_get_px_color(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_color_t color = lv_canvas_get_px(canvas, x, y);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
????????
@param self ????????
@param x X??
@param y Y??
@return integer ?????
@usage local opa = canvas:get_px_opa(10, 10)
*/
static int lvgl_canvas_get_px_opa(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_img_dsc_t* img = lv_canvas_get_img(canvas);
    lv_opa_t opa = LV_OPA_COVER;
    if (img && lv_img_cf_has_alpha(img->header.cf)) {
        opa = lv_img_buf_get_px_alpha(img, x, y);
    }
    lua_pushinteger(L, opa);
    return 1;
}

/*
????
@param self ????????
@param color ????
@param opa ??????????255)
@return self
@usage canvas:fill_bg(0xFFFFFF, 255)
*/
static int lvgl_canvas_fill_bg(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    lv_opa_t opa = (lv_opa_t)luaL_optinteger(L, 3, LV_OPA_COVER);
    lv_canvas_fill_bg(canvas, color, opa);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param x X??
@param y Y??
@param w ??
@param h ??
@param style ????
@return self
@usage canvas:draw_rect(10, 10, 50, 50, style)
*/
static int lvgl_canvas_draw_rect(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    int32_t w = (int32_t)luaL_checkinteger(L, 4);
    int32_t h = (int32_t)luaL_checkinteger(L, 5);
    const lv_style_t* style = (const lv_style_t*)luaL_checklightuserdata(L, 6);
    lv_draw_rect_dsc_t dsc;
    lvgl_style_to_rect_dsc(style, &dsc);
    lv_canvas_draw_rect(canvas, x, y, w, h, &dsc);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param x ??X??
@param y ??Y??
@param r ??
@param style ????
@return self
@usage canvas:draw_circle(100, 100, 30, style)
*/
static int lvgl_canvas_draw_circle(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    int32_t r = (int32_t)luaL_checkinteger(L, 4);
    const lv_style_t* style = (const lv_style_t*)luaL_checklightuserdata(L, 5);
    lv_draw_rect_dsc_t dsc;
    lvgl_style_to_rect_dsc(style, &dsc);
    dsc.radius = LV_RADIUS_CIRCLE;
    lv_canvas_draw_rect(canvas, x - r, y - r, 2 * r, 2 * r, &dsc);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param points ????{{x1,y1},{x2,y2},...})
@param style ????
@return self
@usage canvas:draw_line({{0,0}, {100,100}}, style)
*/
static int lvgl_canvas_draw_line(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    /* points????: {{x1,y1}, {x2,y2}, ...} */
    luaL_checktype(L, 2, LUA_TTABLE);
    uint32_t point_num = (uint32_t)luaL_len(L, 2);

    lv_point_t* points = (lv_point_t*)cm_malloc(sizeof(lv_point_t) * point_num);
    if (!points) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    for (uint32_t i = 0; i < point_num; i++) {
        lua_geti(L, 2, i + 1);
        if (lua_istable(L, -1)) {
            lua_geti(L, -1, 1);
            points[i].x = (int32_t)luaL_checkinteger(L, -1);
            lua_geti(L, -2, 2);
            points[i].y = (int32_t)luaL_checkinteger(L, -1);
            lua_pop(L, 2);
        }
        lua_pop(L, 1);
    }

    const lv_style_t* style = (const lv_style_t*)luaL_checklightuserdata(L, 3);
    lv_draw_line_dsc_t dsc;
    lvgl_style_to_line_dsc(style, &dsc);
    lv_canvas_draw_line(canvas, points, point_num, &dsc);
    cm_free(points);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param points ????
@param style ????
@return self
@usage canvas:draw_polygon({{0,0}, {50,0}, {25,50}}, style)
*/
static int lvgl_canvas_draw_polygon(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    /* points???? */
    luaL_checktype(L, 2, LUA_TTABLE);
    uint32_t point_num = (uint32_t)luaL_len(L, 2);

    lv_point_t* points = (lv_point_t*)cm_malloc(sizeof(lv_point_t) * point_num);
    if (!points) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    for (uint32_t i = 0; i < point_num; i++) {
        lua_geti(L, 2, i + 1);
        if (lua_istable(L, -1)) {
            lua_geti(L, -1, 1);
            points[i].x = (int32_t)luaL_checkinteger(L, -1);
            lua_geti(L, -2, 2);
            points[i].y = (int32_t)luaL_checkinteger(L, -1);
            lua_pop(L, 2);
        }
        lua_pop(L, 1);
    }

    const lv_style_t* style = (const lv_style_t*)luaL_checklightuserdata(L, 3);
    lv_draw_rect_dsc_t dsc;
    lvgl_style_to_rect_dsc(style, &dsc);
    lv_canvas_draw_polygon(canvas, points, point_num, &dsc);
    cm_free(points);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param center_x ??X??
@param center_y ??Y??
@param r ??
@param start_angle ????
@param end_angle ????
@param style ????
@return self
@usage canvas:draw_arc(100, 100, 50, 0, 90, style)
*/
static int lvgl_canvas_draw_arc(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t center_x = (int32_t)luaL_checkinteger(L, 2);
    int32_t center_y = (int32_t)luaL_checkinteger(L, 3);
    int32_t r = (int32_t)luaL_checkinteger(L, 4);
    int32_t start_angle = (int32_t)luaL_checkinteger(L, 5);
    int32_t end_angle = (int32_t)luaL_checkinteger(L, 6);
    const lv_style_t* style = (const lv_style_t*)luaL_checklightuserdata(L, 7);
    lv_draw_arc_dsc_t dsc;
    lvgl_style_to_arc_dsc(style, &dsc);
    lv_canvas_draw_arc(canvas, center_x, center_y, r, start_angle, end_angle, &dsc);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param x X??
@param y Y??
@param max_width ?????
@param style ????
@param txt ????
@return self
@usage canvas:draw_text(10, 10, 100, style, "Hello")
*/
static int lvgl_canvas_draw_text(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    int32_t max_width = (int32_t)luaL_checkinteger(L, 4);
    const lv_style_t* style = (const lv_style_t*)luaL_checklightuserdata(L, 5);
    const char* txt = luaL_checkstring(L, 6);
    lv_draw_label_dsc_t dsc;
    lvgl_style_to_label_dsc(style, &dsc);
    lv_canvas_draw_text(canvas, x, y, max_width, &dsc, txt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param x X??
@param y Y??
@param src ??????
@param dsc ????????
@return self
@usage canvas:draw_img(10, 10, img_src, img_dsc)
*/
static int lvgl_canvas_draw_img(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    const void* src = (const void*)luaL_checklightuserdata(L, 4);
    const lv_draw_img_dsc_t* dsc = (const lv_draw_img_dsc_t*)luaL_checklightuserdata(L, 5);
    lv_canvas_draw_img(canvas, x, y, src, dsc);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param x X??
@param y Y??
@param w ??
@param h ??
@param r ????
@return self
@usage canvas:blur_hor(10, 10, 50, 50, 5)
*/
static int lvgl_canvas_blur_hor(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    int32_t w = (int32_t)luaL_checkinteger(L, 4);
    int32_t h = (int32_t)luaL_checkinteger(L, 5);
    uint16_t r = (uint16_t)luaL_checkinteger(L, 6);
    lv_area_t area = {x, y, x + w - 1, y + h - 1};
    lv_canvas_blur_hor(canvas, &area, r);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param x X??
@param y Y??
@param w ??
@param h ??
@param r ????
@return self
@usage canvas:blur_ver(10, 10, 50, 50, 5)
*/
static int lvgl_canvas_blur_ver(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    int32_t w = (int32_t)luaL_checkinteger(L, 4);
    int32_t h = (int32_t)luaL_checkinteger(L, 5);
    uint16_t r = (uint16_t)luaL_checkinteger(L, 6);
    lv_area_t area = {x, y, x + w - 1, y + h - 1};
    lv_canvas_blur_ver(canvas, &area, r);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param src_buf ??????
@param src_w ????
@param src_h ????
@param src_stride ????
@param x ??X??
@param y ??Y??
@return self
@usage canvas:copy_buf(src_buf, 100, 100, 100, 10, 10)
*/
static int lvgl_canvas_copy_buf(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    const void* src_buf = (const void*)luaL_checklightuserdata(L, 2);
    int32_t src_w = (int32_t)luaL_checkinteger(L, 3);
    int32_t src_h = (int32_t)luaL_checkinteger(L, 4);
    int32_t x = (int32_t)luaL_checkinteger(L, 6);
    int32_t y = (int32_t)luaL_checkinteger(L, 7);
    (void)luaL_optinteger(L, 5, src_w);
    lv_canvas_copy_buf(canvas, src_buf, x, y, src_w, src_h);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param src ????????
@param angle ????
@param zoom ????
@param offset_x X??(???
@param offset_y Y??(???
@param pivot_x ????X(???
@param pivot_y ????Y(???
@param antialias ???????
@return self
@usage canvas:transform(src, 90, 128)
*/
static int lvgl_canvas_transform(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    const lv_img_dsc_t* src = (const lv_img_dsc_t*)luaL_checklightuserdata(L, 2);
    int32_t angle = (int32_t)luaL_checkinteger(L, 3);
    int32_t zoom = (int32_t)luaL_checkinteger(L, 4);
    int32_t offset_x = (int32_t)luaL_optinteger(L, 5, 0);
    int32_t offset_y = (int32_t)luaL_optinteger(L, 6, 0);
    int32_t pivot_x = (int32_t)luaL_optinteger(L, 7, 0);
    int32_t pivot_y = (int32_t)luaL_optinteger(L, 8, 0);
    bool antialias = lua_toboolean(L, 9);
    lv_img_dsc_t img;
    lv_memcpy(&img, src, sizeof(lv_img_dsc_t));
    lv_canvas_transform(canvas, &img, angle, zoom, offset_x, offset_y, pivot_x, pivot_y, antialias);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param src ????????
@param angle ????
@param x X??(???
@param y Y??(???
@param pivot_x ????X(???
@param pivot_y ????Y(???
@return self
@usage canvas:rotate(src, 90)
*/
static int lvgl_canvas_rotate(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    const lv_img_dsc_t* src = (const lv_img_dsc_t*)luaL_checklightuserdata(L, 2);
    int32_t angle = (int32_t)luaL_checkinteger(L, 3);
    int32_t x = (int32_t)luaL_optinteger(L, 4, 0);
    int32_t y = (int32_t)luaL_optinteger(L, 5, 0);
    int32_t pivot_x = (int32_t)luaL_optinteger(L, 6, 0);
    int32_t pivot_y = (int32_t)luaL_optinteger(L, 7, 0);
    lv_img_dsc_t img;
    lv_memcpy(&img, src, sizeof(lv_img_dsc_t));
    lv_canvas_transform(canvas, &img, angle, LV_IMG_ZOOM_NONE, x, y, pivot_x, pivot_y, true);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@return userdata ????
@usage local layer = canvas:init_layer()
*/
static int lvgl_canvas_init_layer(lua_State* L) {
    void* layer = cm_malloc(1);
    if (!layer) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    (void)lvgl_get_obj_ptr(L, 1);
    lua_pushlightuserdata(L, layer);
    return 1;
}

/*
????
@param self ????????
@param layer ????
@return self
@usage canvas:finish_layer(layer)
*/
static int lvgl_canvas_finish_layer(lua_State* L) {
    (void)lvgl_get_obj_ptr(L, 1);
    (void)luaL_checklightuserdata(L, 2);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????
@param self ????????
@param layer ????
@return self
@usage canvas:apply_layer(layer)
*/
static int lvgl_canvas_apply_layer(lua_State* L) {
    (void)lvgl_get_obj_ptr(L, 1);
    void* layer = luaL_checklightuserdata(L, 2);
    cm_free(layer);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param color ????
@param opa ????????
@return self
@usage canvas:set_layer_bg(0xFFFFFF, 255)
*/
static int lvgl_canvas_set_layer_bg(lua_State* L) {
    (void)lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    (void)luaL_optinteger(L, 3, LV_OPA_COVER);
    lua_pushvalue(L, 1);
    return 1;
}

/* ?? canvas ????*/
void lvgl_register_canvas(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "set_buffer", lvgl_canvas_set_buffer);
    REG_METHOD(L, "set_px_color", lvgl_canvas_set_px_color);
    REG_METHOD(L, "set_px_opa", lvgl_canvas_set_px_opa);
    REG_METHOD(L, "get_px_color", lvgl_canvas_get_px_color);
    REG_METHOD(L, "get_px_opa", lvgl_canvas_get_px_opa);
    REG_METHOD(L, "fill_bg", lvgl_canvas_fill_bg);
    REG_METHOD(L, "draw_rect", lvgl_canvas_draw_rect);
    REG_METHOD(L, "draw_circle", lvgl_canvas_draw_circle);
    REG_METHOD(L, "draw_line", lvgl_canvas_draw_line);
    REG_METHOD(L, "draw_polygon", lvgl_canvas_draw_polygon);
    REG_METHOD(L, "draw_arc", lvgl_canvas_draw_arc);
    REG_METHOD(L, "draw_text", lvgl_canvas_draw_text);
    REG_METHOD(L, "draw_img", lvgl_canvas_draw_img);
    REG_METHOD(L, "blur_hor", lvgl_canvas_blur_hor);
    REG_METHOD(L, "blur_ver", lvgl_canvas_blur_ver);
    REG_METHOD(L, "copy_buf", lvgl_canvas_copy_buf);
    REG_METHOD(L, "transform", lvgl_canvas_transform);
    REG_METHOD(L, "rotate", lvgl_canvas_rotate);
    REG_METHOD(L, "init_layer", lvgl_canvas_init_layer);
    REG_METHOD(L, "finish_layer", lvgl_canvas_finish_layer);
    REG_METHOD(L, "apply_layer", lvgl_canvas_apply_layer);
    REG_METHOD(L, "set_layer_bg", lvgl_canvas_set_layer_bg);

    /* ????metatable??(????) */
    canvas_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.canvas.set_buffer(canvas, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, canvas_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.canvas) */
    REG_METHOD(L, "create", lvgl_canvas_create);
}