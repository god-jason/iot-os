/*
@module  lvgl.label
@summary LVGL标签控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建标签
local label = lvgl.label.create(scr)

-- 使用OO风格
label:set_pos(10, 10)
label:set_text("Hello World")
label:set_align(lvgl.TEXT_ALIGN_CENTER)
label:set_size(200, 30)

-- 长文本模式
label:set_long_mode(lvgl.LABEL_LONG_SCROLL_CIRCULAR)
label:set_text_sel_start(0)
label:set_text_sel_end(5)
label:set_recolor(true)

-- 链式调用
local label2 = lvgl.label.create(scr):set_pos(10, 50):set_text("Another Label"):set_align(lvgl.TEXT_ALIGN_LEFT)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* label组件的metatable引用 */
static int label_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_label_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* label = lv_label_create(parent);
    lua_pushlightuserdata(L, label);
    return 1;
}

/* ==================== 标签OO方法 ==================== */

/*
创建标签(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的标签实例
@usage local label = lvgl.label.create(scr)
*/
static int lvgl_label_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_label_create_internal, label_metatable_ref);
}

/*
设置标签文本
@param self 标签实例或指针
@param text 文本内容
@return self
@usage label:set_text("Hello")
*/
static int lvgl_label_set_text(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
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
static int lvgl_label_get_text(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
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
static int lvgl_label_set_align(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    lv_text_align_t align = (lv_text_align_t)luaL_checkinteger(L, 2);
    lv_label_set_align(label, align);
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
static int lvgl_label_set_long_mode(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
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
static int lvgl_label_set_recolor(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
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
static int lvgl_label_set_text_sel_start(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
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
static int lvgl_label_set_text_sel_end(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
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
static int lvgl_label_is_char_under_cursor(lua_State* L) {
    lv_obj_t* label = lvgl_get_obj_ptr(L, 1);
    bool under = lv_label_is_char_under_cursor(label);
    lua_pushboolean(L, under);
    return 1;
}

/* 注册 label 子模块 */
void lvgl_register_label(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_text", lvgl_label_set_text);
    REG_METHOD(L, "get_text", lvgl_label_get_text);
    REG_METHOD(L, "set_align", lvgl_label_set_align);
    REG_METHOD(L, "set_long_mode", lvgl_label_set_long_mode);
    REG_METHOD(L, "set_recolor", lvgl_label_set_recolor);
    REG_METHOD(L, "set_text_sel_start", lvgl_label_set_text_sel_start);
    REG_METHOD(L, "set_text_sel_end", lvgl_label_set_text_sel_end);
    REG_METHOD(L, "is_char_under_cursor", lvgl_label_is_char_under_cursor);

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

    /* 注册create函数到主表(lvgl.label) */
    REG_METHOD(L, "create", lvgl_label_create);
}
