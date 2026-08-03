/**
 * @file iot_lvgl_arclabel.c
 * @brief LVGL弧形标签控件
 *
 * 实现LVGL弧形标签控件的OO风格Lua绑定，包括弧形标签创建、设置/获取文本、设置/获取起始角度、设置/获取角度跨度、设置方向、设置/获取颜色重绘等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"
#include "widgets/arclabel/lv_arclabel_private.h"

/* arclabel组件的metatable引用 */
static int arclabel_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_arclabel_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* arclabel = lv_arclabel_create(parent);
    lua_pushlightuserdata(L, arclabel);
    return 1;
}

/* ==================== 弧形标签OO方法 ==================== */

/*
创建弧形标签控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的弧形标签实例
@usage local label = lvgl.arclabel.create(scr)
*/
static int iot_lvgl_arclabel_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_arclabel_create_internal, arclabel_metatable_ref);
}

/*
设置弧形标签文本
@param self 弧形标签实例或指针
@param text 文本内容
@return self
@usage label:set_text("Hello Arc")
*/
static int iot_lvgl_arclabel_set_text(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_arclabel_set_text(obj, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取弧形标签文本
@param self 弧形标签实例或指针
@return string 文本内容
@usage local text = label:get_text()
*/
static int iot_lvgl_arclabel_get_text(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_t* arclabel = (lv_arclabel_t*)obj;
    lua_pushstring(L, arclabel->text ? arclabel->text : "");
    return 1;
}

/*
设置弧形起始角度
@param self 弧形标签实例或指针
@param angle 起始角度(0度=右侧,90度=底部)
@return self
@usage label:set_angle_start(0)
*/
static int iot_lvgl_arclabel_set_angle_start(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t angle = (lv_value_precise_t)luaL_checknumber(L, 2);
    lv_arclabel_set_angle_start(obj, angle);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取弧形起始角度
@param self 弧形标签实例或指针
@return number 起始角度
@usage local angle = label:get_angle_start()
*/
static int iot_lvgl_arclabel_get_angle_start(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t angle = lv_arclabel_get_angle_start(obj);
    lua_pushnumber(L, (lua_Number)angle);
    return 1;
}

/*
设置弧形角度跨度
@param self 弧形标签实例或指针
@param size 角度跨度(0-360)
@return self
@usage label:set_angle_size(180)
*/
static int iot_lvgl_arclabel_set_angle_size(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t size = (lv_value_precise_t)luaL_checknumber(L, 2);
    lv_arclabel_set_angle_size(obj, size);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取弧形角度跨度
@param self 弧形标签实例或指针
@return number 角度跨度
@usage local size = label:get_angle_size()
*/
static int iot_lvgl_arclabel_get_angle_size(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t size = lv_arclabel_get_angle_size(obj);
    lua_pushnumber(L, (lua_Number)size);
    return 1;
}

/*
设置弧形方向
@param self 弧形标签实例或指针
@param dir 方向(LV_ARCLABEL_DIR_CLOCKWISE=0, LV_ARCLABEL_DIR_COUNTER_CLOCKWISE=1)
@return self
@usage label:set_dir(lvgl.ARCLABEL_DIR_CLOCKWISE)
*/
static int iot_lvgl_arclabel_set_dir(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_dir_t dir = (lv_arclabel_dir_t)luaL_checkinteger(L, 2);
    lv_arclabel_set_dir(obj, dir);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置是否启用颜色重绘
@param self 弧形标签实例或指针
@param en true启用/false禁用
@return self
@usage label:set_recolor(true)
*/
static int iot_lvgl_arclabel_set_recolor(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_arclabel_set_recolor(obj, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取是否启用颜色重绘
@param self 弧形标签实例或指针
@return boolean 是否启用颜色重绘
@usage local en = label:get_recolor()
*/
static int iot_lvgl_arclabel_get_recolor(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lv_arclabel_get_recolor(obj);
    lua_pushboolean(L, en);
    return 1;
}

/*
设置弧形标签格式化文本(可变参数)
@param self 弧形标签实例或指针
@param fmt 格式化字符串
@param ... 可变参数
@return self
@usage label:set_text_fmt("Value: %d", 42)
*/
static int iot_lvgl_arclabel_set_text_fmt(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    const char* fmt = luaL_checkstring(L, 2);
    const char* text = lua_pushfstring(L, fmt);
    lv_arclabel_set_text(obj, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形标签静态文本
@param self 弧形标签实例或指针
@param text 静态文本(需保持有效)
@return self
@usage label:set_text_static("Static Text")
*/
static int iot_lvgl_arclabel_set_text_static(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    lv_arclabel_set_text_static(obj, text);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形标签旋转偏移
@param self 弧形标签实例或指针
@param offset 旋转偏移角度
@return self
@usage label:set_offset(45)
*/
static int iot_lvgl_arclabel_set_offset(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    int32_t offset = (int32_t)luaL_checkinteger(L, 2);
    lv_arclabel_set_offset(obj, offset);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形标签半径
@param self 弧形标签实例或指针
@param radius 半径值(像素)
@return self
@usage label:set_radius(100)
*/
static int iot_lvgl_arclabel_set_radius(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t radius = (uint32_t)luaL_checkinteger(L, 2);
    lv_arclabel_set_radius(obj, radius);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形标签中心X偏移
@param self 弧形标签实例或指针
@param x X偏移值
@return self
@usage label:set_center_offset_x(10)
*/
static int iot_lvgl_arclabel_set_center_offset_x(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t x = (uint32_t)luaL_checkinteger(L, 2);
    lv_arclabel_set_center_offset_x(obj, x);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形标签中心Y偏移
@param self 弧形标签实例或指针
@param y Y偏移值
@return self
@usage label:set_center_offset_y(10)
*/
static int iot_lvgl_arclabel_set_center_offset_y(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t y = (uint32_t)luaL_checkinteger(L, 2);
    lv_arclabel_set_center_offset_y(obj, y);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形标签垂直对齐方式
@param self 弧形标签实例或指针
@param align 垂直对齐方式(LV_ARCLABEL_TEXT_ALIGN_*)
@return self
@usage label:set_text_vertical_align(lvgl.ARCLABEL_TEXT_ALIGN_CENTER)
*/
static int iot_lvgl_arclabel_set_text_vertical_align(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_text_align_t align = (lv_arclabel_text_align_t)luaL_checkinteger(L, 2);
    lv_arclabel_set_text_vertical_align(obj, align);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形标签水平对齐方式
@param self 弧形标签实例或指针
@param align 水平对齐方式(LV_ARCLABEL_TEXT_ALIGN_*)
@return self
@usage label:set_text_horizontal_align(lvgl.ARCLABEL_TEXT_ALIGN_CENTER)
*/
static int iot_lvgl_arclabel_set_text_horizontal_align(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_text_align_t align = (lv_arclabel_text_align_t)luaL_checkinteger(L, 2);
    lv_arclabel_set_text_horizontal_align(obj, align);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形标签溢出模式
@param self 弧形标签实例或指针
@param overflow 溢出模式(LV_ARCLABEL_OVERFLOW_*)
@return self
@usage label:set_overflow(lvgl.ARCLABEL_OVERFLOW_ELLIPSIS)
*/
static int iot_lvgl_arclabel_set_overflow(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_overflow_t overflow = (lv_arclabel_overflow_t)luaL_checkinteger(L, 2);
    lv_arclabel_set_overflow(obj, overflow);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置弧形标签末尾重叠
@param self 弧形标签实例或指针
@param overlap true启用/false禁用
@return self
@usage label:set_end_overlap(true)
*/
static int iot_lvgl_arclabel_set_end_overlap(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    bool overlap = lua_toboolean(L, 2);
    lv_arclabel_set_end_overlap(obj, overlap);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取弧形标签半径
@param self 弧形标签实例或指针
@return integer 半径值
@usage local r = label:get_radius()
*/
static int iot_lvgl_arclabel_get_radius(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t radius = lv_arclabel_get_radius(obj);
    lua_pushinteger(L, (lua_Integer)radius);
    return 1;
}

/*
获取弧形标签中心X偏移
@param self 弧形标签实例或指针
@return integer X偏移值
@usage local x = label:get_center_offset_x()
*/
static int iot_lvgl_arclabel_get_center_offset_x(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t x = lv_arclabel_get_center_offset_x(obj);
    lua_pushinteger(L, (lua_Integer)x);
    return 1;
}

/*
获取弧形标签中心Y偏移
@param self 弧形标签实例或指针
@return integer Y偏移值
@usage local y = label:get_center_offset_y()
*/
static int iot_lvgl_arclabel_get_center_offset_y(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t y = lv_arclabel_get_center_offset_y(obj);
    lua_pushinteger(L, (lua_Integer)y);
    return 1;
}

/*
获取弧形标签垂直对齐方式
@param self 弧形标签实例或指针
@return integer 垂直对齐方式
@usage local align = label:get_text_vertical_align()
*/
static int iot_lvgl_arclabel_get_text_vertical_align(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_text_align_t align = lv_arclabel_get_text_vertical_align(obj);
    lua_pushinteger(L, (lua_Integer)align);
    return 1;
}

/*
获取弧形标签水平对齐方式
@param self 弧形标签实例或指针
@return integer 水平对齐方式
@usage local align = label:get_text_horizontal_align()
*/
static int iot_lvgl_arclabel_get_text_horizontal_align(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_text_align_t align = lv_arclabel_get_text_horizontal_align(obj);
    lua_pushinteger(L, (lua_Integer)align);
    return 1;
}

/*
获取弧形标签溢出模式
@param self 弧形标签实例或指针
@return integer 溢出模式
@usage local overflow = label:get_overflow()
*/
static int iot_lvgl_arclabel_get_overflow(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_arclabel_overflow_t overflow = lv_arclabel_get_overflow(obj);
    lua_pushinteger(L, (lua_Integer)overflow);
    return 1;
}

/*
获取弧形标签末尾重叠
@param self 弧形标签实例或指针
@return boolean 是否启用末尾重叠
@usage local overlap = label:get_end_overlap()
*/
static int iot_lvgl_arclabel_get_end_overlap(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    bool overlap = lv_arclabel_get_end_overlap(obj);
    lua_pushboolean(L, overlap);
    return 1;
}

/*
获取弧形标签文本角度
@param self 弧形标签实例或指针
@return number 文本角度
@usage local angle = label:get_text_angle()
*/
static int iot_lvgl_arclabel_get_text_angle(lua_State* L) {
    lv_obj_t* obj = iot_lvgl_get_obj_ptr(L, 1);
    lv_value_precise_t angle = lv_arclabel_get_text_angle(obj);
    lua_pushnumber(L, (lua_Number)angle);
    return 1;
}

/* 注册 arclabel 子模块 */
void iot_lvgl_register_arclabel(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法(可以instance:method()调用) */
    REG_METHOD(L, "set_text", iot_lvgl_arclabel_set_text);
    REG_METHOD(L, "get_text", iot_lvgl_arclabel_get_text);
    REG_METHOD(L, "set_angle_start", iot_lvgl_arclabel_set_angle_start);
    REG_METHOD(L, "get_angle_start", iot_lvgl_arclabel_get_angle_start);
    REG_METHOD(L, "set_angle_size", iot_lvgl_arclabel_set_angle_size);
    REG_METHOD(L, "get_angle_size", iot_lvgl_arclabel_get_angle_size);
    REG_METHOD(L, "set_dir", iot_lvgl_arclabel_set_dir);
    REG_METHOD(L, "set_recolor", iot_lvgl_arclabel_set_recolor);
    REG_METHOD(L, "get_recolor", iot_lvgl_arclabel_get_recolor);
    REG_METHOD(L, "set_text_fmt", iot_lvgl_arclabel_set_text_fmt);
    REG_METHOD(L, "set_text_static", iot_lvgl_arclabel_set_text_static);
    REG_METHOD(L, "set_offset", iot_lvgl_arclabel_set_offset);
    REG_METHOD(L, "set_radius", iot_lvgl_arclabel_set_radius);
    REG_METHOD(L, "set_center_offset_x", iot_lvgl_arclabel_set_center_offset_x);
    REG_METHOD(L, "set_center_offset_y", iot_lvgl_arclabel_set_center_offset_y);
    REG_METHOD(L, "set_text_vertical_align", iot_lvgl_arclabel_set_text_vertical_align);
    REG_METHOD(L, "set_text_horizontal_align", iot_lvgl_arclabel_set_text_horizontal_align);
    REG_METHOD(L, "set_overflow", iot_lvgl_arclabel_set_overflow);
    REG_METHOD(L, "set_end_overlap", iot_lvgl_arclabel_set_end_overlap);
    REG_METHOD(L, "get_radius", iot_lvgl_arclabel_get_radius);
    REG_METHOD(L, "get_center_offset_x", iot_lvgl_arclabel_get_center_offset_x);
    REG_METHOD(L, "get_center_offset_y", iot_lvgl_arclabel_get_center_offset_y);
    REG_METHOD(L, "get_text_vertical_align", iot_lvgl_arclabel_get_text_vertical_align);
    REG_METHOD(L, "get_text_horizontal_align", iot_lvgl_arclabel_get_text_horizontal_align);
    REG_METHOD(L, "get_overflow", iot_lvgl_arclabel_get_overflow);
    REG_METHOD(L, "get_end_overlap", iot_lvgl_arclabel_get_end_overlap);
    REG_METHOD(L, "get_text_angle", iot_lvgl_arclabel_get_text_angle);

    /* 保存组件metatable引用(用于继承) */
    arclabel_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.arclabel.set_text(label, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, arclabel_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.arclabel) */
    REG_METHOD(L, "create", iot_lvgl_arclabel_create);
}