/**
 * @file iot_lvgl_buttonmatrix.c
 * @brief LVGL按钮矩阵控件
 *
 * 实现LVGL按钮矩阵控件的OO风格Lua绑定，包括按钮矩阵创建、设置按钮映射表、设置选中按钮、设置/清除按钮控制位、设置按钮宽度、获取选中按钮索引、获取按钮文本、获取按钮数量等接口，支持链式调用。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.08.03
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"
#include "src/widgets/buttonmatrix/lv_buttonmatrix.h"

/* buttonmatrix组件的metatable引用 */
static int buttonmatrix_metatable_ref = LUA_NOREF;

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_buttonmatrix_create_internal(lua_State* L) {
    lv_obj_t* parent = iot_lvgl_get_obj_ptr(L, 1);
    lv_obj_t* btnm = lv_buttonmatrix_create(parent);
    lua_pushlightuserdata(L, btnm);
    return 1;
}

/* ==================== 按钮矩阵OO方法 ==================== */

/*
创建按钮矩阵(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的按钮矩阵实例
@usage local btnm = lvgl.buttonmatrix.create(scr)
*/
static int iot_lvgl_buttonmatrix_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_buttonmatrix_create_internal, buttonmatrix_metatable_ref);
}

/*
设置按钮映射表
@param self 按钮矩阵实例或指针
@param map 按钮文本数组(字符串表),使用"\n"元素表示换行,最后一个元素必须为nil或结束标志
@return self
@usage btnm:set_map({"Btn1", "Btn2", "Btn3", "\n", "Btn4", "Btn5"})
@note 此函数会分配临时内存构建NULL结尾的字符串数组,调用LVGL API后自动释放
*/
static int iot_lvgl_buttonmatrix_set_map(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);

    /* 获取表长度(按钮数量) */
    int n = (int)lua_rawlen(L, 2);

    /* 分配NULL结尾的字符串指针数组 */
    const char** map = (const char**)cm_malloc((size_t)(n + 1) * sizeof(const char*));
    if (map == NULL) {
        return luaL_error(L, "内存分配失败");
    }

    /* 遍历Lua表,填充字符串指针 */
    int i;
    for (i = 0; i < n; i++) {
        lua_rawgeti(L, 2, i + 1);  /* Lua表索引从1开始 */
        if (lua_isstring(L, -1)) {
            map[i] = lua_tostring(L, -1);
        } else {
            cm_free(map);
            return luaL_error(L, "按钮映射表第%d个元素必须是字符串(或\"\\n\"表示换行)", i + 1);
        }
    }
    map[n] = NULL;  /* NULL结尾 */

    lv_buttonmatrix_set_map(btnm, map);

    cm_free(map);

    lua_pushvalue(L, 1);
    return 1;
}

/*
设置选中按钮
@param self 按钮矩阵实例或指针
@param btn_id 按钮索引(0-based,不计换行符)
@return self
@usage btnm:set_selected_btn(0)
*/
static int iot_lvgl_buttonmatrix_set_selected_btn(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t btn_id = (uint32_t)luaL_checkinteger(L, 2);
    lv_buttonmatrix_set_selected_button(btnm, btn_id);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置按钮控制位
@param self 按钮矩阵实例或指针
@param btn_id 按钮索引(0-based,不计换行符)
@param ctrl 控制位(OR组合,如 lvgl.BUTTONMATRIX_CTRL_CHECKABLE | lvgl.BUTTONMATRIX_CTRL_NO_REPEAT)
@return self
@usage btnm:set_btn_ctrl(0, lvgl.BUTTONMATRIX_CTRL_CHECKABLE)
*/
static int iot_lvgl_buttonmatrix_set_btn_ctrl(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t btn_id = (uint32_t)luaL_checkinteger(L, 2);
    lv_buttonmatrix_ctrl_t ctrl = (lv_buttonmatrix_ctrl_t)luaL_checkinteger(L, 3);
    lv_buttonmatrix_set_button_ctrl(btnm, btn_id, ctrl);
    lua_pushvalue(L, 1);
    return 1;
}

/*
清除按钮控制位
@param self 按钮矩阵实例或指针
@param btn_id 按钮索引(0-based,不计换行符)
@param ctrl 控制位(OR组合)
@return self
@usage btnm:clear_btn_ctrl(0, lvgl.BUTTONMATRIX_CTRL_CHECKABLE)
*/
static int iot_lvgl_buttonmatrix_clear_btn_ctrl(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t btn_id = (uint32_t)luaL_checkinteger(L, 2);
    lv_buttonmatrix_ctrl_t ctrl = (lv_buttonmatrix_ctrl_t)luaL_checkinteger(L, 3);
    lv_buttonmatrix_clear_button_ctrl(btnm, btn_id, ctrl);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置按钮宽度
@param self 按钮矩阵实例或指针
@param btn_id 按钮索引(0-based,不计换行符)
@param width 相对宽度(1-15)
@return self
@usage btnm:set_btn_width(0, 2)
*/
static int iot_lvgl_buttonmatrix_set_btn_width(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t btn_id = (uint32_t)luaL_checkinteger(L, 2);
    uint32_t width = (uint32_t)luaL_checkinteger(L, 3);
    lv_buttonmatrix_set_button_width(btnm, btn_id, width);
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取选中按钮索引
@param self 按钮矩阵实例或指针
@return integer 选中按钮的索引(0-based),无选中时返回 0xFFFF(LV_BUTTONMATRIX_BUTTON_NONE)
@usage local idx = btnm:get_selected_btn()
*/
static int iot_lvgl_buttonmatrix_get_selected_btn(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t btn_id = lv_buttonmatrix_get_selected_button(btnm);
    lua_pushinteger(L, btn_id);
    return 1;
}

/*
获取按钮文本
@param self 按钮矩阵实例或指针
@param btn_id 按钮索引(0-based,不计换行符)
@return string 按钮文本
@usage local text = btnm:get_btn_text(0)
*/
static int iot_lvgl_buttonmatrix_get_btn_text(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t btn_id = (uint32_t)luaL_checkinteger(L, 2);
    const char* text = lv_buttonmatrix_get_button_text(btnm, btn_id);
    lua_pushstring(L, text);
    return 1;
}

/*
获取按钮数量
@param self 按钮矩阵实例或指针
@return integer 按钮总数(不计换行符)
@usage local count = btnm:get_btn_count()
*/
static int iot_lvgl_buttonmatrix_get_btn_count(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    const char* const* map = lv_buttonmatrix_get_map(btnm);
    uint32_t count = 0;
    if (map != NULL) {
        while (*map != NULL) {
            if (strcmp(*map, "\n") != 0) {
                count++;
            }
            map++;
        }
    }
    lua_pushinteger(L, count);
    return 1;
}

/*
设置按钮控制映射表
@param self 按钮矩阵实例或指针
@param ctrl_map 控制映射表(整数数组)
@return self
@usage btnm:set_ctrl_map({lvgl.BUTTONMATRIX_CTRL_CHECKABLE, 0, lvgl.BUTTONMATRIX_CTRL_NO_REPEAT})
*/
static int iot_lvgl_buttonmatrix_set_ctrl_map(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    luaL_checktype(L, 2, LUA_TTABLE);
    int n = (int)lua_rawlen(L, 2);
    lv_buttonmatrix_ctrl_t* ctrl_map = (lv_buttonmatrix_ctrl_t*)cm_malloc((size_t)n * sizeof(lv_buttonmatrix_ctrl_t));
    if (ctrl_map == NULL) return luaL_error(L, "内存分配失败");
    for (int i = 0; i < n; i++) {
        lua_rawgeti(L, 2, i + 1);
        ctrl_map[i] = (lv_buttonmatrix_ctrl_t)luaL_checkinteger(L, -1);
        lua_pop(L, 1);
    }
    lv_buttonmatrix_set_ctrl_map(btnm, ctrl_map);
    cm_free(ctrl_map);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置所有按钮的控制位
@param self 按钮矩阵实例或指针
@param ctrl 控制位(OR组合)
@return self
@usage btnm:set_btn_ctrl_all(lvgl.BUTTONMATRIX_CTRL_CHECKABLE)
*/
static int iot_lvgl_buttonmatrix_set_btn_ctrl_all(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    lv_buttonmatrix_ctrl_t ctrl = (lv_buttonmatrix_ctrl_t)luaL_checkinteger(L, 2);
    lv_buttonmatrix_set_button_ctrl_all(btnm, ctrl);
    lua_pushvalue(L, 1);
    return 1;
}

/*
清除所有按钮的控制位
@param self 按钮矩阵实例或指针
@param ctrl 控制位(OR组合)
@return self
@usage btnm:clear_btn_ctrl_all(lvgl.BUTTONMATRIX_CTRL_CHECKABLE)
*/
static int iot_lvgl_buttonmatrix_clear_btn_ctrl_all(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    lv_buttonmatrix_ctrl_t ctrl = (lv_buttonmatrix_ctrl_t)luaL_checkinteger(L, 2);
    lv_buttonmatrix_clear_button_ctrl_all(btnm, ctrl);
    lua_pushvalue(L, 1);
    return 1;
}

/*
设置单选模式
@param self 按钮矩阵实例或指针
@param en 是否启用单选模式
@return self
@usage btnm:set_one_checked(true)
*/
static int iot_lvgl_buttonmatrix_set_one_checked(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    lv_buttonmatrix_set_one_checked(btnm, en);
    lua_pushvalue(L, 1);
    return 1;
}

/*
检查按钮是否有指定控制位
@param self 按钮矩阵实例或指针
@param btn_id 按钮索引(0-based,不计换行符)
@param ctrl 控制位
@return boolean 是否具有指定控制位
@usage local has = btnm:has_btn_ctrl(0, lvgl.BUTTONMATRIX_CTRL_CHECKABLE)
*/
static int iot_lvgl_buttonmatrix_has_btn_ctrl(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    uint32_t btn_id = (uint32_t)luaL_checkinteger(L, 2);
    lv_buttonmatrix_ctrl_t ctrl = (lv_buttonmatrix_ctrl_t)luaL_checkinteger(L, 3);
    bool has = lv_buttonmatrix_has_button_ctrl(btnm, btn_id, ctrl);
    lua_pushboolean(L, has);
    return 1;
}

/*
获取单选模式状态
@param self 按钮矩阵实例或指针
@return boolean 是否启用单选模式
@usage local en = btnm:get_one_checked()
*/
static int iot_lvgl_buttonmatrix_get_one_checked(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    bool en = lv_buttonmatrix_get_one_checked(btnm);
    lua_pushboolean(L, en);
    return 1;
}

/*
获取按钮映射表
@param self 按钮矩阵实例或指针
@return table 按钮文本数组(字符串表)
@usage local map = btnm:get_map()
*/
static int iot_lvgl_buttonmatrix_get_map(lua_State* L) {
    lv_obj_t* btnm = iot_lvgl_get_obj_ptr(L, 1);
    const char* const* map = lv_buttonmatrix_get_map(btnm);
    lua_newtable(L);
    int idx = 1;
    if (map != NULL) {
        while (*map != NULL) {
            lua_pushstring(L, *map);
            lua_rawseti(L, -2, idx++);
            map++;
        }
    }
    return 1;
}

/* 注册 buttonmatrix 子模块 */
void iot_lvgl_register_buttonmatrix(lua_State* L) {
    /* 创建组件方法表(用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法(可以instance:method()调用) */
    REG_METHOD(L, "set_map", iot_lvgl_buttonmatrix_set_map);
    REG_METHOD(L, "set_selected_btn", iot_lvgl_buttonmatrix_set_selected_btn);
    REG_METHOD(L, "set_btn_ctrl", iot_lvgl_buttonmatrix_set_btn_ctrl);
    REG_METHOD(L, "clear_btn_ctrl", iot_lvgl_buttonmatrix_clear_btn_ctrl);
    REG_METHOD(L, "set_btn_width", iot_lvgl_buttonmatrix_set_btn_width);
    REG_METHOD(L, "get_selected_btn", iot_lvgl_buttonmatrix_get_selected_btn);
    REG_METHOD(L, "get_btn_text", iot_lvgl_buttonmatrix_get_btn_text);
    REG_METHOD(L, "get_btn_count", iot_lvgl_buttonmatrix_get_btn_count);
    REG_METHOD(L, "set_ctrl_map", iot_lvgl_buttonmatrix_set_ctrl_map);
    REG_METHOD(L, "set_btn_ctrl_all", iot_lvgl_buttonmatrix_set_btn_ctrl_all);
    REG_METHOD(L, "clear_btn_ctrl_all", iot_lvgl_buttonmatrix_clear_btn_ctrl_all);
    REG_METHOD(L, "set_one_checked", iot_lvgl_buttonmatrix_set_one_checked);
    REG_METHOD(L, "has_btn_ctrl", iot_lvgl_buttonmatrix_has_btn_ctrl);
    REG_METHOD(L, "get_one_checked", iot_lvgl_buttonmatrix_get_one_checked);
    REG_METHOD(L, "get_map", iot_lvgl_buttonmatrix_get_map);

    /* 保存组件metatable引用(用于继承) */
    buttonmatrix_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.buttonmatrix.set_map(btnm, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, buttonmatrix_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表(lvgl.buttonmatrix) */
    REG_METHOD(L, "create", iot_lvgl_buttonmatrix_create);
}