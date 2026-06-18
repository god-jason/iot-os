/*
@module  lvgl.textarea
@summary LVGL文本区域控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")
local scr = lvgl.scr_act()

-- 创建文本区域
local ta = lvgl.textarea.create(scr)
ta:set_size(200, 100)
ta:set_pos(50, 50)

-- 设置文本内容
ta:set_text("Hello World!")

-- 获取文本内容
local text = ta:get_text()

-- 设置占位符
ta:set_placeholder_text("请输入...")

-- 设置最大长度
ta:set_max_length(100)

-- 单行模式
ta:set_one_line(false)

-- 密码模式
ta:set_password_mode(false)

-- 添加字符和文本
ta:add_char(string.byte("A"))
ta:add_text(" appended")

-- 删除字符
ta:del_char()
ta:del_char_forward()

-- 清空文本
ta:clear()

-- 链式调用
local ta2 = lvgl.textarea.create(scr):set_size(200, 50):set_pos(50, 170):set_text("Input"):set_max_length(50)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* textarea组件的metatable引用 */
static int textarea_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_textarea_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* ta = lv_textarea_create(parent);
    lua_pushlightuserdata(L, ta);
    return 1;
}

/* ==================== 文本区域OO方法 ==================== */

/*
创建文本区域控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的文本区域实例
@usage local ta = lvgl.textarea.create(scr)
*/
static int lvgl_textarea_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_textarea_create_internal, textarea_metatable_ref);
}

/*
设置文本内容
@param self 文本区域实例或指针
@param txt 文本内容
@return self
@usage ta:set_text("Hello")
*/
static int lvgl_textarea_set_text(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    const char* txt = luaL_checkstring(L, 2);
    lv_textarea_set_text(ta, txt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取文本内容
@param self 文本区域实例或指针
@return string 文本内容
@usage local text = ta:get_text()
*/
static int lvgl_textarea_get_text(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    const char* txt = lv_textarea_get_text(ta);
    lua_pushstring(L, txt ? txt : "");
    return 1;
}

/*
设置占位符文本
@param self 文本区域实例或指针
@param txt 占位符文本
@return self
@usage ta:set_placeholder_text("请输入...")
*/
static int lvgl_textarea_set_placeholder_text(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    const char* txt = luaL_checkstring(L, 2);
    lv_textarea_set_placeholder_text(ta, txt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置最大长度
@param self 文本区域实例或指针
@param max 最大长度
@return self
@usage ta:set_max_length(100)
*/
static int lvgl_textarea_set_max_length(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    uint32_t max = (uint32_t)luaL_checkinteger(L, 2);
    lv_textarea_set_max_length(ta, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置单行模式
@param self 文本区域实例或指针
@param en 是否单行
@return self
@usage ta:set_one_line(false)
*/
static int lvgl_textarea_set_one_line(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_textarea_set_one_line(ta, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置密码模式
@param self 文本区域实例或指针
@param en 是否密码模式
@return self
@usage ta:set_password_mode(false)
*/
static int lvgl_textarea_set_password_mode(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_textarea_set_password_mode(ta, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置密码显示时间
@param self 文本区域实例或指针
@param time 显示时间(ms)
@return self
@usage ta:set_password_show_time(1000)
*/
static int lvgl_textarea_set_password_show_time(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    uint16_t time = (uint16_t)luaL_checkinteger(L, 2);
    lv_textarea_set_password_show_time(ta, time);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置允许输入的字符
@param self 文本区域实例或指针
@param list 允许的字符列表
@return self
@usage ta:set_accepted_chars("0123456789")
*/
static int lvgl_textarea_set_accepted_chars(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    const char* list = luaL_checkstring(L, 2);
    lv_textarea_set_accepted_chars(ta, list);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置文本重新着色
@param self 文本区域实例或指针
@param en 是否启用
@return self
@usage ta:set_recolor(true)
*/
static int lvgl_textarea_set_recolor(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_textarea_set_recolor(ta, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置光标位置
@param self 文本区域实例或指针
@param pos 位置
@return self
@usage ta:set_cursor_pos(5)
*/
static int lvgl_textarea_set_cursor_pos(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    int32_t pos = (int32_t)luaL_checkinteger(L, 2);
    lv_textarea_set_cursor_pos(ta, pos);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置点击时移动光标
@param self 文本区域实例或指针
@param en 是否启用
@return self
@usage ta:set_cursor_click_pos(true)
*/
static int lvgl_textarea_set_cursor_click_pos(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_textarea_set_cursor_click_pos(ta, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
添加字符
@param self 文本区域实例或指针
@param c 字符编码
@return self
@usage ta:add_char(string.byte("A"))
*/
static int lvgl_textarea_add_char(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    uint32_t c = (uint32_t)luaL_checkinteger(L, 2);
    lv_textarea_add_char(ta, c);
    lua_pushvalue(L, 1);
    return 1;
}

/*
添加文本
@param self 文本区域实例或指针
@param txt 文本内容
@return self
@usage ta:add_text(" appended")
*/
static int lvgl_textarea_add_text(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    const char* txt = luaL_checkstring(L, 2);
    lv_textarea_add_text(ta, txt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
删除光标前字符
@param self 文本区域实例或指针
@return self
@usage ta:del_char()
*/
static int lvgl_textarea_del_char(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    lv_textarea_del_char(ta);
    lua_pushvalue(L, 1);
    return 1;
}

/*
删除光标后字符
@param self 文本区域实例或指针
@return self
@usage ta:del_char_forward()
*/
static int lvgl_textarea_del_char_forward(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    lv_textarea_del_char_forward(ta);
    lua_pushvalue(L, 1);
    return 1;
}

/*
清空文本
@param self 文本区域实例或指针
@return self
@usage ta:clear()
*/
static int lvgl_textarea_clear(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    lv_textarea_clear(ta);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取光标位置
@param self 文本区域实例或指针
@return integer 光标位置
@usage local pos = ta:get_cursor_pos()
*/
static int lvgl_textarea_get_cursor_pos(lua_State* L) {
    lv_obj_t* ta = lvgl_get_obj_ptr(L, 1);
    int32_t pos = lv_textarea_get_cursor_pos(ta);
    lua_pushinteger(L, pos);
    return 1;
}

/* 注册 textarea 子模块 */
void lvgl_register_textarea(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_text", lvgl_textarea_set_text);
    REG_METHOD(L, "get_text", lvgl_textarea_get_text);
    REG_METHOD(L, "set_placeholder_text", lvgl_textarea_set_placeholder_text);
    REG_METHOD(L, "set_max_length", lvgl_textarea_set_max_length);
    REG_METHOD(L, "set_one_line", lvgl_textarea_set_one_line);
    REG_METHOD(L, "set_password_mode", lvgl_textarea_set_password_mode);
    REG_METHOD(L, "set_password_show_time", lvgl_textarea_set_password_show_time);
    REG_METHOD(L, "set_accepted_chars", lvgl_textarea_set_accepted_chars);
    REG_METHOD(L, "set_recolor", lvgl_textarea_set_recolor);
    REG_METHOD(L, "set_cursor_pos", lvgl_textarea_set_cursor_pos);
    REG_METHOD(L, "set_cursor_click_pos", lvgl_textarea_set_cursor_click_pos);
    REG_METHOD(L, "add_char", lvgl_textarea_add_char);
    REG_METHOD(L, "add_text", lvgl_textarea_add_text);
    REG_METHOD(L, "del_char", lvgl_textarea_del_char);
    REG_METHOD(L, "del_char_forward", lvgl_textarea_del_char_forward);
    REG_METHOD(L, "clear", lvgl_textarea_clear);
    REG_METHOD(L, "get_cursor_pos", lvgl_textarea_get_cursor_pos);

    /* 保存组件metatable引用(用于继承) */
    textarea_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.textarea.set_text(ta, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, textarea_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.textarea) */
    REG_METHOD(L, "create", lvgl_textarea_create);
}
