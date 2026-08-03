/**
 * @file iot_lvgl_label.c
 * @brief LVGL标签控件
 *
 * 实现LVGL标签控件的OO风格Lua绑定，包括标签创建、设置/获取文本、设置文本对齐、设置自动换行模式、设置长文本模式（滚动/省略/裁剪）、设置文本选择范围等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* label组件的metatable引用 */
static int label_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_label_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* label = lv_label_create(parent);
    lua_pushlightuserdata(L, label);
    return 1;
}

/* ==================== ??OO?? ==================== */

/*
创建标签(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的标签实例
@usage local label = lvgl.label.create(scr)
*/
static int iot_lvgl_label_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_label_create_internal, label_metatable_ref);
}

/*
设置标签文本
@param self 标签实例或指针
@param text 文本内容
@return self
@usage label:set_text("Hello")
*/
static int iot_lvgl_label_set_text(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_label_set_text(label, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取标签文本
@param self 标签实例或指针
@return string 文本内容
@usage local text = label:get_text()
*/
static int iot_lvgl_label_get_text(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = lv_label_get_text(label);
    lua_pushstring(L, text ? text : "");
    return 1;
}

/*
设置文本对齐方式
@param self 标签实例或指针
@param align 对齐方式
@return self
@usage label:set_align(lvgl.TEXT_ALIGN_CENTER)
*/
static int iot_lvgl_label_set_align(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    lv_text_align_t align = (lv_text_align_t)luaL_checkinteger(L, 2);
    lv_obj_set_style_text_align(label, align, LV_PART_MAIN);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置长文本模式
@param self 标签实例或指针
@param mode 模式: LABEL_LONG_WRAP, LABEL_LONG_SCROLL, LABEL_LONG_DOT, LABEL_LONG_SCROLL_CIRCULAR, LABEL_LONG_CROP
@return self
@usage label:set_long_mode(lvgl.LABEL_LONG_SCROLL_CIRCULAR)
*/
static int iot_lvgl_label_set_long_mode(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t mode = (uint32_t)luaL_checkinteger(L, 2);
    lv_label_set_long_mode(label, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置文本重新着色
@param self 标签实例或指针
@param en 是否启用
@return self
@usage label:set_recolor(true)
*/
static int iot_lvgl_label_set_recolor(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_label_set_recolor(label, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置文本选中开始位置
@param self 标签实例或指针
@param pos 位置
@return self
@usage label:set_text_sel_start(0)
*/
static int iot_lvgl_label_set_text_sel_start(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    int32_t pos = (int32_t)luaL_checkinteger(L, 2);
    lv_label_set_text_sel_start(label, pos);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置文本选中结束位置
@param self 标签实例或指针
@param pos 位置
@return self
@usage label:set_text_sel_end(5)
*/
static int iot_lvgl_label_set_text_sel_end(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    int32_t pos = (int32_t)luaL_checkinteger(L, 2);
    lv_label_set_text_sel_end(label, pos);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取文本是否在光标下
@param self 标签实例或指针
@return boolean
@usage local under = label:is_char_under_cursor()
*/
static int iot_lvgl_label_is_char_under_cursor(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    lv_indev_t* indev = lv_indev_get_act();
    lv_point_t rel_pos = {0, 0};
    bool under = false;

    if (indev) {
        lv_point_t point;
        lv_area_t coords;
        lv_indev_get_point(indev, &point);
        lv_obj_get_coords(label, &coords);
        rel_pos.x = point.x - coords.x1;
        rel_pos.y = point.y - coords.y1;
        under = lv_label_is_char_under_pos(label, &rel_pos);
    }

    lua_pushboolean(L, under);
    return 1;
}

/*
设置格式化文本
@param self 标签实例或指针
@param fmt 格式化文本
@return self
@usage label:set_text_fmt("Value: %d", 42)
*/
static int iot_lvgl_label_set_text_fmt(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    const char* fmt = luaL_checkstring(L, 2);
    lv_label_set_text(label, fmt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
插入文本
@param self 标签实例或指针
@param pos 字符索引位置(0: 第一个字符前, LV_LABEL_POS_LAST: 最后一个字符后)
@param txt 要插入的文本
@return self
@usage label:ins_text(0, "prefix")
*/
static int iot_lvgl_label_ins_text(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t pos = (uint32_t)luaL_checkinteger(L, 2);
    const char* txt = luaL_checkstring(L, 3);
    lv_label_ins_text(label, pos, txt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
剪切文本
@param self 标签实例或指针
@param pos 开始剪切的字符索引
@param cnt 要剪切的字符数
@return self
@usage label:cut_text(0, 5)
*/
static int iot_lvgl_label_cut_text(lua_State* L) {
    lv_obj_t* label = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t pos = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t cnt = (uint32_t)luaL_checkinteger(L, 3);
    lv_label_cut_text(label, pos, cnt);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 label 子模块 */
void iot_lvgl_register_label(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_text", iot_lvgl_label_set_text);
    REG_METHOD(L, "get_text", iot_lvgl_label_get_text);
    REG_METHOD(L, "set_align", iot_lvgl_label_set_align);
    REG_METHOD(L, "set_long_mode", iot_lvgl_label_set_long_mode);
    REG_METHOD(L, "set_recolor", iot_lvgl_label_set_recolor);
    REG_METHOD(L, "set_text_sel_start", iot_lvgl_label_set_text_sel_start);
    REG_METHOD(L, "set_text_sel_end", iot_lvgl_label_set_text_sel_end);
    REG_METHOD(L, "is_char_under_cursor", iot_lvgl_label_is_char_under_cursor);
    REG_METHOD(L, "set_text_fmt", iot_lvgl_label_set_text_fmt);
    REG_METHOD(L, "ins_text", iot_lvgl_label_ins_text);
    REG_METHOD(L, "cut_text", iot_lvgl_label_cut_text);

    /* 保存组件metatable引用(用于继承) */
    label_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.label.set_text(label, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, label_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表lvgl.label) */
    REG_METHOD(L, "create", iot_lvgl_label_create);
}
