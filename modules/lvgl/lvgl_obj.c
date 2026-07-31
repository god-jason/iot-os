/**
 * @file lvgl_obj.c
 * @brief LVGL对象系统实现（OO风格）
 *
 * 实现LVGL对象系统的核心功能，包括对象指针获取（支持userdata和table两种传参方式）、OO实例创建（绑定元表和方法）、对象metatable注册等，为所有控件提供统一的OO风格Lua绑定基础。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"
#include "iot_callback.h"
#include <stdio.h>

static int obj_metatable_ref = LUA_NOREF;

/* 前置声明（供 lvgl_register_obj 注册使用） */
int lvgl_obj_has_state(lua_State* L);
int lvgl_obj_add_state(lua_State* L);
int lvgl_obj_clear_state(lua_State* L);

lv_obj_t* lvgl_get_obj_ptr(lua_State* L, int idx) {
    int abs_idx = idx > 0 ? idx : (lua_gettop(L) + idx + 1);

    if (lua_isuserdata(L, abs_idx)) {
        if (lua_islightuserdata(L, abs_idx)) {
            /* lightuserdata: 指针直接存储 */
            return (lv_obj_t*)lua_touserdata(L, abs_idx);
        } else {
            /* full userdata: 存储的是 lv_obj_t* 指针，需要解引用 */
            lv_obj_t** p = (lv_obj_t**)lua_touserdata(L, abs_idx);
            return p ? *p : NULL;
        }
    }

    if (lua_istable(L, abs_idx)) {
        if (abs_idx == 1 && lua_gettop(L) >= 2) {
            if (lua_isuserdata(L, abs_idx + 1)) {
                return (lv_obj_t*)lua_touserdata(L, abs_idx + 1);
            }
            if (lua_istable(L, abs_idx + 1)) {
                lua_getfield(L, abs_idx + 1, "obj");
                if (lua_isuserdata(L, -1)) {
                    lv_obj_t* obj = (lv_obj_t*)lua_touserdata(L, -1);
                    lua_pop(L, 1);
                    return obj;
                }
                lua_pop(L, 1);
            }
        }
        lua_getfield(L, abs_idx, "obj");
        if (lua_isuserdata(L, -1)) {
            lv_obj_t* obj = (lv_obj_t*)lua_touserdata(L, -1);
            lua_pop(L, 1);
            return obj;
        }
        lua_pop(L, 1);
    }

    luaL_error(L, "invalid object parameter at index %d", idx);
    return NULL;
}

int lvgl_register_obj(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "delete", lvgl_obj_delete);
    REG_METHOD(L, "clean", lvgl_obj_clean);
    REG_METHOD(L, "get_parent", lvgl_obj_get_parent);
    REG_METHOD(L, "get_child_cnt", lvgl_obj_get_child_cnt);

    REG_METHOD(L, "set_pos", lvgl_obj_set_pos);
    REG_METHOD(L, "set_size", lvgl_obj_set_size);
    REG_METHOD(L, "set_width", lvgl_obj_set_width);
    REG_METHOD(L, "set_height", lvgl_obj_set_height);
    REG_METHOD(L, "set_x", lvgl_obj_set_x);
    REG_METHOD(L, "set_y", lvgl_obj_set_y);
    REG_METHOD(L, "align", lvgl_obj_align);
    REG_METHOD(L, "align_to", lvgl_obj_align_to);
    REG_METHOD(L, "center", lvgl_obj_center);

    REG_METHOD(L, "set_click", lvgl_obj_set_click);
    REG_METHOD(L, "set_hidden", lvgl_obj_set_hidden);
    REG_METHOD(L, "set_user_data", lvgl_obj_set_user_data);
    REG_METHOD(L, "add_style", lvgl_obj_add_style);

    REG_METHOD(L, "add_to", lvgl_obj_add_to);
    REG_METHOD(L, "move_foreground", lvgl_obj_move_foreground);
    REG_METHOD(L, "move_background", lvgl_obj_move_background);

    REG_METHOD(L, "get_ptr", lvgl_obj_get_ptr);
    REG_METHOD(L, "get_x", lvgl_obj_get_x);
    REG_METHOD(L, "get_y", lvgl_obj_get_y);
    REG_METHOD(L, "get_width", lvgl_obj_get_width);
    REG_METHOD(L, "get_height", lvgl_obj_get_height);
    REG_METHOD(L, "get_pos", lvgl_obj_get_pos);
    REG_METHOD(L, "is_visible", lvgl_obj_is_visible);
    REG_METHOD(L, "is_clickable", lvgl_obj_is_clickable);
    REG_METHOD(L, "get_type", lvgl_obj_get_type);
    REG_METHOD(L, "has_state", lvgl_obj_has_state);
    REG_METHOD(L, "add_state", lvgl_obj_add_state);
    REG_METHOD(L, "clear_state", lvgl_obj_clear_state);

    REG_METHOD(L, "add_event_cb", lvgl_obj_add_event_cb);
    REG_METHOD(L, "remove_event_cb", lvgl_obj_remove_event_cb);
    REG_METHOD(L, "set_event_cb", lvgl_obj_set_event_cb);
    REG_METHOD(L, "on", lvgl_obj_on);
    REG_METHOD(L, "off", lvgl_obj_off);

    REG_METHOD(L, "scroll_to_x", lvgl_obj_scroll_to_x);
    REG_METHOD(L, "scroll_to_y", lvgl_obj_scroll_to_y);
    REG_METHOD(L, "scroll_to", lvgl_obj_scroll_to);
    REG_METHOD(L, "scroll_by", lvgl_obj_scroll_by);
    REG_METHOD(L, "scroll_to_view", lvgl_obj_scroll_to_view);
    REG_METHOD(L, "get_scroll_x", lvgl_obj_get_scroll_x);
    REG_METHOD(L, "get_scroll_y", lvgl_obj_get_scroll_y);
    REG_METHOD(L, "set_scrollbar_mode", lvgl_obj_set_scrollbar_mode);
    REG_METHOD(L, "set_scroll_dir", lvgl_obj_set_scroll_dir);

    obj_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_newtable(L);
    lua_rawgeti(L, LUA_REGISTRYINDEX, obj_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -5, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    return obj_metatable_ref;
}

int lvgl_get_obj_metatable_ref(void) {
    return obj_metatable_ref;
}

int lvgl_obj_create_instance(lua_State* L, lua_CFunction create_func, int metatable_ref) {
    /* 1. 调用 create_func 创建对象，push lightuserdata */
    create_func(L);
    /* 栈: [lightud] */

    /* 2. 取出指针，弹出 lightuserdata */
    lv_obj_t* obj = (lv_obj_t*)lua_touserdata(L, -1);
    lua_pop(L, 1);
    /* 栈: [] */

    /* 3. 创建 full userdata，存储 lv_obj_t* 指针 */
    lv_obj_t** ud = (lv_obj_t**)lua_newuserdata(L, sizeof(lv_obj_t*));
    *ud = obj;
    /* 栈: [ud] */

    if (metatable_ref == LUA_NOREF) {
        /* 仅使用基础 metatable */
        lua_rawgeti(L, LUA_REGISTRYINDEX, obj_metatable_ref);
        lua_setmetatable(L, -2);
        return 1;
    }

    /* 4. 创建专属 metatable（new_mt） */
    lua_newtable(L);
    /* 栈: [ud, new_mt] */

    /* 5. new_mt.__index = new_mt 自身（让方法通过 __index 查找） */
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    /* 栈: [ud, new_mt] */

    /* 6. 把 base_mt 的方法复制到 new_mt（继承基础对象方法） */
    lua_rawgeti(L, LUA_REGISTRYINDEX, obj_metatable_ref);
    /* 栈: [ud, new_mt, base_mt] */
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        /* 栈: [ud, new_mt, base_mt, key, value] */
        /* -1: value, -2: key, -3: base_mt, -4: new_mt */
        const char* key_str = lua_tostring(L, -2);
        if (key_str) {
            lua_setfield(L, -4, key_str);  /* new_mt[key_str] = value */
        } else {
            lua_pop(L, 1);
        }
        /* 栈: [ud, new_mt, base_mt, key] */
    }
    lua_pop(L, 1);  /* 弹出 base_mt */
    /* 栈: [ud, new_mt] */

    /* 7. 把 spec_mt 的方法复制到 new_mt（控件专属方法） */
    lua_rawgeti(L, LUA_REGISTRYINDEX, metatable_ref);
    /* 栈: [ud, new_mt, spec_mt] */
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        /* 栈: [ud, new_mt, spec_mt, key, value] */
        /* -1: value, -2: key, -3: spec_mt, -4: new_mt */
        const char* key_str = lua_tostring(L, -2);
        if (key_str) {
            lua_setfield(L, -4, key_str);  /* new_mt[key_str] = value */
        } else {
            lua_pop(L, 1);
        }
        /* 栈: [ud, new_mt, spec_mt, key] */
    }
    lua_pop(L, 1);  /* 弹出 spec_mt */
    /* 栈: [ud, new_mt] */

    /* 8. 给 ud 设置 metatable = new_mt */
    lua_setmetatable(L, -2);
    /* 栈: [ud] */

    return 1;
}

int lvgl_obj_delete(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_obj_del(obj);
    return 0;
}

int lvgl_obj_clean(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_obj_clean(obj);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_get_parent(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* parent = lv_obj_get_parent(obj);
    lua_pushlightuserdata(L, parent);
    return 1;
}

int lvgl_obj_get_child_cnt(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    uint32_t cnt = lv_obj_get_child_cnt(obj);
    lua_pushinteger(L, cnt);
    return 1;
}

int lvgl_obj_set_pos(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_obj_set_pos(obj, x, y);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_size(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t w = (int32_t)luaL_checkinteger(L, 2);
    int32_t h = (int32_t)luaL_checkinteger(L, 3);
    lv_obj_set_size(obj, w, h);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_width(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t w = (int32_t)luaL_checkinteger(L, 2);
    lv_obj_set_width(obj, w);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_height(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t h = (int32_t)luaL_checkinteger(L, 2);
    lv_obj_set_height(obj, h);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_x(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    lv_obj_set_x(obj, x);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_y(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t y = (int32_t)luaL_checkinteger(L, 2);
    lv_obj_set_y(obj, y);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_align(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_align_t align = (lv_align_t)luaL_checkinteger(L, 2);
    int32_t x_ofs = (int32_t)luaL_optinteger(L, 3, 0);
    int32_t y_ofs = (int32_t)luaL_optinteger(L, 4, 0);
    lv_obj_align(obj, align, x_ofs, y_ofs);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_align_to(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* ref = lvgl_get_obj_ptr(L, 2);
    lv_align_t align = (lv_align_t)luaL_checkinteger(L, 3);
    int32_t x_ofs = (int32_t)luaL_optinteger(L, 4, 0);
    int32_t y_ofs = (int32_t)luaL_optinteger(L, 5, 0);
    lv_obj_align_to(obj, ref, align, x_ofs, y_ofs);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_center(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_obj_center(obj);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_click(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    if (en) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    } else {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    }
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_hidden(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    bool en = lua_toboolean(L, 2);
    if (en) {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_user_data(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    if (lua_gettop(L) >= 2) {
        void* data = (void*)lua_touserdata(L, 2);
        lv_obj_set_user_data(obj, data);
    }
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_add_style(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_style_t* style = (lv_style_t*)luaL_checklightuserdata(L, 2);
    lv_part_t selector = (lv_part_t)luaL_optinteger(L, 3, LV_PART_MAIN);
    lv_obj_add_style(obj, style, selector);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_add_to(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_obj_t* parent = lvgl_get_obj_ptr(L, 2);
    lv_obj_set_parent(obj, parent);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_move_foreground(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_obj_move_foreground(obj);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_move_background(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_obj_move_background(obj);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_get_ptr(lua_State* L) {
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_get_x(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, lv_obj_get_x(obj));
    return 1;
}

int lvgl_obj_get_y(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, lv_obj_get_y(obj));
    return 1;
}

int lvgl_obj_get_width(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, lv_obj_get_width(obj));
    return 1;
}

int lvgl_obj_get_height(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, lv_obj_get_height(obj));
    return 1;
}

int lvgl_obj_get_pos(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, lv_obj_get_x(obj));
    lua_pushinteger(L, lv_obj_get_y(obj));
    return 2;
}

int lvgl_obj_is_visible(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushboolean(L, lv_obj_is_visible(obj));
    return 1;
}

int lvgl_obj_is_clickable(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushboolean(L, lv_obj_has_flag(obj, LV_OBJ_FLAG_CLICKABLE));
    return 1;
}

int lvgl_obj_has_state(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_state_t state = (lv_state_t)luaL_checkinteger(L, 2);
    lua_pushboolean(L, lv_obj_has_state(obj, state));
    return 1;
}

int lvgl_obj_add_state(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_state_t state = (lv_state_t)luaL_checkinteger(L, 2);
    lv_obj_add_state(obj, state);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_clear_state(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_state_t state = (lv_state_t)luaL_checkinteger(L, 2);
    lv_obj_clear_state(obj, state);
    lua_pushvalue(L, 1);
    return 1;
}

static const char* lvgl_obj_type_name(lv_obj_t* obj)
{
#define LVGL_IF_TYPE(cls, name) \
    if (lv_obj_check_type(obj, &cls)) return name;

    LVGL_IF_TYPE(lv_btn_class, "btn")
    LVGL_IF_TYPE(lv_label_class, "label")
    LVGL_IF_TYPE(lv_img_class, "img")
    LVGL_IF_TYPE(lv_line_class, "line")
    LVGL_IF_TYPE(lv_arc_class, "arc")
    LVGL_IF_TYPE(lv_bar_class, "bar")
    LVGL_IF_TYPE(lv_slider_class, "slider")
    LVGL_IF_TYPE(lv_switch_class, "switch")
    LVGL_IF_TYPE(lv_dropdown_class, "dropdown")
    LVGL_IF_TYPE(lv_roller_class, "roller")
    LVGL_IF_TYPE(lv_textarea_class, "textarea")
    LVGL_IF_TYPE(lv_chart_class, "chart")
    LVGL_IF_TYPE(lv_table_class, "table")
    LVGL_IF_TYPE(lv_meter_class, "meter")
    LVGL_IF_TYPE(lv_checkbox_class, "checkbox")
    LVGL_IF_TYPE(lv_canvas_class, "canvas")
    LVGL_IF_TYPE(lv_list_class, "list")
    LVGL_IF_TYPE(lv_keyboard_class, "keyboard")
    LVGL_IF_TYPE(lv_calendar_class, "calendar")
    LVGL_IF_TYPE(lv_msgbox_class, "msgbox")
    LVGL_IF_TYPE(lv_spinner_class, "spinner")
    LVGL_IF_TYPE(lv_obj_class, "obj")

#undef LVGL_IF_TYPE
    return "unknown";
}

int lvgl_obj_get_type(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushstring(L, lvgl_obj_type_name(obj));
    return 1;
}

/* ==================== 事件回调相关 ==================== */

/* 将事件名称字符串映射为LVGL事件码 */
static lv_event_code_t lvgl_event_name_to_code(const char* name) {
    if (!name) return LV_EVENT_ALL;

    /* 按压相关 */
    if (strcmp(name, "press") == 0) return LV_EVENT_PRESSED;
    if (strcmp(name, "pressing") == 0) return LV_EVENT_PRESSING;
    if (strcmp(name, "press_lost") == 0) return LV_EVENT_PRESS_LOST;
    if (strcmp(name, "click") == 0) return LV_EVENT_CLICKED;
    if (strcmp(name, "long_press") == 0) return LV_EVENT_LONG_PRESSED;
    if (strcmp(name, "long_press_repeat") == 0) return LV_EVENT_LONG_PRESSED_REPEAT;
    if (strcmp(name, "release") == 0) return LV_EVENT_RELEASED;

    /* 滚动相关 */
    if (strcmp(name, "scroll") == 0) return LV_EVENT_SCROLL;
    if (strcmp(name, "scroll_begin") == 0) return LV_EVENT_SCROLL_BEGIN;
    if (strcmp(name, "scroll_end") == 0) return LV_EVENT_SCROLL_END;

    /* 通用事件 */
    if (strcmp(name, "change") == 0) return LV_EVENT_VALUE_CHANGED;
    if (strcmp(name, "insert") == 0) return LV_EVENT_INSERT;
    if (strcmp(name, "refresh") == 0) return LV_EVENT_REFRESH;
    if (strcmp(name, "ready") == 0) return LV_EVENT_READY;
    if (strcmp(name, "cancel") == 0) return LV_EVENT_CANCEL;
    if (strcmp(name, "delete") == 0) return LV_EVENT_DELETE;
    if (strcmp(name, "resize") == 0) return LV_EVENT_SIZE_CHANGED;

    /* 焦点相关 */
    if (strcmp(name, "focus") == 0) return LV_EVENT_FOCUSED;
    if (strcmp(name, "defocus") == 0) return LV_EVENT_DEFOCUSED;
    if (strcmp(name, "leave") == 0) return LV_EVENT_LEAVE;

    /* 按键相关 */
    if (strcmp(name, "key") == 0) return LV_EVENT_KEY;
    if (strcmp(name, "enter") == 0) return LV_EVENT_KEY;
    if (strcmp(name, "focus_key") == 0) return LV_EVENT_FOCUS_KEY;

    /* 手势 */
    if (strcmp(name, "gesture") == 0) return LV_EVENT_GESTURE;

    /* 所有事件 */
    if (strcmp(name, "all") == 0) return LV_EVENT_ALL;

    return LV_EVENT_ALL;
}

static void lvgl_event_handler(lv_event_t* e) {
    void* ud = lv_event_get_user_data(e);
    if (!ud) return;

    params_t* params = params_create(4);
    if (!params) return;

    params_push_ptr(params, e);
    params_push_int(params, lv_event_get_code(e));
    params_push_ptr(params, lv_event_get_target(e));
    params_push_ptr(params, lv_event_get_current_target(e));

    iot_callback_call(ud, params);
}

int lvgl_obj_add_event_cb(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    if (!lua_isfunction(L, 2)) {
        luaL_error(L, "event callback must be a function");
        return 0;
    }

    lv_event_code_t event_code = (lv_event_code_t)luaL_optinteger(L, 3, LV_EVENT_ALL);

    void* ud = iot_callback_save(L, 2);
    if (!ud) {
        luaL_error(L, "failed to save callback");
        return 0;
    }

    lv_obj_add_event_cb(obj, lvgl_event_handler, event_code, ud);

    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_remove_event_cb(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    void* ud = (void*)lua_touserdata(L, 2);

    if (ud) {
        lv_obj_remove_event_cb_with_user_data(obj, lvgl_event_handler, ud);
        iot_callback_free(ud);
    } else {
        lv_obj_remove_event_cb(obj, lvgl_event_handler);
    }

    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_event_cb(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    if (!lua_isfunction(L, 2)) {
        luaL_error(L, "event callback must be a function");
        return 0;
    }

    void* old_ud = lv_obj_get_event_user_data(obj, lvgl_event_handler);
    if (old_ud) {
        lv_obj_remove_event_cb_with_user_data(obj, lvgl_event_handler, old_ud);
        iot_callback_free(old_ud);
    }

    void* ud = iot_callback_save(L, 2);
    if (!ud) {
        luaL_error(L, "failed to save callback");
        return 0;
    }

    lv_obj_add_event_cb(obj, lvgl_event_handler, LV_EVENT_ALL, ud);

    lua_pushvalue(L, 1);
    return 1;
}

/*
注册事件监听器(按事件名称)
@param self 对象实例或指针
@param event_name 事件名称: press/pressing/press_lost/click/long_press/long_press_repeat/release/scroll/scroll_begin/scroll_end/change/insert/refresh/ready/cancel/delete/resize/focus/defocus/leave/key/enter/focus_key/gesture/all
@param callback 回调函数 function(e, code, target, current_target)
@return lightuserdata callback_id(用于off移除回调)
@usage local id = btn:on("click", function(e, code) ... end)
*/
int lvgl_obj_on(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    const char* event_name = luaL_checkstring(L, 2);
    if (!lua_isfunction(L, 3)) {
        luaL_error(L, "event callback must be a function");
        return 0;
    }

    lv_event_code_t event_code = lvgl_event_name_to_code(event_name);

    void* ud = iot_callback_save(L, 3);
    if (!ud) {
        luaL_error(L, "failed to save callback");
        return 0;
    }

    lv_obj_add_event_cb(obj, lvgl_event_handler, event_code, ud);

    lua_pushlightuserdata(L, ud);
    return 1;
}

/*
移除事件监听器
@param self 对象实例或指针
@param callback_id on返回的callback_id(lightuserdata)
@return self
@usage btn:off(id)
*/
int lvgl_obj_off(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    void* ud = (void*)lua_touserdata(L, 2);

    if (ud) {
        lv_obj_remove_event_cb_with_user_data(obj, lvgl_event_handler, ud);
        iot_callback_free(ud);
    }

    lua_pushvalue(L, 1);
    return 1;
}

/* ==================== 滚动操作 ==================== */

int lvgl_obj_scroll_to_x(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    lv_obj_scroll_to_x(obj, x, LV_ANIM_ON);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_scroll_to_y(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t y = (int32_t)luaL_checkinteger(L, 2);
    lv_obj_scroll_to_y(obj, y, LV_ANIM_ON);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_scroll_to(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t x = (int32_t)luaL_checkinteger(L, 2);
    int32_t y = (int32_t)luaL_checkinteger(L, 3);
    lv_obj_scroll_to(obj, x, y, LV_ANIM_ON);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_scroll_by(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    int32_t dx = (int32_t)luaL_checkinteger(L, 2);
    int32_t dy = (int32_t)luaL_checkinteger(L, 3);
    lv_obj_scroll_by(obj, dx, dy, LV_ANIM_ON);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_scroll_to_view(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_anim_enable_t anim = (lv_anim_enable_t)luaL_optinteger(L, 2, LV_ANIM_ON);
    lv_obj_scroll_to_view(obj, anim);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_get_scroll_x(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, lv_obj_get_scroll_x(obj));
    return 1;
}

int lvgl_obj_get_scroll_y(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, lv_obj_get_scroll_y(obj));
    return 1;
}

int lvgl_obj_set_scrollbar_mode(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_scrollbar_mode_t mode = (lv_scrollbar_mode_t)luaL_checkinteger(L, 2);
    lv_obj_set_scrollbar_mode(obj, mode);
    lua_pushvalue(L, 1);
    return 1;
}

int lvgl_obj_set_scroll_dir(lua_State* L) {
    lv_obj_t* obj = lvgl_get_obj_ptr(L, 1);
    lv_dir_t dir = (lv_dir_t)luaL_checkinteger(L, 2);
    lv_obj_set_scroll_dir(obj, dir);
    lua_pushvalue(L, 1);
    return 1;
}