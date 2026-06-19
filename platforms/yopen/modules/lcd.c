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

-- 填充屏幕
lcd.fill(0, 0, 240, 320, lcd.color565(255, 255, 0))

-- 显示文字
lcd.drawText(10, 10, "Hello", lcd.color565(0, 0, 0))

-- 刷新显示
lcd.flush()
*/

#include "module.h"
#include "yopen_lcd.h"
#include <string.h>

/* LCD配置 */
typedef struct {
    yopen_lcd_config_s config;
    int initialized;
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

    memset(handle, 0, sizeof(lcd_handle_t));

    /* 设置默认配置 */
    handle->config.bpp = 16;       /* RGB565 */
    handle->config.width = 240;
    handle->config.height = 320;
    handle->config.xoffset = 0;
    handle->config.yoffset = 0;
    handle->config.freq = 20000000;
    handle->config.interfaceType = SPI_4W_II;

    /* 解析配置参数 */
    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "w");
        if (lua_isnumber(L, -1)) handle->config.width = (uint16_t)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "h");
        if (lua_isnumber(L, -1)) handle->config.height = (uint16_t)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    /* 初始化LCD */
    if (yopen_lcd_init(&handle->config) == YOPEN_LCD_SUCCESS) {
        handle->initialized = 1;
        g_default_lcd = handle;
        lua_pushboolean(L, 1);
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
    /* LCD窗口设置通常通过发送命令实现 */
    /* 此处简化处理，实际实现需要调用 yopen_lcd_cmd_data_send */
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int w = (int)luaL_checkinteger(L, 3);
    int h = (int)luaL_checkinteger(L, 4);

    lua_pushboolean(L, 1);
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
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int color = (int)luaL_checkinteger(L, 3);

    /* TODO: 实现像素绘制 - 需要填充单点数据并更新 */
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
    /* TODO: 实现直线绘制算法(Bresenham) */
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
    /* TODO: 实现矩形绘制 */
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
    int x = (int)luaL_checkinteger(L, 1);
    int y = (int)luaL_checkinteger(L, 2);
    int w = (int)luaL_checkinteger(L, 3);
    int h = (int)luaL_checkinteger(L, 4);
    int color = (int)luaL_checkinteger(L, 5);

    /* TODO: 实现填充 - 创建颜色数组并调用 yopen_lcd_update */
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
    /* TODO: 实现文字显示 - 需要字库支持 */
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 刷新显示
 * @api lcd.flush()
 * @return bool true表示成功
 */
static int luaopen_lcd_flush(lua_State* L) {
    /* LCD刷新通常自动完成或通过 yopen_lcd_update 触发 */
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 关闭LCD
 * @api lcd.close()
 * @return nil
 */
static int luaopen_lcd_close(lua_State* L) {
    if (g_default_lcd) {
        iot_free(g_default_lcd);
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