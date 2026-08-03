/**
 * @file iot_lvgl_ime_pinyin.c
 * @brief LVGL拼音输入法组件
 *
 * 实现LVGL拼音输入法(IME Pinyin)控件的OO风格Lua绑定，包括创建、设置键盘关联、
 * 设置字典、设置模式（26键/9键/9键数字）、获取键盘对象、候选面板和字典等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* ime_pinyin组件的metatable引用 */
static int ime_pinyin_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_ime_pinyin_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* ime = lv_ime_pinyin_create(parent);
    lua_pushlightuserdata(L, ime);
    return 1;
}

/* ==================== IME Pinyin OO方法 ==================== */

/*
创建拼音输入法控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的拼音输入法实例
@usage local ime = lvgl.ime_pinyin.create(scr)
*/
static int iot_lvgl_ime_pinyin_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_ime_pinyin_create_internal, ime_pinyin_metatable_ref);
}

/*
设置关联的键盘
@param self 拼音输入法实例或指针
@param kb 键盘对象
@return self
@usage ime:set_keyboard(kb)
*/
static int iot_lvgl_ime_pinyin_set_keyboard(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* kb = (lv_obj_t*)luaL_checklightuserdata(L, 2);
    lv_ime_pinyin_set_keyboard(obj, kb);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置字典
@param self 拼音输入法实例或指针
@param dict 字典指针(lightuserdata, lv_pinyin_dict_t*)
@return self
@usage ime:set_dict(dict_ptr)
*/
static int iot_lvgl_ime_pinyin_set_dict(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_pinyin_dict_t* dict = (lv_pinyin_dict_t*)luaL_checklightuserdata(L, 2);
    lv_ime_pinyin_set_dict(obj, dict);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置模式
@param self 拼音输入法实例或指针
@param mode 模式(0=LV_IME_PINYIN_MODE_K26, 1=LV_IME_PINYIN_MODE_K9, 2=LV_IME_PINYIN_MODE_K9_NUMBER)
@return self
@usage ime:set_mode(0)  -- 26键模式
*/
static int iot_lvgl_ime_pinyin_set_mode(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_ime_pinyin_mode_t mode = (lv_ime_pinyin_mode_t)luaL_checkinteger(L, 2);
    lv_ime_pinyin_set_mode(obj, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取关联的键盘
@param self 拼音输入法实例或指针
@return userdata 键盘对象
@usage local kb = ime:get_kb()
*/
static int iot_lvgl_ime_pinyin_get_kb(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* kb = lv_ime_pinyin_get_kb(obj);
    lua_pushlightuserdata(L, kb);
    return 1;
}

/*
获取候选面板
@param self 拼音输入法实例或指针
@return userdata 候选面板对象
@usage local panel = ime:get_cand_panel()
*/
static int iot_lvgl_ime_pinyin_get_cand_panel(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* panel = lv_ime_pinyin_get_cand_panel(obj);
    lua_pushlightuserdata(L, panel);
    return 1;
}

/*
获取字典
@param self 拼音输入法实例或指针
@return userdata 字典指针
@usage local dict = ime:get_dict()
*/
static int iot_lvgl_ime_pinyin_get_dict(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    const lv_pinyin_dict_t* dict = lv_ime_pinyin_get_dict(obj);
    lua_pushlightuserdata(L, (void*)dict);
    return 1;
}

/* 注册 ime_pinyin 子模块 */
void iot_lvgl_register_ime_pinyin(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_keyboard", iot_lvgl_ime_pinyin_set_keyboard);
    REG_METHOD(L, "set_dict", iot_lvgl_ime_pinyin_set_dict);
    REG_METHOD(L, "set_mode", iot_lvgl_ime_pinyin_set_mode);
    REG_METHOD(L, "get_kb", iot_lvgl_ime_pinyin_get_kb);
    REG_METHOD(L, "get_cand_panel", iot_lvgl_ime_pinyin_get_cand_panel);
    REG_METHOD(L, "get_dict", iot_lvgl_ime_pinyin_get_dict);

    /* 保存组件metatable引用(用于继承) */
    ime_pinyin_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.ime_pinyin.set_keyboard(ime, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, ime_pinyin_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.ime_pinyin) */
    REG_METHOD(L, "create", iot_lvgl_ime_pinyin_create);
}