/**
 * @file iot_lvgl_chart.c
 * @brief LVGL图表控件
 *
 * 实现LVGL图表控件的OO风格Lua绑定，包括图表创建、设置类型（折线/柱状/散点/面积）、设置点数、设置轴范围、添加数据系列、设置/获取系列值、刷新图表等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* chart组件的metatable引用 */
static int chart_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_chart_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* chart = lv_chart_create(parent);
    lua_pushlightuserdata(L, chart);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
创建图表控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的图表实例
@usage local chart = lvgl.chart.create(scr)
*/
static int iot_lvgl_chart_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_chart_create_internal, chart_metatable_ref);
}

/*
设置图表类型
@param self 图表实例或指针
@param type 图表类型(lvgl.CHART_TYPE_LINE等)
@return self
@usage chart:set_type(lvgl.CHART_TYPE_LINE)
*/
static int iot_lvgl_chart_set_type(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    lv_chart_type_t type = (lv_chart_type_t)luaL_checkinteger(L, 2);
    lv_chart_set_type(chart, type);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图表范围
@param self 图表实例或指针
@param axis 轴类型(lvgl.CHART_AXIS_PRIMARY_Y等)
@param min 最小值
@param max 最大值
@return self
@usage chart:set_range(lvgl.CHART_AXIS_PRIMARY_Y, 0, 100)
*/
static int iot_lvgl_chart_set_range(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    lv_chart_axis_t axis = (lv_chart_axis_t)luaL_checkinteger(L, 2);
    int32_t min = (int32_t)luaL_checkinteger(L, 3);
    int32_t max = (int32_t)luaL_checkinteger(L, 4);
    lv_chart_set_range(chart, axis, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置数据点数量
@param self 图表实例或指针
@param cnt 点数量
@return self
@usage chart:set_point_count(10)
*/
static int iot_lvgl_chart_set_point_count(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    uint16_t cnt = (uint16_t)luaL_checkinteger(L, 2);
    lv_chart_set_point_count(chart, cnt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置分割线数量
@param self 图表实例或指针
@param hdiv 水平分割线数量
@param vdiv 垂直分割线数量
@return self
@usage chart:set_div_line_count(3, 5)
*/
static int iot_lvgl_chart_set_div_line_count(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    uint8_t hdiv = (uint8_t)luaL_checkinteger(L, 2);
    uint8_t vdiv = (uint8_t)luaL_checkinteger(L, 3);
    lv_chart_set_div_line_count(chart, hdiv, vdiv);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置更新模式
@param self 图表实例或指针
@param mode 更新模式
@return self
@usage chart:set_update_mode(lvgl.CHART_UPDATE_MODE_SHIFT)
*/
static int iot_lvgl_chart_set_update_mode(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    lv_chart_update_mode_t mode = (lv_chart_update_mode_t)luaL_checkinteger(L, 2);
    lv_chart_set_update_mode(chart, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置轴刻度
@param self 图表实例或指针
@param axis 轴类型
@param major_len 主刻度长度
@param minor_len 次刻度长度
@param major_cnt 主刻度数量
@param minor_cnt 次刻度数量
@param label_en 是否显示标签
@return self
@usage chart:set_axis_tick(lvgl.CHART_AXIS_PRIMARY_Y, 10, 5, 3, 2, true)
*/
static int iot_lvgl_chart_set_axis_tick(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    lv_chart_axis_t axis = (lv_chart_axis_t)luaL_checkinteger(L, 2);
    int32_t major_len = (int32_t)luaL_checkinteger(L, 3);
    int32_t minor_len = (int32_t)luaL_checkinteger(L, 4);
    int32_t major_cnt = (int32_t)luaL_checkinteger(L, 5);
    int32_t minor_cnt = (int32_t)luaL_checkinteger(L, 6);
    bool label_en = lua_toboolean(L, 7);
    int32_t draw_size = (int32_t)luaL_optinteger(L, 8, 20);
    lv_chart_set_axis_tick(chart, axis, major_len, minor_len, major_cnt, minor_cnt, label_en, draw_size);
    lua_pushvalue(L, 1);
    return 1;
}

/*
添加数据系列
@param self 图表实例或指针
@param color 系列颜色
@return userdata 系列指针
@usage local series = chart:add_series(0xFF0000)
*/
static int iot_lvgl_chart_add_series(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 2);
    lv_chart_axis_t axis = (lv_chart_axis_t)luaL_optinteger(L, 3, LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t* series = lv_chart_add_series(chart, color, axis);
    lua_pushlightuserdata(L, series);
    return 1;
}

/*
设置系列颜色
@param self 图表实例或指针
@param series 系列指针
@param color 新颜色
@return self
@usage chart:set_series_color(series, 0x00FF00)
*/
static int iot_lvgl_chart_set_series_color(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    lv_chart_series_t* series = (lv_chart_series_t*)luaL_checklightuserdata(L, 2);
    lv_color_t color;
    color.full = (uint32_t)luaL_checkinteger(L, 3);
    lv_chart_set_series_color(chart, series, color);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置系列所有值
@param self 图表实例或指针
@param series 系列指针
@param value 值
@return self
@usage chart:set_all_value(series, 50)
*/
static int iot_lvgl_chart_set_all_value(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    lv_chart_series_t* series = (lv_chart_series_t*)luaL_checklightuserdata(L, 2);
    int32_t value = (int32_t)luaL_checkinteger(L, 3);
    lv_chart_set_all_value(chart, series, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置系列单个值
@param self 图表实例或指针
@param series 系列指针
@param id 点索引
@param value 值
@return self
@usage chart:set_value_by_id(series, 0, 50)
*/
static int iot_lvgl_chart_set_value_by_id(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    lv_chart_series_t* series = (lv_chart_series_t*)luaL_checklightuserdata(L, 2);
    uint16_t id = (uint16_t)luaL_checkinteger(L, 3);
    int32_t value = (int32_t)luaL_checkinteger(L, 4);
    lv_chart_set_value_by_id(chart, series, id, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
刷新图表
@param self 图表实例或指针
@return self
@usage chart:refresh()
*/
static int iot_lvgl_chart_refresh(lua_State* L) {
    lv_obj_t* chart = iot_lvgl_get_obj_ptr(L, 1);
    lv_chart_refresh(chart);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 chart 子模块 */
void iot_lvgl_register_chart(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_type", iot_lvgl_chart_set_type);
    REG_METHOD(L, "set_range", iot_lvgl_chart_set_range);
    REG_METHOD(L, "set_point_count", iot_lvgl_chart_set_point_count);
    REG_METHOD(L, "set_div_line_count", iot_lvgl_chart_set_div_line_count);
    REG_METHOD(L, "set_update_mode", iot_lvgl_chart_set_update_mode);
    REG_METHOD(L, "set_axis_tick", iot_lvgl_chart_set_axis_tick);
    REG_METHOD(L, "add_series", iot_lvgl_chart_add_series);
    REG_METHOD(L, "set_series_color", iot_lvgl_chart_set_series_color);
    REG_METHOD(L, "set_all_value", iot_lvgl_chart_set_all_value);
    REG_METHOD(L, "set_value_by_id", iot_lvgl_chart_set_value_by_id);
    REG_METHOD(L, "refresh", iot_lvgl_chart_refresh);

    /* 保存组件metatable引用(用于继承) */
    chart_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.chart.set_type(chart, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, chart_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表lvgl.chart) */
    REG_METHOD(L, "create", iot_lvgl_chart_create);
}
