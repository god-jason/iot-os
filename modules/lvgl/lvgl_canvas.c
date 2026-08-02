/**
 * @file iot_lvgl_canvas.c
 * @brief LVGL画布控件
 *
 * 实现LVGL画布控件的OO风格Lua绑定，包括画布创建、设置缓冲区、设置像素、填充背景等基础接口。
 * 适配 LVGL 9.5.0 API：使用 lv_color_format_t 替代 lv_img_cf_t，使用 lv_canvas_set_px 替代 set_px_color/set_px_opa。
 * 注：LVGL 9 移除了 lv_canvas_draw_rect/line/arc/text 等绘制函数，需通过 lv_canvas_init_layer + lv_draw_* 实现。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* canvas组件的metatable引用 */
static int canvas_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_canvas_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
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
static int iot_lvgl_canvas_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_canvas_create_internal, canvas_metatable_ref);
}

/*
设置画布缓冲区
@param self 画布实例或指针
@param w 宽度
@param h 高度
@param buf 缓冲区指针
@param cf 颜色格式(可选,默认 LV_COLOR_FORMAT_RGB888)
@return self
@usage canvas:set_buffer(buf, 200, 200)
*/
static int iot_lvgl_canvas_set_buffer(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    int32_t w = (int32_t)luaL_checkinteger(L, 2);
    int32_t h = (int32_t)luaL_checkinteger(L, 3);
    void* buf = luaL_checklightuserdata(L, 4);
    /* LVGL 9: 使用 lv_color_format_t 替代 lv_img_cf_t */
    lv_color_format_t cf = (lv_color_format_t)luaL_optinteger(L, 5, LV_COLOR_FORMAT_RGB888);
    lv_canvas_set_buffer(canvas, buf, w, h, cf);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置像素颜色和透明度
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param color 颜色值
@param opa 透明度(可选,默认 LV_OPA_COVER)
@return self
@usage canvas:set_px(10, 10, 0xFF0000, 255)
*/
static int iot_lvgl_canvas_set_px(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_color_t color = lv_color_from_u32((uint32_t)luaL_checkinteger(L, 4));
    lv_opa_t opa = (lv_opa_t)luaL_optinteger(L, 5, LV_OPA_COVER);
    /* LVGL 9: lv_canvas_set_px 接受 color + opa 参数 */
    lv_canvas_set_px(canvas, x, y, color, opa);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置像素颜色(兼容旧接口,opa=COVER)
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param color 颜色值
@return self
@usage canvas:set_px_color(10, 10, 0xFF0000)
*/
static int iot_lvgl_canvas_set_px_color(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_color_t color = lv_color_from_u32((uint32_t)luaL_checkinteger(L, 4));
    lv_canvas_set_px(canvas, x, y, color, LV_OPA_COVER);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置像素透明度(LVGL 9 已合并到 set_px,此为兼容存根)
@param self 画布实例或指针
@param x X坐标
@param y Y坐标
@param opa 透明度值(0-255)
@return self
@usage canvas:set_px_opa(10, 10, 128)
*/
static int iot_lvgl_canvas_set_px_opa(lua_State* L) {
    /* LVGL 9: 透明度合并到 set_px，单独设置 opa 需读取后写回 */
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_opa_t opa = (lv_opa_t)luaL_checkinteger(L, 4);
    lv_color32_t px = lv_canvas_get_px(canvas, x, y);
    lv_color_t color = lv_color_from_u32((px.red << 16) | (px.green << 8) | px.blue);
    lv_canvas_set_px(canvas, x, y, color, opa);
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
static int iot_lvgl_canvas_get_px_color(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    /* LVGL 9: lv_canvas_get_px 返回 lv_color32_t (ARGB8888) */
    lv_color32_t px = lv_canvas_get_px(canvas, x, y);
    uint32_t color = ((uint32_t)px.red << 16) | ((uint32_t)px.green << 8) | (uint32_t)px.blue;
    lua_pushinteger(L, color);
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
static int iot_lvgl_canvas_get_px_opa(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_color32_t px = lv_canvas_get_px(canvas, x, y);
    lua_pushinteger(L, px.alpha);
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
static int iot_lvgl_canvas_fill_bg(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    lv_color_t color = lv_color_from_u32((uint32_t)luaL_checkinteger(L, 2));
    lv_opa_t opa = (lv_opa_t)luaL_optinteger(L, 3, LV_OPA_COVER);
    lv_canvas_fill_bg(canvas, color, opa);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置调色板颜色(仅索引颜色格式)
@param self 画布实例或指针
@param index 调色板索引
@param color 颜色值(RGB)
@return self
@usage canvas:set_palette(0, 0xFF0000)
*/
static int iot_lvgl_canvas_set_palette(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    uint8_t index = (uint8_t)luaL_checkinteger(L, 2);
    uint32_t c = (uint32_t)luaL_checkinteger(L, 3);
    lv_color32_t color = { .alpha = 0xFF, .red = (c >> 16) & 0xFF, .green = (c >> 8) & 0xFF, .blue = c & 0xFF };
    lv_canvas_set_palette(canvas, index, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取画布图像描述符
@param self 画布实例或指针
@return userdata 图像描述符指针
@usage local img = canvas:get_img()
*/
static int iot_lvgl_canvas_get_img(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    /* LVGL 9: lv_canvas_get_image 替代 lv_canvas_get_img */
    lv_image_dsc_t* img = lv_canvas_get_image(canvas);
    lua_pushlightuserdata(L, img);
    return 1;
}

/*
获取画布缓冲区指针
@param self 画布实例或指针
@return userdata 缓冲区指针
@usage local buf = canvas:get_buf()
*/
static int iot_lvgl_canvas_get_buf(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    const void* buf = lv_canvas_get_buf(canvas);
    lua_pushlightuserdata(L, (void*)buf);
    return 1;
}

/*
初始化图层(用于 LVGL 9 通用绘制)
@param self 画布实例或指针
@return userdata 图层指针(调用者需负责释放)
@usage local layer = canvas:init_layer()
*/
static int iot_lvgl_canvas_init_layer(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    lv_layer_t* layer = (lv_layer_t*)cm_malloc(sizeof(lv_layer_t));
    if (!layer) {
        return luaL_error(L, "memory allocation failed");
    }
    lv_canvas_init_layer(canvas, layer);
    lua_pushlightuserdata(L, layer);
    return 1;
}

/*
完成图层绘制(与 init_layer 配对使用)
@param self 画布实例或指针
@param layer 图层指针
@return self
@usage canvas:finish_layer(layer)
*/
static int iot_lvgl_canvas_finish_layer(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    lv_layer_t* layer = (lv_layer_t*)luaL_checklightuserdata(L, 2);
    lv_canvas_finish_layer(canvas, layer);
    cm_free(layer);
    lua_pushvalue(L, 1);
    return 1;
}

/*
复制缓冲区(LVGL 9 API)
@param self 画布实例或指针
@param canvas_area 画布目标区域 {x1,y1,x2,y2}
@param src_buf 源缓冲区指针
@param src_area 源区域 {x1,y1,x2,y2}(可选)
@return self
@usage canvas:copy_buf({0,0,99,99}, src_buf, nil)
*/
static int iot_lvgl_canvas_copy_buf(lua_State* L) {
    lv_obj_t* canvas = iot_lvgl_get_obj_ptr(L, 1);
    /* LVGL 9: lv_canvas_copy_buf 签名变更为 (obj, canvas_area, src_buf, src_area) */
    luaL_checktype(L, 2, LUA_TTABLE);
    lv_area_t canvas_area;
    lua_geti(L, 2, 1); canvas_area.x1 = (int32_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_geti(L, 2, 2); canvas_area.y1 = (int32_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_geti(L, 2, 3); canvas_area.x2 = (int32_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
    lua_geti(L, 2, 4); canvas_area.y2 = (int32_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
    lv_draw_buf_t* src_buf = (lv_draw_buf_t*)luaL_checklightuserdata(L, 3);
    lv_area_t src_area;
    lv_area_t* src_area_p = NULL;
    if (!lua_isnoneornil(L, 4)) {
        luaL_checktype(L, 4, LUA_TTABLE);
        lua_geti(L, 4, 1); src_area.x1 = (int32_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_geti(L, 4, 2); src_area.y1 = (int32_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_geti(L, 4, 3); src_area.x2 = (int32_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
        lua_geti(L, 4, 4); src_area.y2 = (int32_t)luaL_checkinteger(L, -1); lua_pop(L, 1);
        src_area_p = &src_area;
    }
    lv_canvas_copy_buf(canvas, &canvas_area, src_buf, src_area_p);
    lua_pushvalue(L, 1);
    return 1;
}

/* ==================== 已移除 API 的兼容存根 ==================== */
/* LVGL 9 移除了 lv_canvas_draw_rect/line/arc/text/img/polygon 以及 blur_hor/ver/transform/rotate。
 * 这些功能现在需要通过 lv_canvas_init_layer + lv_draw_* + lv_canvas_finish_layer 实现。
 * 此处保留方法名作为存根，避免旧 Lua 代码调用时报错。 */

static int iot_lvgl_canvas_draw_rect_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_draw_circle_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_draw_line_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_draw_polygon_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_draw_arc_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_draw_text_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_draw_img_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_blur_hor_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_blur_ver_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_transform_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_rotate_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_canvas_set_layer_bg_stub(lua_State* L) {
    (void)iot_lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    (void)luaL_optinteger(L, 3, LV_OPA_COVER);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 canvas 子模块 */
void iot_lvgl_register_canvas(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_buffer", iot_lvgl_canvas_set_buffer);
    REG_METHOD(L, "set_px", iot_lvgl_canvas_set_px);
    REG_METHOD(L, "set_px_color", iot_lvgl_canvas_set_px_color);
    REG_METHOD(L, "set_px_opa", iot_lvgl_canvas_set_px_opa);
    REG_METHOD(L, "get_px_color", iot_lvgl_canvas_get_px_color);
    REG_METHOD(L, "get_px_opa", iot_lvgl_canvas_get_px_opa);
    REG_METHOD(L, "fill_bg", iot_lvgl_canvas_fill_bg);
    REG_METHOD(L, "set_palette", iot_lvgl_canvas_set_palette);
    REG_METHOD(L, "get_img", iot_lvgl_canvas_get_img);
    REG_METHOD(L, "get_buf", iot_lvgl_canvas_get_buf);
    REG_METHOD(L, "init_layer", iot_lvgl_canvas_init_layer);
    REG_METHOD(L, "finish_layer", iot_lvgl_canvas_finish_layer);
    REG_METHOD(L, "copy_buf", iot_lvgl_canvas_copy_buf);
    /* 已移除 API 的兼容存根 */
    REG_METHOD(L, "draw_rect", iot_lvgl_canvas_draw_rect_stub);
    REG_METHOD(L, "draw_circle", iot_lvgl_canvas_draw_circle_stub);
    REG_METHOD(L, "draw_line", iot_lvgl_canvas_draw_line_stub);
    REG_METHOD(L, "draw_polygon", iot_lvgl_canvas_draw_polygon_stub);
    REG_METHOD(L, "draw_arc", iot_lvgl_canvas_draw_arc_stub);
    REG_METHOD(L, "draw_text", iot_lvgl_canvas_draw_text_stub);
    REG_METHOD(L, "draw_img", iot_lvgl_canvas_draw_img_stub);
    REG_METHOD(L, "blur_hor", iot_lvgl_canvas_blur_hor_stub);
    REG_METHOD(L, "blur_ver", iot_lvgl_canvas_blur_ver_stub);
    REG_METHOD(L, "transform", iot_lvgl_canvas_transform_stub);
    REG_METHOD(L, "rotate", iot_lvgl_canvas_rotate_stub);
    REG_METHOD(L, "set_layer_bg", iot_lvgl_canvas_set_layer_bg_stub);

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

    /* 注册create函数到主表lvgl.canvas) */
    REG_METHOD(L, "create", iot_lvgl_canvas_create);
}
