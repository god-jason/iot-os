/**
 * @file lvgl_img.c
 * @brief LVGL图片控件
 *
 * 实现LVGL图片控件的OO风格Lua绑定，包括图片创建、设置图片源、设置缩放/旋转角度/旋转中心、设置偏移、设置抗锯齿、设置尺寸模式等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"

/* imgç»ä»¶çmetatableå¼ç¨ */
static int img_metatable_ref = LUA_NOREF;

/* ==================== åé¨åå»ºå½æ° ==================== */

static int lvgl_img_create_internal(lua_State* L) {
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* img = lv_img_create(parent);
    lua_pushlightuserdata(L, img);
    return 1;
}

/* ==================== å¾çOOæ¹æ³ ==================== */

/*
åå»ºå¾çæ§ä»¶(OOé£æ ¼)
@param self ç¶å¯¹è±?å¯é?
@return userdata å¸¦metatableçå¾çå®ä¾?
@usage local img = lvgl.img.create(scr)
*/
static int lvgl_img_create(lua_State* L) {
    return lvgl_obj_create_instance(L, lvgl_img_create_internal, img_metatable_ref);
}

/*
è®¾ç½®å¾çæº?
@param self å¾çå®ä¾ææé?
@param src å¾çè·¯å¾æç¬¦å·åç§?
@return self
@usage img:set_src("/ç©?bin")
*/
static int lvgl_img_set_src(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    const char* src = luaL_checkstring(L, 2);
    lv_img_set_src(img, src);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¾çXè½´åç§?
@param self å¾çå®ä¾ææé?
@param offset Xè½´åç§»é
@return self
@usage img:set_offset_x(10)
*/
static int lvgl_img_set_offset_x(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    int32_t offset = (int32_t)luaL_checkinteger(L, 2);
    lv_img_set_offset_x(img, offset);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¾çYè½´åç§?
@param self å¾çå®ä¾ææé?
@param offset Yè½´åç§»é
@return self
@usage img:set_offset_y(10)
*/
static int lvgl_img_set_offset_y(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    int32_t offset = (int32_t)luaL_checkinteger(L, 2);
    lv_img_set_offset_y(img, offset);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¾çç¼©æ¾
@param self å¾çå®ä¾ææé?
@param zoom ç¼©æ¾å?128=1å?256=2å?64=0.5å?
@return self
@usage img:set_zoom(128)  -- åå§å¤§å°
*/
static int lvgl_img_set_zoom(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    uint32_t zoom = (uint32_t)luaL_checkinteger(L, 2);
    lv_img_set_zoom(img, zoom);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¾çæè½¬è§åº¦
@param self å¾çå®ä¾ææé?
@param angle è§åº¦å?0-3600,å®éä¸ºè§åº?10)
@return self
@usage img:set_angle(900)  -- 90åº?
*/
static int lvgl_img_set_angle(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    int32_t angle = (int32_t)luaL_checkinteger(L, 2);
    lv_img_set_angle(img, angle);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¾çæè½¬ä¸­å¿ç?
@param self å¾çå®ä¾ææé?
@param x ä¸­å¿ç¹Xåæ 
@param y ä¸­å¿ç¹Yåæ 
@return self
@usage img:set_pivot(50, 50)
*/
static int lvgl_img_set_pivot(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_img_set_pivot(img, x, y);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®æé¯é½?
@param self å¾çå®ä¾ææé?
@param en æ¯å¦å¯ç¨æé¯é½?
@return self
@usage img:set_antialias(true)
*/
static int lvgl_img_set_antialias(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_img_set_antialias(img, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
è®¾ç½®å¾çå°ºå¯¸æ¨¡å¼
@param self å¾çå®ä¾ææé?
@param mode å°ºå¯¸æ¨¡å¼
@return self
@usage img:set_size_mode(lvgl.IMG_SIZE_MODE_REAL)
*/
static int lvgl_img_set_size_mode(lua_State* L) {
    lv_obj_t* img = lvgl_get_obj_ptr(L, 1);
    lv_img_size_mode_t mode = (lv_img_size_mode_t)luaL_checkinteger(L, 2);
    lv_img_set_size_mode(img, mode);
    lua_pushvalue(L, 1);
    return 1;
}

/* æ³¨å img å­æ¨¡块*/
void lvgl_register_img(lua_State* L) {
    /* åå»ºç»ä»¶æ¹æ³è¡?ç¨äºmetatableç»§æ¿) */
    lua_newtable(L);

    /* æ³¨åOOé£æ ¼æ¹æ³ */
    REG_METHOD(L, "set_src", lvgl_img_set_src);
    REG_METHOD(L, "set_offset_x", lvgl_img_set_offset_x);
    REG_METHOD(L, "set_offset_y", lvgl_img_set_offset_y);
    REG_METHOD(L, "set_zoom", lvgl_img_set_zoom);
    REG_METHOD(L, "set_angle", lvgl_img_set_angle);
    REG_METHOD(L, "set_pivot", lvgl_img_set_pivot);
    REG_METHOD(L, "set_antialias", lvgl_img_set_antialias);
    REG_METHOD(L, "set_size_mode", lvgl_img_set_size_mode);

    /* ä¿å­ç»ä»¶metatableå¼ç¨(ç¨äºç»§æ¿) */
    img_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* å°æ¹æ³å¤å¶å°ç»ä»¶å­è¡¨(æ¯æ lvgl.img.set_src(img, ...) è°ç¨) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, img_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* æ³¨åcreateå½æ°å°ä¸»è¡?lvgl.img) */
    REG_METHOD(L, "create", lvgl_img_create);
}
