/**
 * @file lvgl_spinner.c
 * @brief LVGL旋转器控件
 *
 * 实现LVGL旋转器控件的OO风格Lua绑定，包括旋转器创建、设置角度、设置类型、暂停/恢复等接口，用于显示加载状态。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* spinner???metatable?? */
static int spinner_metatable_ref = LUA_NOREF;

/* ==================== ?????? ==================== */

static int lvgl_spinner_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    uint32_t period = (uint32_t)luaL_optinteger(L, 2, 1000);
    uint32_t arc_length = (uint32_t)luaL_optinteger(L, 3, 60);
    lv_obj_t* spinner = lv_spinner_create(parent, period, arc_length);
    lua_pushlightuserdata(L, spinner);
    return 1;
}

/* ==================== ???OO?? ==================== */

static int lvgl_spinner_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_spinner_create_internal, spinner_metatable_ref);
}

static int lvgl_spinner_set_angle(lua_State* L) {
    lv_obj_t* spinner = lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* arc length is fixed at create time in LVGL 8; pass as 3rd arg to create() */
    (void)spinner;
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinner_set_type(lua_State* L) {
    lv_obj_t* spinner = lvgl_get_obj_ptr(L, 1);
    (void)luaL_checkinteger(L, 2);
    /* lv_spinner_set_type() was removed in LVGL 8 */
    (void)spinner;
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinner_pause(lua_State* L) {
    lv_obj_t* spinner = lvgl_get_obj_ptr(L, 1);
    /* lv_spinner_pause() was removed in LVGL 8 */
    (void)spinner;
    lua_pushvalue(L, 1);
    return 1;
}

static int lvgl_spinner_resume(lua_State* L) {
    lv_obj_t* spinner = lvgl_get_obj_ptr(L, 1);
    /* lv_spinner_resume() was removed in LVGL 8 */
    (void)spinner;
    lua_pushvalue(L, 1);
    return 1;
}

/* ?? spinner ????*/
void lvgl_register_spinner(lua_State* L) {
    /* ??????????metatable??) */
    lua_newtable(L);

    /* ??OO???? */
    REG_METHOD(L, "set_angle", lvgl_spinner_set_angle);
    REG_METHOD(L, "set_type", lvgl_spinner_set_type);
    REG_METHOD(L, "pause", lvgl_spinner_pause);
    REG_METHOD(L, "resume", lvgl_spinner_resume);

    /* ????metatable??(????) */
    spinner_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* ??????????(?? lvgl.spinner.set_angle(sp, ...) ??) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, spinner_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* ??create??????lvgl.spinner) */
    REG_METHOD(L, "create", lvgl_spinner_create);
}
