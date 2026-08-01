/**
 * @file iot_lvgl_dropdown.c
 * @brief LVGL下拉菜单控件
 *
 * 实现LVGL下拉菜单控件的OO风格Lua绑定，包括下拉菜单创建、设置选项（用\n分隔）、设置/获取选中项、设置方向、打开/关闭下拉列表等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* dropdown组件的metatable引用 */
static int dropdown_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_dropdown_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* dd = lv_dropdown_create(parent);
    lua_pushlightuserdata(L, dd);
    return 1;
}

/* ==================== 下拉菜单OO方法 ==================== */

/*
创建下拉菜单控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的下拉菜单实例
@usage local dd = lvgl.dropdown.create(scr)
*/
static int iot_lvgl_dropdown_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_dropdown_create_internal, dropdown_metatable_ref);
}

/*
设置下拉菜单选项
@param self 下拉菜单实例或指针
@param options 选项字符串(用\n分隔)
@return self
@usage dd:set_options("Apple\nBanana\nOrange")
*/
static int iot_lvgl_dropdown_set_options(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    const char* options = luaL_checkstring(L, 2);
    lv_dropdown_set_options(dd, options);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置静态选项(不复制字符串)
@param self 下拉菜单实例或指针
@param options 选项字符串(用\n分隔)
@return self
@usage dd:set_options_static("A\nB\nC")
*/
static int iot_lvgl_dropdown_set_options_static(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    const char* options = luaL_checkstring(L, 2);
    lv_dropdown_set_options_static(dd, options);
    lua_pushvalue(L, 1);
    return 1;
}

/*
添加选项
@param self 下拉菜单实例或指针
@param option 选项文本
@param pos 位置(可选,默认最后)
@return self
@usage dd:add_option("Mango", 3)
*/
static int iot_lvgl_dropdown_add_option(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    const char* option = luaL_checkstring(L, 2);
    int32_t pos = (int32_t)luaL_optinteger(L, 3, LV_DROPDOWN_POS_LAST);
    lv_dropdown_add_option(dd, option, pos);
    lua_pushvalue(L, 1);
    return 1;
}

/*
清除所有选项
@param self 下拉菜单实例或指针
@return self
@usage dd:clear_options()
*/
static int iot_lvgl_dropdown_clear_options(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    lv_dropdown_clear_options(dd);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置选中项
@param self 下拉菜单实例或指针
@param sel_opt 选中项索引
@return self
@usage dd:set_selected(1)
*/
static int iot_lvgl_dropdown_set_selected(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    uint16_t sel_opt = (uint16_t)luaL_checkinteger(L, 2);
    lv_dropdown_set_selected(dd, sel_opt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置选中项高亮
@param self 下拉菜单实例或指针
@param en 是否高亮
@return self
@usage dd:set_selected_highlight(true)
*/
static int iot_lvgl_dropdown_set_selected_highlight(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_dropdown_set_selected_highlight(dd, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置显示文本
@param self 下拉菜单实例或指针
@param txt 文本内容
@return self
@usage dd:set_text("请选择")
*/
static int iot_lvgl_dropdown_set_text(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    const char* txt = luaL_checkstring(L, 2);
    lv_dropdown_set_text(dd, txt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置下拉方向
@param self 下拉菜单实例或指针
@param dir 方向(lvgl.DIR_BOTTOM等)
@return self
@usage dd:set_direction(lvgl.DIR_BOTTOM)
*/
static int iot_lvgl_dropdown_set_direction(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    lv_dir_t dir = (lv_dir_t)luaL_checkinteger(L, 2);
    lv_dropdown_set_dir(dd, dir);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取选中项索引
@param self 下拉菜单实例或指针
@return integer 选中项索引
@usage local sel = dd:get_selected()
*/
static int iot_lvgl_dropdown_get_selected(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    uint16_t sel = lv_dropdown_get_selected(dd);
    lua_pushinteger(L, sel);
    return 1;
}

/*
获取选中项文本
@param self 下拉菜单实例或指针
@return string 选中项文本
@usage local sel_str = dd:get_selected_str()
*/
static int iot_lvgl_dropdown_get_selected_str(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    char buf[256];
    lv_dropdown_get_selected_str(dd, buf, sizeof(buf));
    lua_pushstring(L, buf);
    return 1;
}

/*
获取显示文本
@param self 下拉菜单实例或指针
@return string 显示文本
@usage local txt = dd:get_text()
*/
static int iot_lvgl_dropdown_get_text(lua_State* L) {
    lv_obj_t* dd = iot_lvgl_get_obj_ptr(L, 1);
    const char* txt = lv_dropdown_get_text(dd);
    lua_pushstring(L, txt);
    return 1;
}

/* 注册 dropdown 子模块 */
void iot_lvgl_register_dropdown(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_options", iot_lvgl_dropdown_set_options);
    REG_METHOD(L, "set_options_static", iot_lvgl_dropdown_set_options_static);
    REG_METHOD(L, "add_option", iot_lvgl_dropdown_add_option);
    REG_METHOD(L, "clear_options", iot_lvgl_dropdown_clear_options);
    REG_METHOD(L, "set_selected", iot_lvgl_dropdown_set_selected);
    REG_METHOD(L, "set_selected_highlight", iot_lvgl_dropdown_set_selected_highlight);
    REG_METHOD(L, "set_text", iot_lvgl_dropdown_set_text);
    REG_METHOD(L, "set_direction", iot_lvgl_dropdown_set_direction);
    REG_METHOD(L, "get_selected", iot_lvgl_dropdown_get_selected);
    REG_METHOD(L, "get_selected_str", iot_lvgl_dropdown_get_selected_str);
    REG_METHOD(L, "get_text", iot_lvgl_dropdown_get_text);

    /* 保存组件metatable引用(用于继承) */
    dropdown_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.dropdown.set_options(dd, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, dropdown_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.dropdown) */
    REG_METHOD(L, "create", iot_lvgl_dropdown_create);
}
