/*
@module  lvgl.layout
@summary LVGL布局管理器
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* ==================== Flex布局 ==================== */

static int lvgl_layout_flex_init(lua_State* L) {
    lv_flex_init();
    return 0;
}

static int lvgl_layout_flex_set_flow(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    lv_flex_flow_t flow = (lv_flex_flow_t)luaL_checkinteger(L, 2);
    lv_obj_set_flex_flow(obj, flow);
    return 0;
}

static int lvgl_layout_flex_set_align(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    lv_flex_align_t main_place = (lv_flex_align_t)luaL_checkinteger(L, 2);
    lv_flex_align_t cross_place = (lv_flex_align_t)luaL_checkinteger(L, 3);
    lv_flex_align_t track_place = (lv_flex_align_t)luaL_optinteger(L, 4, main_place);
    lv_obj_set_flex_align(obj, main_place, cross_place, track_place);
    return 0;
}

static int lvgl_layout_flex_set_grow(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    uint8_t grow = (uint8_t)luaL_checkinteger(L, 2);
    lv_obj_set_flex_grow(obj, grow);
    return 0;
}

/* ==================== Grid布局 ==================== */

static int lvgl_layout_grid_init(lua_State* L) {
    lv_grid_init();
    return 0;
}

static int lvgl_layout_grid_set_template(lua_State* L) {
    /* 设置网格模板列/行 */
    luaL_error(L, "grid template not fully supported yet");
    return 0;
}

static int lvgl_layout_grid_set_cell(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    uint8_t col_pos = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t col_span = (uint8_t)luaL_checkinteger(L, 3);
    uint8_t row_pos = (uint8_t)luaL_checkinteger(L, 4);
    uint8_t row_span = (uint8_t)luaL_checkinteger(L, 5);
    lv_obj_set_grid_cell(obj, LV_GRID_ALIGN_START, col_pos, col_span, LV_GRID_ALIGN_START, row_pos, row_span);
    return 0;
}

static int lvgl_layout_grid_set_align(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    lv_grid_align_t column_align = (lv_grid_align_t)luaL_checkinteger(L, 2);
    lv_grid_align_t row_align = (lv_grid_align_t)luaL_optinteger(L, 3, column_align);
    lv_obj_set_grid_align(obj, column_align, row_align);
    return 0;
}

/* ==================== 布局属性 ==================== */

static int lvgl_layout_set(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    uint32_t layout = (uint32_t)luaL_checkinteger(L, 2);
    lv_obj_set_layout(obj, layout);
    return 0;
}

static int lvgl_layout_get(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    uint32_t layout = lv_obj_get_layout(obj);
    lua_pushinteger(L, layout);
    return 1;
}

static int lvgl_layout_update(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    lv_obj_update_layout(obj);
    return 0;
}

static int lvgl_layout_have_size_dependency(lua_State* L) {
    lv_obj_t* obj = (lv_obj_t*)luaL_checklightuserdata(L, 1);
    uint32_t layout = (uint32_t)luaL_optinteger(L, 2, LV_LAYOUT_FLEX);
    bool has = lv_obj_has_flag_any(obj, LV_OBJ_FLAG_LAYOUT_1);
    lua_pushboolean(L, has);
    return 1;
}

/* 注册 layout 子模块 */
void lvgl_register_layout(lua_State* L) {
    /* Flex布局 */
    REG_METHOD(L, "flex_init", lvgl_layout_flex_init);
    REG_METHOD(L, "flex_set_flow", lvgl_layout_flex_set_flow);
    REG_METHOD(L, "flex_set_align", lvgl_layout_flex_set_align);
    REG_METHOD(L, "flex_set_grow", lvgl_layout_flex_set_grow);

    /* Grid布局 */
    REG_METHOD(L, "grid_init", lvgl_layout_grid_init);
    REG_METHOD(L, "grid_set_template", lvgl_layout_grid_set_template);
    REG_METHOD(L, "grid_set_cell", lvgl_layout_grid_set_cell);
    REG_METHOD(L, "grid_set_align", lvgl_layout_grid_set_align);

    /* 通用布局 */
    REG_METHOD(L, "set", lvgl_layout_set);
    REG_METHOD(L, "get", lvgl_layout_get);
    REG_METHOD(L, "update", lvgl_layout_update);
    REG_METHOD(L, "have_size_dependency", lvgl_layout_have_size_dependency);
}
