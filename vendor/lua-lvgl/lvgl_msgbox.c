/*
@module  lvgl.msgbox
@summary LVGL消息框控件
@version 2.0
@date    2026.06.18
@author  杰神 & TRAE & ChatGPT
@tag     Graphics
@usage
-- Lua示例(OO风格)
local lvgl = require("lvgl")

-- 创建消息框
local msgbox = lvgl.msgbox.create(nil)
msgbox:set_text("操作成功!")
msgbox:add_button("确定", 0)
msgbox:add_button("取消", 1)

-- 设置宽高
msgbox:set_size(200, 100)

-- 获取按钮索引
local btn_index = msgbox:get_active_btn()
local btn_text = msgbox:get_active_btn_text()

-- 关闭消息框
msgbox:close()

-- 链式调用
local box = lvgl.msgbox.create(nil):set_text("Hello"):add_button("OK", 0):set_size(180, 80)
*/

#include "lvgl.h"
#include "lvgl_obj.h"

/* msgbox组件的metatable引用 */
static int msgbox_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int lvgl_msgbox_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* msgbox = lv_msgbox_create(parent);
    lua_pushlightuserdata(L, msgbox);
    return 1;
}

/* ==================== 消息框OO方法 ==================== */

/*
创建消息框控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的消息框实例
@usage local msgbox = lvgl.msgbox.create(nil)
*/
static int lvgl_msgbox_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_msgbox_create_internal, msgbox_metatable_ref);
}

/*
设置消息框标题
@param self 消息框实例或指针
@param title 标题文本
@return self
@usage msgbox:set_title("提示")
*/
static int lvgl_msgbox_set_title(lua_State* L) {
    lv_obj_t* msgbox = lvgl_get_obj_ptr(L, 1);
    const char* title = luaL_checkstring(L, 2);
    lv_msgbox_set_title(msgbox, title);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置消息框文本
@param self 消息框实例或指针
@param text 文本内容
@return self
@usage msgbox:set_text("操作成功")
*/
static int lvgl_msgbox_set_text(lua_State* L) {
    lv_obj_t* msgbox = lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_msgbox_set_text(msgbox, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
添加按钮
@param self 消息框实例或指针
@param txt 按钮文本
@return self
@usage msgbox:add_button("确定")
*/
static int lvgl_msgbox_add_button(lua_State* L) {
    lv_obj_t* msgbox = lvgl_get_obj_ptr(L, 1);
    const char* txt = luaL_checkstring(L, 2);
    lv_msgbox_add_button(msgbox, txt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取激活的按钮索引
@param self 消息框实例或指针
@return integer 按钮索引
@usage local btn_index = msgbox:get_active_btn()
*/
static int lvgl_msgbox_get_active_btn(lua_State* L) {
    lv_obj_t* msgbox = lvgl_get_obj_ptr(L, 1);
    int16_t btn = lv_msgbox_get_active_btn(msgbox);
    lua_pushinteger(L, btn);
    return 1;
}

/*
获取激活的按钮文本
@param self 消息框实例或指针
@return string 按钮文本
@usage local btn_text = msgbox:get_active_btn_text()
*/
static int lvgl_msgbox_get_active_btn_text(lua_State* L) {
    lv_obj_t* msgbox = lvgl_get_obj_ptr(L, 1);
    const char* txt = lv_msgbox_get_active_btn_text(msgbox);
    lua_pushstring(L, txt);
    return 1;
}

/*
关闭消息框
@param self 消息框实例或指针
@return self
@usage msgbox:close()
*/
static int lvgl_msgbox_close(lua_State* L) {
    lv_obj_t* msgbox = lvgl_get_obj_ptr(L, 1);
    lv_msgbox_close(msgbox);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 msgbox 子模块 */
void lvgl_register_msgbox(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_title", lvgl_msgbox_set_title);
    REG_METHOD(L, "set_text", lvgl_msgbox_set_text);
    REG_METHOD(L, "add_button", lvgl_msgbox_add_button);
    REG_METHOD(L, "get_active_btn", lvgl_msgbox_get_active_btn);
    REG_METHOD(L, "get_active_btn_text", lvgl_msgbox_get_active_btn_text);
    REG_METHOD(L, "close", lvgl_msgbox_close);

    /* 保存组件metatable引用(用于继承) */
    msgbox_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.msgbox.set_title(mb, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, msgbox_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.msgbox) */
    REG_METHOD(L, "create", lvgl_msgbox_create);
}
