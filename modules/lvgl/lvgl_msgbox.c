/**
 * @file iot_lvgl_msgbox.c
 * @brief LVGL消息框控件
 *
 * 实现LVGL消息框控件的OO风格Lua绑定，包括消息框创建、设置文本、添加按钮、设置尺寸、获取活动按钮索引/文本、关闭消息框等接口。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.06.10
 */

#include "lvgl_port.h"
#include "lvgl_obj.h"
#include <string.h>

/* msgbox组件的metatable引用 */
static int msgbox_metatable_ref = LUA_NOREF;

typedef struct {
    char** texts;
    char** map;
    uint32_t count;
} iot_lvgl_msgbox_btns_t;

static void iot_lvgl_msgbox_btns_free(iot_lvgl_msgbox_btns_t* btns)
{
    uint32_t i;

    if (!btns) {
        return;
    }
    for (i = 0; i < btns->count; i++) {
        cm_free(btns->texts[i]);
    }
    cm_free(btns->texts);
    cm_free(btns->map);
    cm_free(btns);
}

static iot_lvgl_msgbox_btns_t* iot_lvgl_msgbox_btns_get(lv_obj_t* msgbox)
{
    return (iot_lvgl_msgbox_btns_t*)lv_obj_get_user_data(msgbox);
}

static char* iot_lvgl_msgbox_strdup(const char* src)
{
    size_t len = strlen(src) + 1;
    char* dst = (char*)cm_malloc(len);
    if (dst) {
        memcpy(dst, src, len);
    }
    return dst;
}

static lv_obj_t* iot_lvgl_msgbox_ensure_title(lv_obj_t* msgbox)
{
    lv_obj_t* title = lv_msgbox_get_title(msgbox);
    if (title) {
        return title;
    }

    lv_msgbox_t* mbox = (lv_msgbox_t*)msgbox;
    mbox->title = lv_label_create(msgbox);
    lv_label_set_long_mode(mbox->title, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_width(mbox->title, LV_PCT(100));
    return mbox->title;
}

static lv_obj_t* iot_lvgl_msgbox_ensure_text(lv_obj_t* msgbox)
{
    lv_obj_t* text = lv_msgbox_get_text(msgbox);
    if (text) {
        return text;
    }

    lv_msgbox_t* mbox = (lv_msgbox_t*)msgbox;
    if (!mbox->content) {
        mbox->content = lv_obj_class_create_obj(&lv_msgbox_content_class, msgbox);
        lv_obj_class_init_obj(mbox->content);
    }
    mbox->text = lv_label_create(mbox->content);
    lv_label_set_long_mode(mbox->text, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(mbox->text, lv_pct(100));
    return mbox->text;
}

static void iot_lvgl_msgbox_refresh_btns(lv_obj_t* msgbox, iot_lvgl_msgbox_btns_t* store)
{
    lv_msgbox_t* mbox = (lv_msgbox_t*)msgbox;

    if (!store || store->count == 0) {
        return;
    }

    if (!mbox->btns) {
        mbox->btns = lv_btnmatrix_create(msgbox);
        lv_btnmatrix_set_btn_ctrl_all(mbox->btns,
            LV_BTNMATRIX_CTRL_CLICK_TRIG | LV_BTNMATRIX_CTRL_NO_REPEAT);
        lv_obj_set_style_max_width(mbox->btns, lv_pct(100), 0);
        lv_obj_add_flag(mbox->btns, LV_OBJ_FLAG_EVENT_BUBBLE);
    }

    lv_btnmatrix_set_map(mbox->btns, (const char**)store->map);

    const lv_font_t* font = lv_obj_get_style_text_font(mbox->btns, LV_PART_ITEMS);
    lv_coord_t btn_h = lv_font_get_line_height(font) + LV_DPI_DEF / 10;
    lv_obj_set_size(mbox->btns, store->count * (2 * LV_DPI_DEF / 3), btn_h);
}

/* ==================== 内部创建函数 ==================== */

static int iot_lvgl_msgbox_create_internal(lua_State* L) {
    lv_obj_t* parent = NULL;
    if (!lua_isnoneornil(L, 1)) {
        parent = iot_lvgl_get_obj_ptr(L, 1);
    }
    lv_obj_t* msgbox = lv_msgbox_create(parent, "", "", NULL, false);
    lua_pushlightuserdata(L, msgbox);
    return 1;
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
    lv_label_set_text(iot_lvgl_msgbox_ensure_title(msgbox), title);
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
    lv_label_set_text(iot_lvgl_msgbox_ensure_text(msgbox), text);
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
    iot_lvgl_msgbox_btns_t* old = iot_lvgl_msgbox_btns_get(msgbox);
    iot_lvgl_msgbox_btns_t* store = (iot_lvgl_msgbox_btns_t*)cm_malloc(sizeof(iot_lvgl_msgbox_btns_t));
    uint32_t i;

    if (!store) {
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    store->count = (old ? old->count : 0) + 1;
    store->texts = (char**)cm_malloc(sizeof(char*) * store->count);
    store->map = (char**)cm_malloc(sizeof(char*) * (store->count + 1));
    if (!store->texts || !store->map) {
        cm_free(store->texts);
        cm_free(store->map);
        cm_free(store);
        luaL_error(L, "memory allocation failed");
        return 0;
    }

    for (i = 0; i < store->count - 1; i++) {
        store->texts[i] = iot_lvgl_msgbox_strdup(old->texts[i]);
        store->map[i] = store->texts[i];
    }
    store->texts[store->count - 1] = iot_lvgl_msgbox_strdup(txt);
    store->map[store->count - 1] = store->texts[store->count - 1];
    store->map[store->count] = (char*)"";

    iot_lvgl_msgbox_refresh_btns(msgbox, store);
    iot_lvgl_msgbox_btns_free(old);
    lv_obj_set_user_data(msgbox, store);

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
    lv_obj_t* msgbox = iot_lvgl_get_obj_ptr(L, 1);
    int16_t btn = lv_msgbox_get_active_btn(msgbox);
    lua_pushinteger(L, btn);
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
    const char* txt = lv_msgbox_get_active_btn_text(msgbox);
    lua_pushstring(L, txt);
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
    iot_lvgl_msgbox_btns_free(iot_lvgl_msgbox_btns_get(msgbox));
    lv_obj_set_user_data(msgbox, NULL);
    lv_msgbox_close(msgbox);
    lua_pushvalue(L, 1);
    return 1;
}

/* 注册 msgbox 子模块 */
void iot_lvgl_register_msgbox(lua_State* L) {
    /* 创建组件方法表用于metatable继承) */
    lua_newtable(L);

    /* 注册OO风格方法 */
    REG_METHOD(L, "set_title", iot_lvgl_msgbox_set_title);
    REG_METHOD(L, "set_text", iot_lvgl_msgbox_set_text);
    REG_METHOD(L, "add_button", iot_lvgl_msgbox_add_button);
    REG_METHOD(L, "get_active_btn", iot_lvgl_msgbox_get_active_btn);
    REG_METHOD(L, "get_active_btn_text", iot_lvgl_msgbox_get_active_btn_text);
    REG_METHOD(L, "close", iot_lvgl_msgbox_close);

    /* 保存组件metatable引用(用于继承) */
    msgbox_metatable_ref = luaL_ref(L, LUA_REGISTRYINDEX);

    /* 将方法复制到组件子表(支持 lvgl.msgbox.set_title(mb, ...) 调用) */
    lua_rawgeti(L, LUA_REGISTRYINDEX, msgbox_metatable_ref);
    lua_pushnil(L);
    while (lua_next(L, -2) != 0) {
        lua_pushvalue(L, -2);
        lua_insert(L, -2);
        lua_setfield(L, -4, lua_tostring(L, -2));
        lua_pop(L, 1);
    }
    lua_pop(L, 1);

    /* 注册create函数到主表lvgl.msgbox) */
    REG_METHOD(L, "create", iot_lvgl_msgbox_create);
}
