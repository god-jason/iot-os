/**
 * @file lvgl_roller.c
 * @brief LVGL滚轮控件
 *
 * 实现LVGL滚轮控件的OO风格Lua绑定，包括滚轮创建、设置选项、设置/获取选中项、设置可见行数等接口，用于从多个选项中选择一项。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* rollerç»ä»¶çmetatableå¼ç¨ */
static int roller_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_roller_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* roller = lv_roller_create(parent);
    lua_pushlightuserdata(L, roller);
    return 1;
}

/* ==================== æ»è½®OOæ¹æ³ ==================== */

/*
åå»ºæ»è½®æ§ä»¶(OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçæ»è½®å®ä¾?
@usage local roller = lvgl.roller.create(scr)
*/
static int lvgl_roller_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_roller_create_internal, roller_metatable_ref);
}

/*
è®¾ç½®æ»è½®éé¡¹
@param self æ»è½®å®ä¾ææé?
@param options éé¡¹å­ç¬¦ä¸?ç¨\nåé)
@param mode æ¨¡å¼(å¯é?é»è®¤NORMAL)
@return self
@usage roller:set_options("A\nB\nC", lvgl.ROLLER_MODE_NORMAL)
*/
static int lvgl_roller_set_options(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    const char* options = luaL_checkstring(L, 2);
    lv_roller_mode_t mode = (lv_roller_mode_t)luaL_optinteger(L, 3, LV_ROLLER_MODE_NORMAL);
    lv_roller_set_options(roller, options, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®éä¸­é¡?
@param self æ»è½®å®ä¾ææé?
@param sel éä¸­é¡¹ç´¢å¼?
@param anim æ¯å¦å¨ç»(å¯é?
@return self
@usage roller:set_selected(1, 0)
*/
static int lvgl_roller_set_selected(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    uint16_t sel = (uint16_t)luaL_checkinteger(L, 2);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 3, LV_ANIM_OFF);
    lv_roller_set_selected(roller, sel, anim);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¯è§è¡æ°
@param self æ»è½®å®ä¾ææé?
@param row_cnt è¡æ°
@return self
@usage roller:set_visible_row_count(3)
*/
static int lvgl_roller_set_visible_row_count(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    uint8_t row_cnt = (uint8_t)luaL_checkinteger(L, 2);
    lv_roller_set_visible_row_count(roller, row_cnt);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è·åéä¸­é¡¹ç´¢å¼?
@param self æ»è½®å®ä¾ææé?
@return integer éä¸­é¡¹ç´¢å¼?
@usage local sel = roller:get_selected()
*/
static int lvgl_roller_get_selected(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    uint16_t sel = lv_roller_get_selected(roller);
    lua_pushinteger(L, sel);
    return 1;
}

static int lvgl_roller_get_selected_str(lua_State* L) {
    lv_obj_t* roller = lvgl_get_obj_ptr(L, 1);
    char buf[256];
    uint32_t len = (uint32_t)luaL_optinteger(L, 2, sizeof(buf));
    lv_roller_get_selected_str(roller, buf, len);
    lua_pushstring(L, buf);
    return 1;
}

/* æ³¨å roller å­æ¨¡å?*/
void lvgl_register_roller(lua_State* L) {
    /* åå»ºç»ä»¶æ¹æ³è¡?ç¨äºmetatableç»§æ¿) */
    lua_newtable(L);

    /* æ³¨åOOé£æ ¼æ¹æ³ */
    REG_METHOD(L, "set_options", lvgl_roller_set_options);
    REG_METHOD(L, "set_selected", lvgl_roller_set_selected);
    REG_METHOD(L, "set_visible_row_count", lvgl_roller_set_visible_row_count);
    REG_METHOD(L, "get_selected", lvgl_roller_get_selected);
    REG_METHOD(L, "get_selected_str", lvgl_roller_get_selected_str);

    /* ä¿å­ç»ä»¶metatableå¼ç¨(ç¨äºç»§æ¿) */
    roller_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* å°æ¹æ³å¤å¶å°ç»ä»¶å­è¡¨(æ¯æ lvgl.roller.set_options(roller, ...) è°ç¨) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, roller_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.roller) */
    REG_METHOD(L, "create", lvgl_roller_create);
}
