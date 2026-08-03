/**
 * @file iot_lvgl_layout.c
 * @brief LVGL布局系统
 *
 * 实现LVGL布局系统的Lua绑定，包括Flex布局初始化、设置Flex流/对齐/增长因子、Grid布局初始化、设置Grid单元格/对齐、设置对象布局类型等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ==================== Flex布局 ==================== */

static int iot_lvgl_layout_flex_init(lua_State* L) {
    lv_flex_init();
    return 0;
}

static int iot_lvgl_layout_flex_set_flow(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    lv_flex_flow_t flow = (lv_flex_flow_t)luaL_checkinteger(L, 2);
    lv_obj_set_flex_flow(obj, flow);
    return 0;
}

static int iot_lvgl_layout_flex_set_align(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    lv_flex_align_t main_place = (lv_flex_align_t)luaL_checkinteger(L, 2);
    lv_flex_align_t cross_place = (lv_flex_align_t)luaL_checkinteger(L, 3);
    lv_flex_align_t track_place = (lv_flex_align_t)luaL_optinteger(L, 4, main_place);
    lv_obj_set_flex_align(obj, main_place, cross_place, track_place);
    return 0;
}

static int iot_lvgl_layout_flex_set_grow(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    uint8_t grow = (uint8_t)luaL_checkinteger(L, 2);
    lv_obj_set_flex_grow(obj, grow);
    return 0;
}

/* ==================== Grid布局 ==================== */

static int iot_lvgl_layout_grid_init(lua_State* L) {
    lv_grid_init();
    return 0;
}

static int iot_lvgl_layout_grid_set_template(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    if (!obj) {
        luaL_error(L, "invalid object");
        return 0;
    }

    int32_t* col_dsc = NULL;
    int32_t* row_dsc = NULL;

    /* 处理列描述符表 */
    if (!lua_isnil(L, 2)) {
        luaL_checktype(L, 2, LUA_TTABLE);
        int col_count = (int)lua_rawlen(L, 2);
        col_dsc = (int32_t*)cm_malloc((col_count + 1) * sizeof(int32_t));
        if (!col_dsc) {
            luaL_error(L, "memory allocation failed");
            return 0;
        }
        for (int i = 0; i < col_count; i++) {
            lua_rawgeti(L, 2, i + 1);
            col_dsc[i] = (int32_t)luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
        col_dsc[col_count] = LV_GRID_TEMPLATE_LAST;
    }

    /* 处理行描述符表 */
    if (!lua_isnil(L, 3)) {
        luaL_checktype(L, 3, LUA_TTABLE);
        int row_count = (int)lua_rawlen(L, 3);
        row_dsc = (int32_t*)cm_malloc((row_count + 1) * sizeof(int32_t));
        if (!row_dsc) {
            if (col_dsc) cm_free(col_dsc);
            luaL_error(L, "memory allocation failed");
            return 0;
        }
        for (int i = 0; i < row_count; i++) {
            lua_rawgeti(L, 3, i + 1);
            row_dsc[i] = (int32_t)luaL_checkinteger(L, -1);
            lua_pop(L, 1);
        }
        row_dsc[row_count] = LV_GRID_TEMPLATE_LAST;
    }

    /* 设置网格描述符数组 */
    lv_obj_set_grid_dsc_array(obj, col_dsc, row_dsc);

    /* 释放临时数组 */
    if (col_dsc) cm_free(col_dsc);
    if (row_dsc) cm_free(row_dsc);

    /* 返回对象本身，支持链式调用 */
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_layout_grid_set_cell(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    uint8_t col_pos = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t col_span = (uint8_t)luaL_checkinteger(L, 3);
    uint8_t row_pos = (uint8_t)luaL_checkinteger(L, 4);
    uint8_t row_span = (uint8_t)luaL_checkinteger(L, 5);
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col_pos, col_span, LV_GRID_ALIGN_START, row_pos, row_span);
    return 0;
}

static int iot_lvgl_layout_grid_set_align(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    lv_grid_align_t column_align = (lv_grid_align_t)luaL_checkinteger(L, 2);
    lv_grid_align_t row_align = (lv_grid_align_t)luaL_optinteger(L, 3, column_align);
    lv_obj_set_grid_align(obj, column_align, row_align);
    return 0;
}

/* ==================== 布局属性 ==================== */

static int iot_lvgl_layout_set(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    uint32_t layout = (uint32_t)luaL_checkinteger(L, 2);
    lv_obj_set_layout(obj, layout);
    return 0;
}

static int iot_lvgl_layout_get(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    uint32_t layout = lv_obj_get_style_layout(obj, LV_PART_MAIN);
    lua_pushinteger(L, layout);
    return 1;
}

static int iot_lvgl_layout_update(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    lv_obj_update_layout(obj);
    return 0;
}

static int iot_lvgl_layout_have_size_dependency(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    (void)luaL_optinteger(L, 2, LV_LAYOUT_FLEX);
    lv_coord_t w = lv_obj_get_style_width(obj, LV_PART_MAIN);
    lv_coord_t h = lv_obj_get_style_height(obj, LV_PART_MAIN);
    bool has = (w == LV_SIZE_CONTENT || h == LV_SIZE_CONTENT ||
                LV_COORD_IS_PCT(w) || LV_COORD_IS_PCT(h));
    lua_pushboolean(L, has);
    return 1;
}

/* 注册 layout 子模块 */
void iot_lvgl_register_layout(lua_State* L) {
    /* Flex布局 */
    REG_METHOD(L, "flex_init", iot_lvgl_layout_flex_init);
    REG_METHOD(L, "flex_set_flow", iot_lvgl_layout_flex_set_flow);
    REG_METHOD(L, "flex_set_align", iot_lvgl_layout_flex_set_align);
    REG_METHOD(L, "flex_set_grow", iot_lvgl_layout_flex_set_grow);

    /* Grid布局 */
    REG_METHOD(L, "grid_init", iot_lvgl_layout_grid_init);
    REG_METHOD(L, "grid_set_template", iot_lvgl_layout_grid_set_template);
    REG_METHOD(L, "grid_set_cell", iot_lvgl_layout_grid_set_cell);
    REG_METHOD(L, "grid_set_align", iot_lvgl_layout_grid_set_align);

    /* 通用布局 */
    REG_METHOD(L, "set", iot_lvgl_layout_set);
    REG_METHOD(L, "get", iot_lvgl_layout_get);
    REG_METHOD(L, "update", iot_lvgl_layout_update);
    REG_METHOD(L, "have_size_dependency", iot_lvgl_layout_have_size_dependency);
}
