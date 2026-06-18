/*
@module  lvgl.style
@summary LVGL样式系统(OO风格)
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")

-- 创建样式(传统方式)
local style1 = lvgl.style.create()

-- 创建样式(对象方式,支持表参数)
local style2 = lvgl.style.create({
    width = 100,
    height = 50,
    radius = 10,
    bg_color = 0x3366FF,
    bg_opa = 255,
    pad_all = 10,
    border_width = 2,
    border_color = 0x000000,
    text_color = 0xFFFFFF,
    text_align = lvgl.TEXT_ALIGN_CENTER,
    shadow_width = 10,
    shadow_color = 0x000000,
    shadow_opa = 100
})

-- 使用OO风格设置属性
style1:set_width(100)
style1:set_height(50)
style1:set_radius(10)
style1:set_bg_color(0x3366FF)

-- 链式调用
style1:set_width(100):set_height(50):set_radius(10)

-- 将样式应用到对象
btn:add_style(style1)

-- 删除样式
style1:delete()
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* style组件的metatable引用 */
static int style_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_style_create_internal(lua_State* L) {
    lv_style_t* style = (lv_style_t*)cm_malloc(sizeof(lv_style_t));
    if (!style) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }
    lv_style_init(style);
    lua_pushlightuserdata(L, style);
    return 1;
}

/*
从表创建样式(内部函数)
@param L Lua状态
@param idx 表的索引
@return lv_style_t* 样式指针
*/
static lv_style_t* lvgl_style_create_from_table_internal(lua_State* L, int idx) {
    lv_style_t* style = (lv_style_t*)cm_malloc(sizeof(lv_style_t));
    if (!style) {
        return NULL;
    }
    lv_style_init(style);

    /* 检查并应用各属性 */
    lua_getfield(L, idx, "width");
    if (lua_isinteger(L, -1)) {
        lv_style_set_width(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "height");
    if (lua_isinteger(L, -1)) {
        lv_style_set_height(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "min_width");
    if (lua_isinteger(L, -1)) {
        lv_style_set_min_width(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "min_height");
    if (lua_isinteger(L, -1)) {
        lv_style_set_min_height(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "max_width");
    if (lua_isinteger(L, -1)) {
        lv_style_set_max_width(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "max_height");
    if (lua_isinteger(L, -1)) {
        lv_style_set_max_height(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "radius");
    if (lua_isinteger(L, -1)) {
        lv_style_set_radius(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "pad_all");
    if (lua_isinteger(L, -1)) {
        lv_style_set_pad_all(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "pad_top");
    if (lua_isinteger(L, -1)) {
        lv_style_set_pad_top(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "pad_bottom");
    if (lua_isinteger(L, -1)) {
        lv_style_set_pad_bottom(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "pad_left");
    if (lua_isinteger(L, -1)) {
        lv_style_set_pad_left(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "pad_right");
    if (lua_isinteger(L, -1)) {
        lv_style_set_pad_right(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "pad_row");
    if (lua_isinteger(L, -1)) {
        lv_style_set_pad_row(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "pad_column");
    if (lua_isinteger(L, -1)) {
        lv_style_set_pad_column(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "bg_color");
    if (lua_isinteger(L, -1)) {
        lv_color_t color;
        color.full = (uint32_t)lua_tointeger(L, -1);
        lv_style_set_bg_color(style, color);
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "bg_opa");
    if (lua_isinteger(L, -1)) {
        lv_style_set_bg_opa(style, (lv_opa_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "bg_grad_color");
    if (lua_isinteger(L, -1)) {
        lv_color_t color;
        color.full = (uint32_t)lua_tointeger(L, -1);
        lv_style_set_bg_grad_color(style, color);
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "bg_grad_dir");
    if (lua_isinteger(L, -1)) {
        lv_style_set_bg_grad_dir(style, (lv_grad_dir_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "border_width");
    if (lua_isinteger(L, -1)) {
        lv_style_set_border_width(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "border_color");
    if (lua_isinteger(L, -1)) {
        lv_color_t color;
        color.full = (uint32_t)lua_tointeger(L, -1);
        lv_style_set_border_color(style, color);
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "border_opa");
    if (lua_isinteger(L, -1)) {
        lv_style_set_border_opa(style, (lv_opa_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "border_side");
    if (lua_isinteger(L, -1)) {
        lv_style_set_border_side(style, (lv_border_side_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "text_color");
    if (lua_isinteger(L, -1)) {
        lv_color_t color;
        color.full = (uint32_t)lua_tointeger(L, -1);
        lv_style_set_text_color(style, color);
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "text_opa");
    if (lua_isinteger(L, -1)) {
        lv_style_set_text_opa(style, (lv_opa_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "text_align");
    if (lua_isinteger(L, -1)) {
        lv_style_set_text_align(style, (lv_text_align_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "shadow_width");
    if (lua_isinteger(L, -1)) {
        lv_style_set_shadow_width(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "shadow_color");
    if (lua_isinteger(L, -1)) {
        lv_color_t color;
        color.full = (uint32_t)lua_tointeger(L, -1);
        lv_style_set_shadow_color(style, color);
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "shadow_opa");
    if (lua_isinteger(L, -1)) {
        lv_style_set_shadow_opa(style, (lv_opa_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "shadow_spread");
    if (lua_isinteger(L, -1)) {
        lv_style_set_shadow_spread(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "shadow_ofs_x");
    if (lua_isinteger(L, -1)) {
        lv_style_set_shadow_ofs_x(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "shadow_ofs_y");
    if (lua_isinteger(L, -1)) {
        lv_style_set_shadow_ofs_y(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "outline_width");
    if (lua_isinteger(L, -1)) {
        lv_style_set_outline_width(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "outline_color");
    if (lua_isinteger(L, -1)) {
        lv_color_t color;
        color.full = (uint32_t)lua_tointeger(L, -1);
        lv_style_set_outline_color(style, color);
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "outline_opa");
    if (lua_isinteger(L, -1)) {
        lv_style_set_outline_opa(style, (lv_opa_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    lua_getfield(L, idx, "outline_pad");
    if (lua_isinteger(L, -1)) {
        lv_style_set_outline_pad(style, (lv_coord_t)lua_tointeger(L, -1));
    }
    lua_pop(L, 1);

    return style;
}

/* ==================== 样式OO方法 ==================== */

/*
创建样式
@param tab 可选,包含样式属性的表
@return userdata 样式指针
@usage local style = lvgl.style.create()
@usage local style = lvgl.style.create({width=100, height=50, radius=10})
*/
static int lvgl_style_create(lua_State* L) {
    /* 检查是否传入了表参数 */
    if (lua_istable(L, 1)) {
        lv_style_t* style = lvgl_style_create_from_table_internal(L, 1);
        if (!style) {
            luaL_error(L, "memory allocation failed");
            return 0;
        }
        lua_pushlightuserdata(L, style);
        return 1;
    }

    /* 传统方式创建 */
    return lvgl_obj_create_instance(L, lvgl_style_create_internal, style_metatable_ref);
}

/*
删除样式
@param self 样式实例
@return nil
@usage style:delete()
*/
static int lvgl_style_delete(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    if (style) {
        lv_style_reset(style);
        cm_free(style);
    }
    return 0;
}

/*
重置样式
@param self 样式实例
@return self
@usage style:reset()
*/
static int lvgl_style_reset(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    if (style) {
        lv_style_reset(style);
    }
    return 1;
}

/* ==================== 尺寸属性 ==================== */

static int lvgl_style_set_width(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_width(style, value);
    }
    return 1;
}

static int lvgl_style_set_height(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_height(style, value);
    }
    return 1;
}

static int lvgl_style_set_min_width(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_min_width(style, value);
    }
    return 1;
}

static int lvgl_style_set_min_height(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_min_height(style, value);
    }
    return 1;
}

static int lvgl_style_set_max_width(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_max_width(style, value);
    }
    return 1;
}

static int lvgl_style_set_max_height(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_max_height(style, value);
    }
    return 1;
}

/* ==================== 边距属性 ==================== */

static int lvgl_style_set_pad_top(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_pad_top(style, value);
    }
    return 1;
}

static int lvgl_style_set_pad_bottom(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_pad_bottom(style, value);
    }
    return 1;
}

static int lvgl_style_set_pad_left(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_pad_left(style, value);
    }
    return 1;
}

static int lvgl_style_set_pad_right(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_pad_right(style, value);
    }
    return 1;
}

static int lvgl_style_set_pad_row(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_pad_row(style, value);
    }
    return 1;
}

static int lvgl_style_set_pad_column(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_pad_column(style, value);
    }
    return 1;
}

static int lvgl_style_set_pad_all(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_pad_all(style, value);
    }
    return 1;
}

/* ==================== 背景属性 ==================== */

static int lvgl_style_set_bg_color(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_bg_color(style, color);
    }
    return 1;
}

static int lvgl_style_set_bg_opa(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_opa_t opa = (lv_opa_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_bg_opa(style, opa);
    }
    return 1;
}

static int lvgl_style_set_bg_grad_color(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_bg_grad_color(style, color);
    }
    return 1;
}

static int lvgl_style_set_bg_grad_dir(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_grad_dir_t dir = (lv_grad_dir_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_bg_grad_dir(style, dir);
    }
    return 1;
}

static int lvgl_style_set_bg_grad_stop(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    uint8_t stop = (uint8_t)luaL_checkinteger(L, 2);
    lv_opa_t opa = (lv_opa_t)luaL_checkinteger(L, 3);
    if (style) {
        lv_style_set_bg_grad_stop(style, stop, opa);
    }
    return 1;
}

/* ==================== 边框属性 ==================== */

static int lvgl_style_set_border_width(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_border_width(style, value);
    }
    return 1;
}

static int lvgl_style_set_border_color(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_border_color(style, color);
    }
    return 1;
}

static int lvgl_style_set_border_opa(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_opa_t opa = (lv_opa_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_border_opa(style, opa);
    }
    return 1;
}

static int lvgl_style_set_border_side(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_border_side_t side = (lv_border_side_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_border_side(style, side);
    }
    return 1;
}

/* ==================== 圆角属性 ==================== */

static int lvgl_style_set_radius(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_radius(style, value);
    }
    return 1;
}

/* ==================== 文本属性 ==================== */

static int lvgl_style_set_text_color(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_text_color(style, color);
    }
    return 1;
}

static int lvgl_style_set_text_opa(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_opa_t opa = (lv_opa_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_text_opa(style, opa);
    }
    return 1;
}

static int lvgl_style_set_text_font(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    const lv_font_t* font = (const lv_font_t*)luaL_checklightuserdata(L, 2);
    if (style) {
        lv_style_set_text_font(style, font);
    }
    return 1;
}

static int lvgl_style_set_text_align(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_text_align_t align = (lv_text_align_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_text_align(style, align);
    }
    return 1;
}

/* ==================== 图片属性 ==================== */

static int lvgl_style_set_img_recolor(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_img_recolor(style, color);
    }
    return 1;
}

static int lvgl_style_set_img_recolor_opa(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_opa_t opa = (lv_opa_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_img_recolor_opa(style, opa);
    }
    return 1;
}

/* ==================== 阴影属性 ==================== */

static int lvgl_style_set_shadow_width(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_shadow_width(style, value);
    }
    return 1;
}

static int lvgl_style_set_shadow_color(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_shadow_color(style, color);
    }
    return 1;
}

static int lvgl_style_set_shadow_opa(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_opa_t opa = (lv_opa_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_shadow_opa(style, opa);
    }
    return 1;
}

static int lvgl_style_set_shadow_spread(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_shadow_spread(style, value);
    }
    return 1;
}

static int lvgl_style_set_shadow_ofs_x(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_shadow_ofs_x(style, value);
    }
    return 1;
}

static int lvgl_style_set_shadow_ofs_y(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_shadow_ofs_y(style, value);
    }
    return 1;
}

/* ==================== 轮廓属性 ==================== */

static int lvgl_style_set_outline_width(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_outline_width(style, value);
    }
    return 1;
}

static int lvgl_style_set_outline_color(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_outline_color(style, color);
    }
    return 1;
}

static int lvgl_style_set_outline_opa(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_opa_t opa = (lv_opa_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_outline_opa(style, opa);
    }
    return 1;
}

static int lvgl_style_set_outline_pad(lua_State* L) {
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 1);
    lv_coord_t value = (lv_coord_t)luaL_checkinteger(L, 2);
    if (style) {
        lv_style_set_outline_pad(style, value);
    }
    return 1;
}

/* 注册 style 子模块 */
void lvgl_register_style(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "delete", lvgl_style_delete);
    REG_METHOD(L, "reset", lvgl_style_reset);

    /* 尺寸属性 */
    REG_METHOD(L, "set_width", lvgl_style_set_width);
    REG_METHOD(L, "set_height", lvgl_style_set_height);
    REG_METHOD(L, "set_min_width", lvgl_style_set_min_width);
    REG_METHOD(L, "set_min_height", lvgl_style_set_min_height);
    REG_METHOD(L, "set_max_width", lvgl_style_set_max_width);
    REG_METHOD(L, "set_max_height", lvgl_style_set_max_height);

    /* 边距属性 */
    REG_METHOD(L, "set_pad_top", lvgl_style_set_pad_top);
    REG_METHOD(L, "set_pad_bottom", lvgl_style_set_pad_bottom);
    REG_METHOD(L, "set_pad_left", lvgl_style_set_pad_left);
    REG_METHOD(L, "set_pad_right", lvgl_style_set_pad_right);
    REG_METHOD(L, "set_pad_row", lvgl_style_set_pad_row);
    REG_METHOD(L, "set_pad_column", lvgl_style_set_pad_column);
    REG_METHOD(L, "set_pad_all", lvgl_style_set_pad_all);

    /* 背景属性 */
    REG_METHOD(L, "set_bg_color", lvgl_style_set_bg_color);
    REG_METHOD(L, "set_bg_opa", lvgl_style_set_bg_opa);
    REG_METHOD(L, "set_bg_grad_color", lvgl_style_set_bg_grad_color);
    REG_METHOD(L, "set_bg_grad_dir", lvgl_style_set_bg_grad_dir);
    REG_METHOD(L, "set_bg_grad_stop", lvgl_style_set_bg_grad_stop);

    /* 边框属性 */
    REG_METHOD(L, "set_border_width", lvgl_style_set_border_width);
    REG_METHOD(L, "set_border_color", lvgl_style_set_border_color);
    REG_METHOD(L, "set_border_opa", lvgl_style_set_border_opa);
    REG_METHOD(L, "set_border_side", lvgl_style_set_border_side);

    /* 圆角属性 */
    REG_METHOD(L, "set_radius", lvgl_style_set_radius);

    /* 文本属性 */
    REG_METHOD(L, "set_text_color", lvgl_style_set_text_color);
    REG_METHOD(L, "set_text_opa", lvgl_style_set_text_opa);
    REG_METHOD(L, "set_text_font", lvgl_style_set_text_font);
    REG_METHOD(L, "set_text_align", lvgl_style_set_text_align);

    /* 图片属性 */
    REG_METHOD(L, "set_img_recolor", lvgl_style_set_img_recolor);
    REG_METHOD(L, "set_img_recolor_opa", lvgl_style_set_img_recolor_opa);

    /* 阴影属性 */
    REG_METHOD(L, "set_shadow_width", lvgl_style_set_shadow_width);
    REG_METHOD(L, "set_shadow_color", lvgl_style_set_shadow_color);
    REG_METHOD(L, "set_shadow_opa", lvgl_style_set_shadow_opa);
    REG_METHOD(L, "set_shadow_spread", lvgl_style_set_shadow_spread);
    REG_METHOD(L, "set_shadow_ofs_x", lvgl_style_set_shadow_ofs_x);
    REG_METHOD(L, "set_shadow_ofs_y", lvgl_style_set_shadow_ofs_y);

    /* 轮廓属性 */
    REG_METHOD(L, "set_outline_width", lvgl_style_set_outline_width);
    REG_METHOD(L, "set_outline_color", lvgl_style_set_outline_color);
    REG_METHOD(L, "set_outline_opa", lvgl_style_set_outline_opa);
    REG_METHOD(L, "set_outline_pad", lvgl_style_set_outline_pad);

    /* 保存组件metatable引用(用于继承) */
    style_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.style.set_radius(style, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, style_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.style) */
    REG_METHOD(L, "create", lvgl_style_create);
}
