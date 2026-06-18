/*
@module  lvgl.canvas
@summary LVGL画布控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建画布
local canvas = lvgl.canvas.create(scr)
canvas:set_size(200, 200)
canvas:set_pos(50, 50)

-- 设置缓冲区(需要预先分配)
-- canvas:set_buffer(buf, 200, 200, LV_CF_TRUE_COLOR)

-- 设置像素颜色
canvas:set_px_color(10, 10, 0xFF0000)

-- 填充背景
canvas:fill_bg(0xFFFFFF, 255)

-- 绘制矩形
canvas:draw_rect(10, 10, 50, 50, style)

-- 绘制圆形
canvas:draw_circle(100, 100, 30, style)

-- 绘制线条
canvas:draw_line({{0,0}, {100,100}}, style)

-- 绘制弧线
canvas:draw_arc(100, 100, 50, 0, 90, style)

-- 绘制文本
canvas:draw_text(10, 10, 100, style, "Hello")

-- 链式调用
local c = lvgl.canvas.create(scr):set_size(150, 150):set_pos(80, 30)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* canvas组件的metatable引用 */
static int canvas_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_canvas_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* canvas = lv_canvas_create(parent);
    lua_pushlightuserdata(L, canvas);
    return 1;
}

/* ==================== 画布OO方法 ==================== */

/*
创建画布控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的画布实例
@usage local canvas = lvgl.canvas.create(scr)
*/
static int lvgl_canvas_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_canvas_create_internal, canvas_metatable_ref);
}

/*
设置画布缓冲区
@param self 画布实例或指针
@param w 宽度
@param h 高度
@param buf 缓冲区指针
@param stride 行跨度(可选,默认为w)
@return self
@usage canvas:set_buffer(buf, 200, 200)
*/
static int lvgl_canvas_set_buffer(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t w = (int32_t)luaL_checkinteger(L, 2);
    int32_t h = (int32_t)luaL_checkinteger(L, 3);
    lv_color_t* buf = (lv_color_t*)luaL_checklightuserdata(L, 4);
    int32_t stride = (int32_t)luaL_optinteger(L, 5, w);
    lv_canvas_set_buffer(canvas, buf, w, h, stride);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置像素颜色
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param color 颜色值
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
设置像素透明度
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param opa 透明度值(0-255)
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
获取像素颜色
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@return integer 颜色值
@usage local color = canvas:get_px_color(10, 10)
*/
static int lvgl_canvas_get_px_color(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_color_t color = lv_canvas_get_px_color(canvas, x, y);
    lua_pushinteger(L, color.full);
    return 1;
}

/*
获取像素透明度
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@return integer 透明度值
@usage local opa = canvas:get_px_opa(10, 10)
*/
static int lvgl_canvas_get_px_opa(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_opa_t opa = lv_canvas_get_px_opa(canvas, x, y);
    lua_pushinteger(L, opa);
    return 1;
}

/*
填充背景
@param self 画布实例或指针
@param color 颜色值
@param opa 透明度值(可选,默认255)
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
绘制矩形
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param w 宽度
@param h 高度
@param style 样式指针
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
    lv_canvas_draw_rect(canvas, x, y, w, h, style);
    lua_pushvalue(L, 1);
    return 1;
}

/*
绘制圆形
@param self 画布实例或指针
@param x 圆心X坐标
@param y 圆心Y坐标
@param r 半径
@param style 样式指针
@return self
@usage canvas:draw_circle(100, 100, 30, style)
*/
static int lvgl_canvas_draw_circle(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    int32_t r = (int32_t)luaL_checkinteger(L, 4);
    const lv_style_t* style = (const lv_style_t*)luaL_checklightuserdata(L, 5);
    lv_canvas_draw_circle(canvas, x, y, r, style);
    lua_pushvalue(L, 1);
    return 1;
}

/*
绘制线条
@param self 画布实例或指针
@param points 点数组({{x1,y1},{x2,y2},...})
@param style 样式指针
@return self
@usage canvas:draw_line({{0,0}, {100,100}}, style)
*/
static int lvgl_canvas_draw_line(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    /* points参数为表: {{x1,y1}, {x2,y2}, ...} */
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
    lv_canvas_draw_line(canvas, points, point_num, style);
    cm_free(points);
    lua_pushvalue(L, 1);
    return 1;
}

/*
绘制多边形
@param self 画布实例或指针
@param points 点数组
@param style 样式指针
@return self
@usage canvas:draw_polygon({{0,0}, {50,0}, {25,50}}, style)
*/
static int lvgl_canvas_draw_polygon(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    /* points参数为表 */
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
    lv_canvas_draw_polygon(canvas, points, point_num, style);
    cm_free(points);
    lua_pushvalue(L, 1);
    return 1;
}

/*
绘制弧线
@param self 画布实例或指针
@param center_x 圆心X坐标
@param center_y 圆心Y坐标
@param r 半径
@param start_angle 起始角度
@param end_angle 结束角度
@param style 样式指针
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
    lv_canvas_draw_arc(canvas, center_x, center_y, r, start_angle, end_angle, style);
    lua_pushvalue(L, 1);
    return 1;
}

/*
绘制文本
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param max_width 最大宽度
@param style 样式指针
@param txt 文本内容
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
    lv_draw_label_dsc_init(&dsc);
    dsc.style = style;
    dsc.text = txt;
    lv_canvas_draw_text(canvas, x, y, max_width, &dsc);
    lua_pushvalue(L, 1);
    return 1;
}

/*
绘制图片
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param src 图片源指针
@param dsc 图片描述符指针
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
水平模糊
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param w 宽度
@param h 高度
@param r 模糊半径
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
    lv_canvas_blur_hor(canvas, x, y, w, h, r);
    lua_pushvalue(L, 1);
    return 1;
}

/*
垂直模糊
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param w 宽度
@param h 高度
@param r 模糊半径
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
    lv_canvas_blur_ver(canvas, x, y, w, h, r);
    lua_pushvalue(L, 1);
    return 1;
}

/*
复制缓冲区
@param self 画布实例或指针
@param src_buf 源缓冲区指针
@param src_w 源宽度
@param src_h 源高度
@param src_stride 源行跨度
@param x 目标X坐标
@param y 目标Y坐标
@return self
@usage canvas:copy_buf(src_buf, 100, 100, 100, 10, 10)
*/
static int lvgl_canvas_copy_buf(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    const void* src_buf = (const void*)luaL_checklightuserdata(L, 2);
    int32_t src_w = (int32_t)luaL_checkinteger(L, 3);
    int32_t src_h = (int32_t)luaL_checkinteger(L, 4);
    int32_t src_stride = (int32_t)luaL_checkinteger(L, 5);
    int32_t x = (int32_t)luaL_checkinteger(L, 6);
    int32_t y = (int32_t)luaL_checkinteger(L, 7);
    lv_canvas_copy_buf(canvas, src_buf, src_w, src_h, src_stride, x, y);
    lua_pushvalue(L, 1);
    return 1;
}

/*
变换图片
@param self 画布实例或指针
@param src 源图片描述符指针
@param angle 旋转角度
@param zoom 缩放值
@param offset_x X偏移(可选)
@param offset_y Y偏移(可选)
@param pivot_x 旋转中心X(可选)
@param pivot_y 旋转中心Y(可选)
@param antialias 抗锯齿(可选)
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
    lv_canvas_transform(canvas, src, angle, zoom, offset_x, offset_y, pivot_x, pivot_y, antialias);
    lua_pushvalue(L, 1);
    return 1;
}

/*
旋转图片
@param self 画布实例或指针
@param src 源图片描述符指针
@param angle 旋转角度
@param x X坐标(可选)
@param y Y坐标(可选)
@param pivot_x 旋转中心X(可选)
@param pivot_y 旋转中心Y(可选)
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
    lv_canvas_rotate(canvas, src, angle, x, y, pivot_x, pivot_y);
    lua_pushvalue(L, 1);
    return 1;
}

/*
初始化图层
@param self 画布实例或指针
@return userdata 图层指针
@usage local layer = canvas:init_layer()
*/
static int lvgl_canvas_init_layer(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    lv_canvas_layer_t* layer = (lv_canvas_layer_t*)cm_malloc(sizeof(lv_canvas_layer_t));
    if (!layer) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    lv_canvas_init_layer(canvas, layer);
    lua_pushlightuserdata(L, layer);
    return 1;
}

/*
完成图层
@param self 画布实例或指针
@param layer 图层指针
@return self
@usage canvas:finish_layer(layer)
*/
static int lvgl_canvas_finish_layer(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    lv_canvas_layer_t* layer = (lv_canvas_layer_t*)luaL_checklightuserdata(L, 2);
    lv_canvas_finish_layer(canvas, layer);
    lua_pushvalue(L, 1);
    return 1;
}

/*
应用图层
@param self 画布实例或指针
@param layer 图层指针
@return self
@usage canvas:apply_layer(layer)
*/
static int lvgl_canvas_apply_layer(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    lv_canvas_layer_t* layer = (lv_canvas_layer_t*)luaL_checklightuserdata(L, 2);
    lv_canvas_apply_layer(canvas, layer);
    cm_free(layer);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图层背景
@param self 画布实例或指针
@param color 颜色值
@param opa 透明度值(可选)
@return self
@usage canvas:set_layer_bg(0xFFFFFF, 255)
*/
static int lvgl_canvas_set_layer_bg(lua_State* L) {
    lv_obj_t* canvas = lvgl_get_obj_ptr(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    lv_opa_t opa = (lv_opa_t)luaL_optinteger(L, 3, LV_OPA_COVER);
    lv_canvas_set_layer_bg(canvas, color, opa);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 canvas 子模块 */
void lvgl_register_canvas(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
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

    /* 保存组件metatable引用(用于继承) */
    canvas_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.canvas.set_buffer(canvas, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, canvas_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.canvas) */
    REG_METHOD(L, "create", lvgl_canvas_create);
}