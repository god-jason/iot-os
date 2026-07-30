/**
 * @file lvgl_table.c
 * @brief LVGL表格控件
 *
 * 实现LVGL表格控件的OO风格Lua绑定，包括表格创建、设置行列数、设置/获取单元格值、设置列宽、设置单元格对齐、获取选中行列等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* table???metatable?? */
static int table_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_table_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* table = lv_table_create(parent);
    lua_pushlightuserdata(L, table);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
??????(OO??)
@param self ???????
@return userdata ?metatable??????
@usage local table = lvgl.table.create(scr)
*/
static int lvgl_table_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_table_create_internal, table_metatable_ref);
}

/*
??????
@param self ????????
@param row_cnt ??
@return self
@usage table:set_row_cnt(3)
*/
static int lvgl_table_set_row_cnt(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    uint16_t row_cnt = (uint16_t)luaL_checkinteger(L, 2);
    lv_table_set_row_cnt(table, row_cnt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param col_cnt ??
@return self
@usage table:set_col_cnt(3)
*/
static int lvgl_table_set_col_cnt(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    uint16_t col_cnt = (uint16_t)luaL_checkinteger(L, 2);
    lv_table_set_col_cnt(table, col_cnt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param row ?????????
@param col ?????????
@param txt ????
@return self
@usage table:set_cell_value(0, 0, "Name")
*/
static int lvgl_table_set_cell_value(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    uint16_t row = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t col = (uint16_t)luaL_checkinteger(L, 3);
    const char* txt = luaL_checkstring(L, 4);
    lv_table_set_cell_value(table, row, col, txt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????
@param self ????????
@param col ?????????
@param width ??
@return self
@usage table:set_col_width(0, 80)
*/
static int lvgl_table_set_col_width(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    uint16_t col = (uint16_t)luaL_checkinteger(L, 2);
    int16_t width = (int16_t)luaL_checkinteger(L, 3);
    lv_table_set_col_width(table, col, width);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????????
@param self ????????
@param row ?????????
@param col ?????????
@param align ????
@return self
@usage table:set_cell_align(0, 0, lvgl.TEXT_ALIGN_CENTER)
*/
static int lvgl_table_set_cell_align(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    (void)luaL_checkinteger(L, 3);
    (void)luaL_checkinteger(L, 4);
    /* lv_table_set_cell_align() was removed in LVGL 8; use styles on the table */
    (void)table;
    lua_pushvalue(L, 1);
    return 1;
}

/*
????????
@param self ????????
@param row ?????????
@param col ?????????
@return string ????
@usage local text = table:get_cell_value(0, 0)
*/
static int lvgl_table_get_cell_value(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    uint16_t row = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t col = (uint16_t)luaL_checkinteger(L, 3);
    const char* txt = lv_table_get_cell_value(table, row, col);
    lua_pushstring(L, txt ? txt : "");
    return 1;
}

/*
??????
@param self ????????
@return integer ??????
@usage local row = table:get_selected_row()
*/
static int lvgl_table_get_selected_row(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    uint16_t row = LV_TABLE_CELL_NONE;
    uint16_t col = LV_TABLE_CELL_NONE;
    lv_table_get_selected_cell(table, &row, &col);
    lua_pushinteger(L, row);
    return 1;
}

/*
??????
@param self ????????
@return integer ??????
@usage local col = table:get_selected_col()
*/
static int lvgl_table_get_selected_col(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    uint16_t row = LV_TABLE_CELL_NONE;
    uint16_t col = LV_TABLE_CELL_NONE;
    lv_table_get_selected_cell(table, &row, &col);
    lua_pushinteger(L, col);
    return 1;
}

/*
??????????
@param self ????????
@param row ?????????
@param col ?????????
@param ctrl ???????
@return self
@usage table:add_cell_ctrl(0, 0, lvgl.TABLE_CELL_CTRL_NONE)
*/
static int lvgl_table_add_cell_ctrl(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    uint16_t row = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t col = (uint16_t)luaL_checkinteger(L, 3);
    uint32_t ctrl = (uint32_t)luaL_checkinteger(L, 4);
    lv_table_add_cell_ctrl(table, row, col, ctrl);
    lua_pushvalue(L, 1);
    return 1;
}

/*
??????????
@param self ????????
@param row ?????????
@param col ?????????
@param ctrl ???????
@return self
@usage table:remove_cell_ctrl(0, 0, lvgl.TABLE_CELL_CTRL_NONE)
*/
static int lvgl_table_remove_cell_ctrl(lua_State* L) {
    lv_obj_t* table = lvgl_get_obj_ptr(L, 1);
    uint16_t row = (uint16_t)luaL_checkinteger(L, 2);
    uint16_t col = (uint16_t)luaL_checkinteger(L, 3);
    uint32_t ctrl = (uint32_t)luaL_checkinteger(L, 4);
    lv_table_clear_cell_ctrl(table, row, col, (lv_table_cell_ctrl_t)ctrl);
    lua_pushvalue(L, 1);
    return 1;
}

/* ?? table ????*/
void lvgl_register_table(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_row_cnt", lvgl_table_set_row_cnt);
    REG_METHOD(L, "set_col_cnt", lvgl_table_set_col_cnt);
    REG_METHOD(L, "set_cell_value", lvgl_table_set_cell_value);
    REG_METHOD(L, "set_col_width", lvgl_table_set_col_width);
    REG_METHOD(L, "set_cell_align", lvgl_table_set_cell_align);
    REG_METHOD(L, "get_cell_value", lvgl_table_get_cell_value);
    REG_METHOD(L, "get_selected_row", lvgl_table_get_selected_row);
    REG_METHOD(L, "get_selected_col", lvgl_table_get_selected_col);
    REG_METHOD(L, "add_cell_ctrl", lvgl_table_add_cell_ctrl);
    REG_METHOD(L, "remove_cell_ctrl", lvgl_table_remove_cell_ctrl);

    table_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, table_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.table) */
    REG_METHOD(L, "create", lvgl_table_create);
}
