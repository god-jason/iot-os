/**
 * @file iot_lvgl_span.c
 * @brief LVGL富文本Span控件
 *
 * 实现LVGL富文本Span控件的OO风格Lua绑定，包括SpanGroup创建、添加/删除Span、设置Span文本/样式、设置对齐/溢出/缩进/最大行数等接口，支持链式调用。
 * 注意：lv_span_t 是结构体而非 lv_obj_t，在Lua中以lightuserdata传递。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* span组件的metatable引用 */
static int span_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_span_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* spangroup = lv_spangroup_create(parent);
    lua_pushlightuserdata(L, spangroup);
    return 1;
}

/* ==================== SpanGroup OO方法 ==================== */

/*
创建SpanGroup控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的SpanGroup实例
@usage local span = lvgl.span.create(scr)
*/
static int iot_lvgl_span_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_span_create_internal, span_metatable_ref);
}

/*
添加Span到SpanGroup
@param self SpanGroup实例或指针
@return userdata span指针(lightuserdata)
@usage local sp = span:add_span()
*/
static int iot_lvgl_span_add_span(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_t* span = lv_spangroup_add_span(obj);
    lua_pushlightuserdata(L, span);
    return 1;
}

/*
设置Span文本
@param self SpanGroup实例或指针
@param span span指针
@param text 文本内容
@return self
@usage span:set_span_text(sp, "Hello")
*/
static int iot_lvgl_span_set_span_text(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_t* span = (lv_span_t*)luaL_checklightuserdata(L, 2);
    const char* text = luaL_checkstring(L, 3);
    lv_spangroup_set_span_text(obj, span, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置Span文本(静态文本，不拷贝)
@param self SpanGroup实例或指针
@param span span指针
@param text 静态文本内容
@return self
@usage span:set_span_text_static(sp, "Hello")
*/
static int iot_lvgl_span_set_span_text_static(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_t* span = (lv_span_t*)luaL_checklightuserdata(L, 2);
    const char* text = luaL_checkstring(L, 3);
    lv_spangroup_set_span_text_static(obj, span, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置Span样式(拷贝样式属性到Span的内置样式)
@param self SpanGroup实例或指针
@param span span指针
@param style 样式指针
@return self
@usage span:set_span_style(sp, style)
*/
static int iot_lvgl_span_set_span_style(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_t* span = (lv_span_t*)luaL_checklightuserdata(L, 2);
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 3);
    lv_spangroup_set_span_style(obj, span, style);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置SpanGroup对齐方式
@param self SpanGroup实例或指针
@param align 对齐方式 (lv_text_align_t)
@return self
@usage span:set_align(lvgl.TEXT_ALIGN_CENTER)
*/
static int iot_lvgl_span_set_align(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_text_align_t align = (lv_text_align_t)luaL_checkinteger(L, 2);
    lv_spangroup_set_align(obj, align);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置SpanGroup溢出模式
@param self SpanGroup实例或指针
@param overflow 溢出模式 (lv_span_overflow_t)
@return self
@usage span:set_overflow(lvgl.SPAN_OVERFLOW_ELLIPSIS)
*/
static int iot_lvgl_span_set_overflow(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_overflow_t overflow = (lv_span_overflow_t)luaL_checkinteger(L, 2);
    lv_spangroup_set_overflow(obj, overflow);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置SpanGroup首行缩进
@param self SpanGroup实例或指针
@param indent 缩进值(像素)
@return self
@usage span:set_indent(20)
*/
static int iot_lvgl_span_set_indent(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    int32_t indent = (int32_t)luaL_checkinteger(L, 2);
    lv_spangroup_set_indent(obj, indent);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置SpanGroup最大行数
@param self SpanGroup实例或指针
@param lines 最大行数(小于0表示无限制)
@return self
@usage span:set_max_lines(3)
*/
static int iot_lvgl_span_set_max_lines(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    int32_t lines = (int32_t)luaL_checkinteger(L, 2);
    lv_spangroup_set_max_lines(obj, lines);
    lua_pushvalue(L, 1);
    return 1;
}

/*
删除指定的Span
@param self SpanGroup实例或指针
@param span span指针
@return self
@usage span:delete_span(sp)
*/
static int iot_lvgl_span_delete_span(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_t* span = (lv_span_t*)luaL_checklightuserdata(L, 2);
    lv_spangroup_delete_span(obj, span);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取Span的文本
@param self SpanGroup实例或指针
@param span span指针
@return string 文本内容
@usage local text = span:get_span_text(sp)
*/
static int iot_lvgl_span_get_span_text(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_t* span = (lv_span_t*)luaL_checklightuserdata(L, 2);
    (void)obj;
    const char* text = lv_span_get_text(span);
    lua_pushstring(L, text ? text : "");
    return 1;
}

/*
获取Span的内置样式指针
@param self SpanGroup实例或指针
@param span span指针
@return userdata 样式指针
@usage local style = span:get_span_style(sp)
*/
static int iot_lvgl_span_get_span_style(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_t* span = (lv_span_t*)luaL_checklightuserdata(L, 2);
    (void)obj;
    lv_style_t* style = lv_span_get_style(span);
    lua_pushlightuserdata(L, style);
    return 1;
}

/*
按索引获取SpanGroup中的Span
@param self SpanGroup实例或指针
@param id span索引(0=第一个, -1=最后一个)
@return userdata span指针, 未找到返回nil
@usage local sp = span:get_child(0)
*/
static int iot_lvgl_span_get_child(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    int32_t id = (int32_t)luaL_checkinteger(L, 2);
    lv_span_t* span = lv_spangroup_get_child(obj, id);
    if (span) {
        lua_pushlightuserdata(L, span);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/*
获取SpanGroup中的Span数量
@param self SpanGroup实例或指针
@return integer Span数量
@usage local count = span:get_span_count()
*/
static int iot_lvgl_span_get_span_count(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t count = lv_spangroup_get_span_count(obj);
    lua_pushinteger(L, count);
    return 1;
}

/*
获取SpanGroup对齐方式
@param self SpanGroup实例或指针
@return integer 对齐方式
@usage local align = span:get_align()
*/
static int iot_lvgl_span_get_align(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_text_align_t align = lv_spangroup_get_align(obj);
    lua_pushinteger(L, align);
    return 1;
}

/*
获取SpanGroup溢出模式
@param self SpanGroup实例或指针
@return integer 溢出模式
@usage local overflow = span:get_overflow()
*/
static int iot_lvgl_span_get_overflow(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_overflow_t overflow = lv_spangroup_get_overflow(obj);
    lua_pushinteger(L, overflow);
    return 1;
}

/*
获取SpanGroup首行缩进
@param self SpanGroup实例或指针
@return integer 缩进值
@usage local indent = span:get_indent()
*/
static int iot_lvgl_span_get_indent(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    int32_t indent = lv_spangroup_get_indent(obj);
    lua_pushinteger(L, indent);
    return 1;
}

/*
获取SpanGroup最大行数
@param self SpanGroup实例或指针
@return integer 最大行数
@usage local lines = span:get_max_lines()
*/
static int iot_lvgl_span_get_max_lines(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    int32_t lines = lv_spangroup_get_max_lines(obj);
    lua_pushinteger(L, lines);
    return 1;
}

/*
刷新SpanGroup(更新布局)
@param self SpanGroup实例或指针
@return self
@usage span:refresh()
*/
static int iot_lvgl_span_refresh(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_spangroup_refresh(obj);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置Span文本(格式化字符串)
@param self SpanGroup实例或指针
@param span span指针
@param fmt 格式字符串
@return self
@usage span:set_span_text_fmt(sp, "value=%d", 123)
*/
static int iot_lvgl_span_set_span_text_fmt(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_t* span = (lv_span_t*)luaL_checklightuserdata(L, 2);
    const char* fmt = luaL_checkstring(L, 3);
    lv_spangroup_set_span_text_fmt(obj, span, "%s", fmt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置SpanGroup模式
@param self SpanGroup实例或指针
@param mode 模式 (lv_span_mode_t)
@return self
@usage span:set_mode(lvgl.SPAN_MODE_FIXED)
*/
static int iot_lvgl_span_set_mode(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_mode_t mode = (lv_span_mode_t)luaL_checkinteger(L, 2);
    lv_spangroup_set_mode(obj, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取SpanGroup模式
@param self SpanGroup实例或指针
@return integer 模式
@usage local mode = span:get_mode()
*/
static int iot_lvgl_span_get_mode(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_span_mode_t mode = lv_spangroup_get_mode(obj);
    lua_pushinteger(L, mode);
    return 1;
}

/*
获取最大行高
@param self SpanGroup实例或指针
@return integer 最大行高
@usage local height = span:get_max_line_height()
*/
static int iot_lvgl_span_get_max_line_height(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    int32_t height = lv_spangroup_get_max_line_height(obj);
    lua_pushinteger(L, height);
    return 1;
}

/*
获取展开宽度
@param self SpanGroup实例或指针
@param max_width 最大宽度(可选,默认0)
@return integer 展开宽度
@usage local width = span:get_expand_width()
*/
static int iot_lvgl_span_get_expand_width(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t max_width = (uint32_t)luaL_optinteger(L, 2, 0);
    uint32_t width = lv_spangroup_get_expand_width(obj, max_width);
    lua_pushinteger(L, width);
    return 1;
}

/*
获取展开高度
@param self SpanGroup实例或指针
@param width 宽度
@return integer 展开高度
@usage local height = span:get_expand_height(200)
*/
static int iot_lvgl_span_get_expand_height(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    int32_t width = (int32_t)luaL_checkinteger(L, 2);
    int32_t height = lv_spangroup_get_expand_height(obj, width);
    lua_pushinteger(L, height);
    return 1;
}

/* 注册 span 子模块 */
void iot_lvgl_register_span(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法(可以 instance:method() 调用) */
    REG_METHOD(L, "add_span", iot_lvgl_span_add_span);
    REG_METHOD(L, "set_span_text", iot_lvgl_span_set_span_text);
    REG_METHOD(L, "set_span_text_static", iot_lvgl_span_set_span_text_static);
    REG_METHOD(L, "set_span_style", iot_lvgl_span_set_span_style);
    REG_METHOD(L, "set_align", iot_lvgl_span_set_align);
    REG_METHOD(L, "set_overflow", iot_lvgl_span_set_overflow);
    REG_METHOD(L, "set_indent", iot_lvgl_span_set_indent);
    REG_METHOD(L, "set_max_lines", iot_lvgl_span_set_max_lines);
    REG_METHOD(L, "delete_span", iot_lvgl_span_delete_span);
    REG_METHOD(L, "get_span_text", iot_lvgl_span_get_span_text);
    REG_METHOD(L, "get_span_style", iot_lvgl_span_get_span_style);
    REG_METHOD(L, "get_child", iot_lvgl_span_get_child);
    REG_METHOD(L, "get_span_count", iot_lvgl_span_get_span_count);
    REG_METHOD(L, "get_align", iot_lvgl_span_get_align);
    REG_METHOD(L, "get_overflow", iot_lvgl_span_get_overflow);
    REG_METHOD(L, "get_indent", iot_lvgl_span_get_indent);
    REG_METHOD(L, "get_max_lines", iot_lvgl_span_get_max_lines);
    REG_METHOD(L, "set_span_text_fmt", iot_lvgl_span_set_span_text_fmt);
    REG_METHOD(L, "set_mode", iot_lvgl_span_set_mode);
    REG_METHOD(L, "get_mode", iot_lvgl_span_get_mode);
    REG_METHOD(L, "get_max_line_height", iot_lvgl_span_get_max_line_height);
    REG_METHOD(L, "get_expand_width", iot_lvgl_span_get_expand_width);
    REG_METHOD(L, "get_expand_height", iot_lvgl_span_get_expand_height);
    REG_METHOD(L, "refresh", iot_lvgl_span_refresh);

    /* 保存组件metatable引用(用于继承) */
    span_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.span.add_span(span, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, span_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.span) */
    REG_METHOD(L, "create", iot_lvgl_span_create);
}