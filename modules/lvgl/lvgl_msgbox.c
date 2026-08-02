/**
 * @file iot_lvgl_msgbox.c
 * @brief LVGL消息框控件
 *
 * 实现LVGL消息框控件的OO风格Lua绑定，包括消息框创建、设置文本、添加按钮、关闭消息框等接口。
 * 适配 LVGL 9.5.0 API：使用 lv_msgbox_add_title/add_text/add_footer_button 等新接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"
#include <string.h>

/* msgbox组件的metatable引用 */
static int msgbox_metatable_ref = LUA_NOREF;

/* 用于保存 title/text label 指针到 user_data */
typedef struct {
    lv_obj_t* title;
    lv_obj_t* text;
    /* LVGL 9: 按钮是独立对象，需记录最后点击的按钮文本以兼容旧 API */
    char active_btn_text[64];
} iot_lvgl_msgbox_data_t;

static iot_lvgl_msgbox_data_t* iot_lvgl_msgbox_data_get(lv_obj_t* msgbox)
{
    return (iot_lvgl_msgbox_data_t*)lv_obj_get_user_data(msgbox);
}

static iot_lvgl_msgbox_data_t* iot_lvgl_msgbox_data_ensure(lv_obj_t* msgbox)
{
    iot_lvgl_msgbox_data_t* data = iot_lvgl_msgbox_data_get(msgbox);
    if (!data) {
        data = (iot_lvgl_msgbox_data_t*)cm_malloc(sizeof(iot_lvgl_msgbox_data_t));
        if (data) {
            memset(data, 0, sizeof(*data));
            lv_obj_set_user_data(msgbox, data);
        }
    }
    return data;
}

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_msgbox_create_internal(lua_State* L) {
    lv_obj_t* parent = NULL;
    if (!lua_isnoneornil(L, 1)) {
        parent = iot_lvgl_get_obj_ptr(L, 1);
    }
    /* LVGL 9: lv_msgbox_create 只接受 parent 参数 */
    lv_obj_t* msgbox = lv_msgbox_create(parent);
    lua_pushlightuserdata(L, msgbox);
    return 1;
}

/* ==================== 内部按钮点击转发 ==================== */

/* LVGL 9 按钮是独立对象，点击只发 LV_EVENT_CLICKED 给按钮自身。
 * 此回调将点击转发为 msgbox 的 LV_EVENT_VALUE_CHANGED，并记录按钮文本，
 * 以兼容 LVGL 8 的 msgbox 事件行为。 */
static void iot_lvgl_msgbox_btn_click_cb(lv_event_t* e) {
    lv_obj_t* btn = (lv_obj_t*)lv_event_get_target(e);
    lv_obj_t* msgbox = lv_obj_get_parent(lv_obj_get_parent(btn)); /* btn -> footer -> msgbox */
    if (!msgbox) return;

    iot_lvgl_msgbox_data_t* data = iot_lvgl_msgbox_data_get(msgbox);
    if (data) {
        /* 从按钮下的 label 读取文本 */
        uint32_t cnt = lv_obj_get_child_count(btn);
        for (uint32_t i = 0; i < cnt; i++) {
            lv_obj_t* child = lv_obj_get_child(btn, i);
            if (lv_obj_check_type(child, &lv_label_class)) {
                const char* txt = lv_label_get_text(child);
                if (txt) {
                    strncpy(data->active_btn_text, txt, sizeof(data->active_btn_text) - 1);
                    data->active_btn_text[sizeof(data->active_btn_text) - 1] = '\0';
                }
                break;
            }
        }
    }

    /* 向 msgbox 转发 VALUE_CHANGED 事件 */
    lv_obj_send_event(msgbox, LV_EVENT_VALUE_CHANGED, NULL);
}

/* ==================== 消息框OO方法 ==================== */

/*
创建消息框控件(OO风格)
@param self 父对象(可选)
@return userdata 带metatable的消息框实例
@usage local msgbox = lvgl.msgbox.create(nil)
*/
static int iot_lvgl_msgbox_create(lua_State* L) {
    return iot_lvgl_obj_create_instance(L, iot_lvgl_msgbox_create_internal, msgbox_metatable_ref);
}

/*
设置消息框标题
@param self 消息框实例或指针
@param title 标题文本
@return self
@usage msgbox:set_title("提示")
*/
static int iot_lvgl_msgbox_set_title(lua_State* L) {
    lv_obj_t* msgbox = iot_lvgl_get_obj_ptr(L, 1);
    const char* title = luaL_checkstring(L, 2);
    iot_lvgl_msgbox_data_t* data = iot_lvgl_msgbox_data_ensure(msgbox);
    if (data) {
        if (!data->title) {
            /* 首次设置：创建标题 */
            data->title = lv_msgbox_add_title(msgbox, title);
        } else {
            /* 更新已有标题 */
            lv_label_set_text(data->title, title);
        }
    }
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
static int iot_lvgl_msgbox_set_text(lua_State* L) {
    lv_obj_t* msgbox = iot_lvgl_get_obj_ptr(L, 1);
    const char* text = luaL_checkstring(L, 2);
    iot_lvgl_msgbox_data_t* data = iot_lvgl_msgbox_data_ensure(msgbox);
    if (data) {
        if (!data->text) {
            /* 首次设置：创建文本 */
            data->text = lv_msgbox_add_text(msgbox, text);
        } else {
            /* 更新已有文本 */
            lv_label_set_text(data->text, text);
        }
    }
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
static int iot_lvgl_msgbox_add_button(lua_State* L) {
    lv_obj_t* msgbox = iot_lvgl_get_obj_ptr(L, 1);
    const char* txt = luaL_checkstring(L, 2);
    /* LVGL 9: 使用 lv_msgbox_add_footer_button 添加底部按钮 */
    lv_obj_t* btn = lv_msgbox_add_footer_button(msgbox, txt);
    /* 注册点击转发：按钮点击时向 msgbox 发送 VALUE_CHANGED 事件 */
    if (btn) {
        lv_obj_add_event_cb(btn, iot_lvgl_msgbox_btn_click_cb, LV_EVENT_CLICKED, NULL);
    }
    lua_pushvalue(L, 1);
    return 1;
}

/*
获取激活的按钮索引
@param self 消息框实例或指针
@return integer 按钮索引
@usage local btn_index = msgbox:get_active_btn()
*/
static int iot_lvgl_msgbox_get_active_btn(lua_State* L) {
    /* LVGL 9 移除了 lv_msgbox_get_active_btn 接口，兼容返回 0 */
    (void)iot_lvgl_get_obj_ptr(L, 1);
    lua_pushinteger(L, 0);
    return 1;
}

/*
获取激活的按钮文本
@param self 消息框实例或指针
@return string 按钮文本
@usage local btn_text = msgbox:get_active_btn_text()
*/
static int iot_lvgl_msgbox_get_active_btn_text(lua_State* L) {
    lv_obj_t* msgbox = iot_lvgl_get_obj_ptr(L, 1);
    iot_lvgl_msgbox_data_t* data = iot_lvgl_msgbox_data_get(msgbox);
    if (data && data->active_btn_text[0]) {
        lua_pushstring(L, data->active_btn_text);
    } else {
        lua_pushnil(L);
    }
    return 1;
}

/*
关闭消息框
@param self 消息框实例或指针
@return self
@usage msgbox:close()
*/
static int iot_lvgl_msgbox_close(lua_State* L) {
    lv_obj_t* msgbox = iot_lvgl_get_obj_ptr(L, 1);
    /* 释放保存的数据 */
    iot_lvgl_msgbox_data_t* data = iot_lvgl_msgbox_data_get(msgbox);
    if (data) {
        cm_free(data);
        lv_obj_set_user_data(msgbox, NULL);
    }
    lv_msgbox_close(msgbox);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 msgbox 子模块 */
void iot_lvgl_register_msgbox(lua_State* L) {
    lua_newtable(L);

    REG_METHOD(L, "set_title", iot_lvgl_msgbox_set_title);
    REG_METHOD(L, "set_text", iot_lvgl_msgbox_set_text);
    REG_METHOD(L, "add_button", iot_lvgl_msgbox_add_button);
    REG_METHOD(L, "get_active_btn", iot_lvgl_msgbox_get_active_btn);
    REG_METHOD(L, "get_active_btn_text", iot_lvgl_msgbox_get_active_btn_text);
    REG_METHOD(L, "close", iot_lvgl_msgbox_close);

    msgbox_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    lua_rawgeti(L, LUA_REGISTRYINDEX, msgbox_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    REG_METHOD(L, "create", iot_lvgl_msgbox_create);
}
