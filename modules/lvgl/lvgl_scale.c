/**
 * @file iot_lvgl_scale.c
 * @brief LVGL刻度/标尺控件
 *
 * 实现LVGL刻度/标尺控件的OO风格Lua绑定，包括刻度创建、设置模式、设置刻度数量、设置主刻度间隔、设置标签显示、设置范围、设置后绘制等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* scale组件的metatable引用 */
static int scale_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_scale_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* scale = lv_scale_create(parent);
    lua_pushlightuserdata(L, scale);
    return 1;
}

/* ==================== 刻度OO方法 ==================== */

/*
创建刻度控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的刻度实例
@usage local scale = lvgl.scale.create(scr)
*/
static int iot_lvgl_scale_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_scale_create_internal, scale_metatable_ref);
}

/*
设置刻度模式
@param self 刻度实例或指针
@param mode 刻度模式 (lv_scale_mode_t)
@return self
@usage scale:set_mode(lvgl.SCALE_MODE_HORIZONTAL_TOP)
*/
static int iot_lvgl_scale_set_mode(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    lv_scale_mode_t mode = (lv_scale_mode_t)luaL_checkinteger(L, 2);
    lv_scale_set_mode(scale, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置总刻度数量(包含小刻度和大刻度)
@param self 刻度实例或指针
@param count 总刻度数量
@return self
@usage scale:set_total_tick_count(11)
*/
static int iot_lvgl_scale_set_total_tick_count(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t count = (uint32_t)luaL_checkinteger(L, 2);
    lv_scale_set_total_tick_count(scale, count);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置主刻度间隔(每N个刻度为主刻度)
@param self 刻度实例或指针
@param count 主刻度间隔
@return self
@usage scale:set_major_tick_every(5)
*/
static int iot_lvgl_scale_set_major_tick_every(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t count = (uint32_t)luaL_checkinteger(L, 2);
    lv_scale_set_major_tick_every(scale, count);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置标签显示
@param self 刻度实例或指针
@param show 是否显示标签
@return self
@usage scale:set_label_show(true)
*/
static int iot_lvgl_scale_set_label_show(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    bool show = lua_toboolean(L, 2);
    lv_scale_set_label_show(scale, show);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置刻度值范围
@param self 刻度实例或指针
@param min 最小值
@param max 最大值
@return self
@usage scale:set_range(0, 100)
*/
static int iot_lvgl_scale_set_range(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    int32_t max = (int32_t)luaL_checkinteger(L, 3);
    lv_scale_set_range(scale, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置刻度最小值
@param self 刻度实例或指针
@param min 最小值
@return self
@usage scale:set_min_value(0)
*/
static int iot_lvgl_scale_set_min_value(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    int32_t min = (int32_t)luaL_checkinteger(L, 2);
    lv_scale_set_min_value(scale, min);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置刻度最大值
@param self 刻度实例或指针
@param max 最大值
@return self
@usage scale:set_max_value(100)
*/
static int iot_lvgl_scale_set_max_value(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    int32_t max = (int32_t)luaL_checkinteger(L, 2);
    lv_scale_set_max_value(scale, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置角度范围(仅适用于圆形刻度)
@param self 刻度实例或指针
@param angle_range 角度范围(度)
@return self
@usage scale:set_angle_range(270)
*/
static int iot_lvgl_scale_set_angle_range(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t angle_range = (uint32_t)luaL_checkinteger(L, 2);
    lv_scale_set_angle_range(scale, angle_range);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置旋转角度(仅适用于圆形刻度)
@param self 刻度实例或指针
@param rotation 旋转角度(度)
@return self
@usage scale:set_rotation(135)
*/
static int iot_lvgl_scale_set_rotation(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    int32_t rotation = (int32_t)luaL_checkinteger(L, 2);
    lv_scale_set_rotation(scale, rotation);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置后绘制(子对象绘制后再绘制刻度)
@param self 刻度实例或指针
@param en 是否启用后绘制
@return self
@usage scale:set_post_draw(true)
*/
static int iot_lvgl_scale_set_post_draw(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_scale_set_post_draw(scale, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置刻度线绘制在最上层
@param self 刻度实例或指针
@param en 是否在最上层绘制刻度线
@return self
@usage scale:set_draw_ticks_on_top(true)
*/
static int iot_lvgl_scale_set_draw_ticks_on_top(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_scale_set_draw_ticks_on_top(scale, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取刻度模式
@param self 刻度实例或指针
@return integer 刻度模式
@usage local mode = scale:get_mode()
*/
static int iot_lvgl_scale_get_mode(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    lv_scale_mode_t mode = lv_scale_get_mode(scale);
    lua_pushinteger(L, mode);
    return 1;
}

/*
获取总刻度数量
@param self 刻度实例或指针
@return integer 总刻度数量
@usage local count = scale:get_total_tick_count()
*/
static int iot_lvgl_scale_get_total_tick_count(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    int32_t count = lv_scale_get_total_tick_count(scale);
    lua_pushinteger(L, count);
    return 1;
}

/*
获取主刻度间隔
@param self 刻度实例或指针
@return integer 主刻度间隔
@usage local every = scale:get_major_tick_every()
*/
static int iot_lvgl_scale_get_major_tick_every(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    int32_t every = lv_scale_get_major_tick_every(scale);
    lua_pushinteger(L, every);
    return 1;
}

/*
获取标签显示状态
@param self 刻度实例或指针
@return boolean 是否显示标签
@usage local show = scale:get_label_show()
*/
static int iot_lvgl_scale_get_label_show(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    bool show = lv_scale_get_label_show(scale);
    lua_pushboolean(L, show);
    return 1;
}

/*
获取旋转角度
@param self 刻度实例或指针
@return integer 旋转角度
@usage local rotation = scale:get_rotation()
*/
static int iot_lvgl_scale_get_rotation(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    int32_t rotation = lv_scale_get_rotation(scale);
    lua_pushinteger(L, rotation);
    return 1;
}

/*
获取角度范围
@param self 刻度实例或指针
@return integer 角度范围
@usage local range = scale:get_angle_range()
*/
static int iot_lvgl_scale_get_angle_range(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t range = lv_scale_get_angle_range(scale);
    lua_pushinteger(L, range);
    return 1;
}

/*
获取最小值
@param self 刻度实例或指针
@return integer 最小值
@usage local min = scale:get_range_min_value()
*/
static int iot_lvgl_scale_get_range_min_value(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    int32_t min = lv_scale_get_range_min_value(scale);
    lua_pushinteger(L, min);
    return 1;
}

/*
获取最大值
@param self 刻度实例或指针
@return integer 最大值
@usage local max = scale:get_range_max_value()
*/
static int iot_lvgl_scale_get_range_max_value(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    int32_t max = lv_scale_get_range_max_value(scale);
    lua_pushinteger(L, max);
    return 1;
}

/*
添加刻度分区
@param self 刻度实例或指针
@return userdata 分区指针
@usage local section = scale:add_section()
*/
static int iot_lvgl_scale_add_section(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    lv_scale_section_t* section = lv_scale_add_section(scale);
    lua_pushlightuserdata(L, section);
    return 1;
}

/*
设置分区范围
@param self 刻度实例或指针
@param section 分区指针
@param min 最小值
@param max 最大值
@return self
@usage scale:set_section_range(section, 0, 50)
*/
static int iot_lvgl_scale_set_section_range(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    lv_scale_section_t* section = (lv_scale_section_t*)luaL_checklightuserdata(L, 2);
    int32_t min = (int32_t)luaL_checkinteger(L, 3);
    int32_t max = (int32_t)luaL_checkinteger(L, 4);
    lv_scale_set_section_range(scale, section, min, max);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置线条指针指向指定值
@param self 刻度实例或指针
@param needle_line 指针线条对象
@param needle_length 指针长度
@param value 目标值
@return self
@usage scale:set_line_needle_value(needle_line, 50, 75)
*/
static int iot_lvgl_scale_set_line_needle_value(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* needle_line = iot_lvgl_get_obj_ptr(L, 2);
    int32_t needle_length = (int32_t)luaL_checkinteger(L, 3);
    int32_t value = (int32_t)luaL_checkinteger(L, 4);
    lv_scale_set_line_needle_value(scale, needle_line, needle_length, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置图片指针指向指定值
@param self 刻度实例或指针
@param needle_img 指针图片对象
@param value 目标值
@return self
@usage scale:set_image_needle_value(needle_img, 75)
*/
static int iot_lvgl_scale_set_image_needle_value(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* needle_img = iot_lvgl_get_obj_ptr(L, 2);
    int32_t value = (int32_t)luaL_checkinteger(L, 3);
    lv_scale_set_image_needle_value(scale, needle_img, value);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置自定义标签文本源
@param self 刻度实例或指针
@param txt_src 标签文本数组
@return self
@usage scale:set_text_src({"Low", "Mid", "High"})
*/
static int iot_lvgl_scale_set_text_src(lua_State* L) {
    lv_obj_t* scale = iot_lvgl_get_obj_ptr(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    int n = (int)lua_rawlen(L, 2);
    const char** txt_src = (const char**)cm_malloc((size_t)(n + 1) * sizeof(const char*));
    if (txt_src == NULL) return luaL_error(L, "内存分配失败");
    for (int i = 0; i < n; i++) {
        lua_rawgeti(L, 2, i + 1);
        txt_src[i] = lua_tostring(L, -1);
    }
    txt_src[n] = NULL;
    lv_scale_set_text_src(scale, txt_src);
    cm_free(txt_src);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 scale 子模块 */
void iot_lvgl_register_scale(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法(可以 instance:method() 调用) */
    REG_METHOD(L, "set_mode", iot_lvgl_scale_set_mode);
    REG_METHOD(L, "set_total_tick_count", iot_lvgl_scale_set_total_tick_count);
    REG_METHOD(L, "set_major_tick_every", iot_lvgl_scale_set_major_tick_every);
    REG_METHOD(L, "set_label_show", iot_lvgl_scale_set_label_show);
    REG_METHOD(L, "set_range", iot_lvgl_scale_set_range);
    REG_METHOD(L, "set_min_value", iot_lvgl_scale_set_min_value);
    REG_METHOD(L, "set_max_value", iot_lvgl_scale_set_max_value);
    REG_METHOD(L, "set_angle_range", iot_lvgl_scale_set_angle_range);
    REG_METHOD(L, "set_rotation", iot_lvgl_scale_set_rotation);
    REG_METHOD(L, "set_post_draw", iot_lvgl_scale_set_post_draw);
    REG_METHOD(L, "set_draw_ticks_on_top", iot_lvgl_scale_set_draw_ticks_on_top);
    REG_METHOD(L, "get_mode", iot_lvgl_scale_get_mode);
    REG_METHOD(L, "get_total_tick_count", iot_lvgl_scale_get_total_tick_count);
    REG_METHOD(L, "get_major_tick_every", iot_lvgl_scale_get_major_tick_every);
    REG_METHOD(L, "get_label_show", iot_lvgl_scale_get_label_show);
    REG_METHOD(L, "get_rotation", iot_lvgl_scale_get_rotation);
    REG_METHOD(L, "get_angle_range", iot_lvgl_scale_get_angle_range);
    REG_METHOD(L, "get_range_min_value", iot_lvgl_scale_get_range_min_value);
    REG_METHOD(L, "get_range_max_value", iot_lvgl_scale_get_range_max_value);
    REG_METHOD(L, "add_section", iot_lvgl_scale_add_section);
    REG_METHOD(L, "set_section_range", iot_lvgl_scale_set_section_range);
    REG_METHOD(L, "set_line_needle_value", iot_lvgl_scale_set_line_needle_value);
    REG_METHOD(L, "set_image_needle_value", iot_lvgl_scale_set_image_needle_value);
    REG_METHOD(L, "set_text_src", iot_lvgl_scale_set_text_src);

    /* 保存组件metatable引用(用于继承) */
    scale_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.scale.set_mode(scale, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, scale_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.scale) */
    REG_METHOD(L, "create", iot_lvgl_scale_create);
}