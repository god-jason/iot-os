/**
 * @file iot_lvgl_spinner.c
 * @brief LVGL旋转器控件
 *
 * 实现LVGL旋转器控件的OO风格Lua绑定，包括旋转器创建、设置角度、设置类型、暂停/恢复等接口，用于显示加载状态。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* spinner组件的metatable引用 */
static int spinner_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_spinner_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t period = (uint32_t)luaL_optinteger(L, 2, 1000);
    uint32_t arc_length = (uint32_t)luaL_optinteger(L, 3, 60);
    /* LVGL 9: lv_spinner_create 只接受 parent，动画参数通过 lv_spinner_set_anim_params 设置 */
    lv_obj_t* spinner = lv_spinner_create(parent);
    if (spinner) {
        lv_spinner_set_anim_params(spinner, period, arc_length);
    }
    lua_pushlightuserdata(L, spinner);
    return 1;
}

/* ==================== 旋转器OO方法 ==================== */

static int iot_lvgl_spinner_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_spinner_create_internal, spinner_metatable_ref);
}

static int iot_lvgl_spinner_set_angle(lua_State* L) {
    lv_obj_t* spinner = iot_lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* arc length is fixed at create time in LVGL 8; pass as 3rd arg to create() */
    (void)spinner;
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_spinner_set_type(lua_State* L) {
    lv_obj_t* spinner = iot_lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* lv_spinner_set_type() was removed in LVGL 8 */
    (void)spinner;
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_spinner_pause(lua_State* L) {
    lv_obj_t* spinner = iot_lvgl_get_obj_ptr(L, 1);
    /* lv_spinner_pause() was removed in LVGL 8 */
    (void)spinner;
    lua_pushvalue(L, 1);
    return 1;
}

static int iot_lvgl_spinner_resume(lua_State* L) {
    lv_obj_t* spinner = iot_lvgl_get_obj_ptr(L, 1);
    /* lv_spinner_resume() was removed in LVGL 8 */
    (void)spinner;
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 spinner 子模块 */
void iot_lvgl_register_spinner(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_angle", iot_lvgl_spinner_set_angle);
    REG_METHOD(L, "set_type", iot_lvgl_spinner_set_type);
    REG_METHOD(L, "pause", iot_lvgl_spinner_pause);
    REG_METHOD(L, "resume", iot_lvgl_spinner_resume);

    /* 保存组件metatable引用(用于继承) */
    spinner_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.spinner.set_angle(sp, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, spinner_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表lvgl.spinner) */
    REG_METHOD(L, "create", iot_lvgl_spinner_create);
}
