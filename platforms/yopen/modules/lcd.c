/*
@module  lcd
@summary LCD显示驱动
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     LCD
*/

/*
LCD参考示例
-- 初始化LCD
lcd.init(0, {w=240, h=320})

-- 设置窗口
lcd.setWindow(0, 0, 240, 320)

-- 绘制像素
lcd.drawPixel(10, 10, lcd.color565(255, 0, 0))

-- 绘制线
lcd.drawLine(0, 0, 100, 100, lcd.color565(0, 255, 0))

-- 绘制矩形
lcd.drawRect(10, 10, 100, 50, lcd.color565(0, 0, 255))

-- 填充屏幕
lcd.fill(0, 0, 240, 320, lcd.color565(255, 255, 0))

-- 显示文字
lcd.drawText(10, 10, "Hello", lcd.color565(0, 0, 0))

-- 刷新显示
lcd.flush()
*/

#include "module.h"
#include "yopen_lcd.h"

/* LCD句柄 */
typedef struct {
    yopen_lcd_h handle;
    int width;
    int height;
} lcd_handle_t;

/* 默认LCD句柄 */
static lcd_handle_t* g_default_lcd = NULL;

/**
 * @brief 初始化LCD
 * @api lcd.init(id, config)
 * @int id LCD ID (通常为0)
 * @table config 配置参数 {w=宽度, h=高度}
 * @return bool true表示成功
 */
static int luaopen_lcd_init(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);

    lcd_handle_t* handle = (lcd_handle_t*)iot_malloc(sizeof(lcd_handle_t));
    if (!handle) {
        lua_pushnil(L);
        return 1;
    }

    handle->width = 240;
    handle->height = 320;

    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "w");
        if (lua_isnumber(L, -1)) handle->width = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "h");
        if (lua_isnumber(L, -1)) handle->height = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    handle->handle = yopen_lcd_create(id);
    if (handle->handle != 0) {
        yopen_lcd_init(handle->handle);
        g_default_lcd = handle;
        lua_pushlightuserdata(L, handle);
    } else {
        iot_free(handle);
        lua_pushnil(L);
    }
    return 1;
}

/**
 * @brief 设置显示窗口
 * @api lcd.setWindow(x, y, w, h)
 * @int x 起始X坐标
 * @int y 起始Y坐标
 * @int w 宽度
 * @int h 高度
 * @return bool true表示成功
 */
static int luaopen_lcd_setwindow(lua_State* L) {
    lcd_handle_t* handle = g_default_lcd;

    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int w = (int)luaL_checkinteger(L, 3);
    int h = (int)luaL_checkinteger(L, 4);

    int ret = yopen_lcd_set_window(handle->handle, x, y, w, h);

    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 绘制像素点
 * @api lcd.drawPixel(x, y, color)
 * @int x X坐标
 * @int y Y坐标
 * @int color 颜色值 (RGB565)
 * @return bool true表示成功
 */
static int luaopen_lcd_drawpixel(lua_State* L) {
    lcd_handle_t* handle = g_default_lcd;

    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int color = (int)luaL_checkinteger(L, 3);

    yopen_lcd_draw_point(handle->handle, x, y, color);

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 绘制直线
 * @api lcd.drawLine(x1, y1, x2, y2, color)
 * @int x1 起点X
 * @int y1 起点Y
 * @int x2 终点X
 * @int y2 终点Y
 * @int color 颜色值
 * @return bool true表示成功
 */
static int luaopen_lcd_drawline(lua_State* L) {
    lcd_handle_t* handle = g_default_lcd;

    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int x1 = (int)luaL_checkinteger(L, 1);
    int y1 = (int)luaL_checkinteger(L, 2);
    int x2 = (int)luaL_checkinteger(L, 3);
    int y2 = (int)luaL_checkinteger(L, 4);
    int color = (int)luaL_checkinteger(L, 5);

    yopen_lcd_draw_line(handle->handle, x1, y1, x2, y2, color);

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 绘制矩形
 * @api lcd.drawRect(x, y, w, h, color)
 * @int x 起始X
 * @int y 起始Y
 * @int w 宽度
 * @int h 高度
 * @int color 颜色值
 * @return bool true表示成功
 */
static int luaopen_lcd_drawrect(lua_State* L) {
    lcd_handle_t* handle = g_default_lcd;

    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int w = (int)luaL_checkinteger(L, 3);
    int h = (int)luaL_checkinteger(L, 4);
    int color = (int)luaL_checkinteger(L, 5);

    yopen_lcd_draw_rectangle(handle->handle, x, y, w, h, color);

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 填充区域
 * @api lcd.fill(x, y, w, h, color)
 * @int x 起始X
 * @int y 起始Y
 * @int w 宽度
 * @int h 高度
 * @int color 颜色值
 * @return bool true表示成功
 */
static int luaopen_lcd_fill(lua_State* L) {
    lcd_handle_t* handle = g_default_lcd;

    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int w = (int)luaL_checkinteger(L, 3);
    int h = (int)luaL_checkinteger(L, 4);
    int color = (int)luaL_checkinteger(L, 5);

    yopen_lcd_fill(handle->handle, x, y, w, h, color);

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 显示文字
 * @api lcd.drawText(x, y, text, color)
 * @int x X坐标
 * @int y Y坐标
 * @string text 文字内容
 * @int color 颜色值
 * @return bool true表示成功
 */
static int luaopen_lcd_drawtext(lua_State* L) {
    lcd_handle_t* handle = g_default_lcd;

    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    const char* text = luaL_checkstring(L, 3);
    int color = (int)luaL_checkinteger(L, 4);

    yopen_lcd_draw_string(handle->handle, x, y, (char*)text, color);

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 刷新显示
 * @api lcd.flush()
 * @return bool true表示成功
 */
static int luaopen_lcd_flush(lua_State* L) {
    lcd_handle_t* handle = g_default_lcd;

    if (!handle) {
        lua_pushboolean(L, 0);
        return 1;
    }

    yopen_lcd_flush(handle->handle);

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 关闭LCD
 * @api lcd.close()
 * @return nil
 */
static int luaopen_lcd_close(lua_State* L) {
    lcd_handle_t* handle = g_default_lcd;

    if (handle) {
        yopen_lcd_close(handle->handle);
        iot_free(handle);
        g_default_lcd = NULL;
    }
    return 0;
}

/**
 * @brief RGB转换为RGB565颜色值
 * @api lcd.color565(r, g, b)
 * @int r 红色 (0-255)
 * @int g 绿色 (0-255)
 * @int b 蓝色 (0-255)
 * @return int RGB565颜色值
 */
static int luaopen_lcd_color565(lua_State* L) {
    int r = (int)luaL_checkinteger(L, 1);
    int g = (int)luaL_checkinteger(L, 2);
    int b = (int)luaL_checkinteger(L, 3);

    r = (r >> 3) & 0x1F;
    g = (g >> 2) & 0x3F;
    b = (b >> 3) & 0x1F;

    int color = (r << 11) | (g << 5) | b;

    lua_pushinteger(L, color);
    return 1;
}

static const luaL_Reg luaopen_lcd_funcs[] = {
    {"init",      luaopen_lcd_init},
    {"setWindow", luaopen_lcd_setwindow},
    {"drawPixel", luaopen_lcd_drawpixel},
    {"drawLine",  luaopen_lcd_drawline},
    {"drawRect",  luaopen_lcd_drawrect},
    {"fill",      luaopen_lcd_fill},
    {"drawText",  luaopen_lcd_drawtext},
    {"flush",     luaopen_lcd_flush},
    {"close",     luaopen_lcd_close},
    {"color565",  luaopen_lcd_color565},
    {NULL, NULL}
};

int luaopen_lcd(lua_State* L) {
    luaL_newlib(L, luaopen_lcd_funcs);
    return 1;
}