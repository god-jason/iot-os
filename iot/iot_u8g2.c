/*
@module  u8g2
@summary 轻量级图形库封装
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     U8G2
*/

/*
U8G2参考示例
local u8g2 = require("u8g2")

-- 初始化（需要先初始化LCD）
u8g2.setup(lcd)

-- 设置字体
u8g2.setfont(u8g2.font_6x10_tf)  -- 内置字体

-- 清除屏幕
u8g2.clearBuffer()

-- 绘制文字
u8g2.drawStr(0, 10, "Hello World!")

-- 绘制像素
u8g2.drawPixel(50, 50)

-- 绘制线条
u8g2.drawLine(0, 0, 100, 100)

-- 绘制矩形
u8g2.drawBox(10, 10, 50, 30)
u8g2.drawFrame(10, 10, 50, 30)  -- 空心矩形

-- 绘制圆形
u8g2.drawCircle(64, 32, 20, u8g2.DRAW_UPPER_RIGHT)

-- 发送缓冲区到LCD
u8g2.sendBuffer()

-- 获取屏幕尺寸
local w, h = u8g2.getDisplayWidth(), u8g2.getDisplayHeight()
*/

#include "lua.h"
#include "lauxlib.h"
#include "iot_base.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

/****************************************************************************
 * Types and Constants
 ****************************************************************************/

#define U8G2_MAX_WIDTH    240
#define U8G2_MAX_HEIGHT   320
#define U8G2_MAX_BPP      2   /* 最大2bpp，16级灰度 */

typedef struct {
    uint8_t* buffer;          /* 帧缓冲 */
    int width;                 /* 屏幕宽度 */
    int height;                /* 屏幕高度 */
    int bpp;                   /* 每像素位数 */
    int page_height;           /* 页高度(SSD1306等OLED常用) */
    int current_page;          /* 当前页 */
    uint8_t color;             /* 当前颜色 */
    void* lcd_handle;          /* LCD设备句柄 */
} u8g2_device_t;

static u8g2_device_t g_u8g2 = {0};

/* 绘制方向 */
enum {
    U8G2_DRAW_UPPER_RIGHT = 1,
    U8G2_DRAW_UPPER_LEFT = 2,
    U8G2_DRAW_LOWER_RIGHT = 4,
    U8G2_DRAW_LOWER_LEFT = 8,
    U8G2_DRAW_ALL = 15,
};

/****************************************************************************
 * Internal Functions
 ****************************************************************************/

/**
 * @brief 设置像素点
 */
static void set_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= g_u8g2.width || y < 0 || y >= g_u8g2.height) {
        return;
    }
    
    int idx;
    uint8_t mask;
    
    if (g_u8g2.bpp == 1) {
        /* 单色模式 */
        idx = y * ((g_u8g2.width + 7) / 8) + (x / 8);
        mask = 0x80 >> (x % 8);
        if (color) {
            g_u8g2.buffer[idx] |= mask;
        } else {
            g_u8g2.buffer[idx] &= ~mask;
        }
    } else if (g_u8g2.bpp == 2) {
        /* 4级灰度模式 */
        idx = y * ((g_u8g2.width + 3) / 4) + (x / 4);
        mask = (3 << ((x % 4) * 2));
        g_u8g2.buffer[idx] = (g_u8g2.buffer[idx] & ~mask) | (color & 3);
    }
}

/**
 * @brief 获取像素点
 */
static uint8_t get_pixel(int x, int y) {
    if (x < 0 || x >= g_u8g2.width || y < 0 || y >= g_u8g2.height) {
        return 0;
    }
    
    int idx;
    uint8_t mask;
    
    if (g_u8g2.bpp == 1) {
        idx = y * ((g_u8g2.width + 7) / 8) + (x / 8);
        mask = 0x80 >> (x % 8);
        return (g_u8g2.buffer[idx] & mask) ? 1 : 0;
    } else if (g_u8g2.bpp == 2) {
        idx = y * ((g_u8g2.width + 3) / 4) + (x / 4);
        mask = (3 << ((x % 4) * 2));
        return (g_u8g2.buffer[idx] & mask) >> ((x % 4) * 2);
    }
    
    return 0;
}

/**
 * @brief 绘制水平线（使用Bresenham算法优化）
 */
static void draw_horizontal_line(int x, int y, int w) {
    if (w < 0) {
        x += w;
        w = -w;
    }
    for (int i = 0; i < w; i++) {
        set_pixel(x + i, y, g_u8g2.color);
    }
}

/**
 * @brief 绘制垂直线
 */
static void draw_vertical_line(int x, int y, int h) {
    if (h < 0) {
        y += h;
        h = -h;
    }
    for (int i = 0; i < h; i++) {
        set_pixel(x, y + i, g_u8g2.color);
    }
}

/**
 * @brief 绘制直线（Bresenham算法）
 */
static void draw_line(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (1) {
        set_pixel(x1, y1, g_u8g2.color);
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/**
 * @brief 绘制空心圆（Bresenham算法）
 */
static void draw_circle(int x0, int y0, int r, int options) {
    int x = r;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        if (options & U8G2_DRAW_UPPER_RIGHT) set_pixel(x0 + x, y0 - y, g_u8g2.color);
        if (options & U8G2_DRAW_UPPER_LEFT)  set_pixel(x0 - x, y0 - y, g_u8g2.color);
        if (options & U8G2_DRAW_LOWER_RIGHT) set_pixel(x0 + x, y0 + y, g_u8g2.color);
        if (options & U8G2_DRAW_LOWER_LEFT)  set_pixel(x0 - x, y0 + y, g_u8g2.color);
        
        if (options & U8G2_DRAW_UPPER_RIGHT) set_pixel(x0 + y, y0 - x, g_u8g2.color);
        if (options & U8G2_DRAW_UPPER_LEFT)  set_pixel(x0 - y, y0 - x, g_u8g2.color);
        if (options & U8G2_DRAW_LOWER_RIGHT) set_pixel(x0 + y, y0 + x, g_u8g2.color);
        if (options & U8G2_DRAW_LOWER_LEFT)  set_pixel(x0 - y, y0 + x, g_u8g2.color);
        
        y++;
        err += 1 + 2 * y;
        if (2 * (err - x) + 1 > 0) {
            x--;
            err += 1 - 2 * x;
        }
    }
}

/**
 * @brief 绘制实心圆
 */
static void draw_filled_circle(int x0, int y0, int r, int options) {
    int x = r;
    int y = 0;
    int err = 0;
    
    while (x >= y) {
        if (options & U8G2_DRAW_UPPER_RIGHT) draw_horizontal_line(x0 - x, y0 - y, 2 * x);
        if (options & U8G2_DRAW_UPPER_LEFT)  draw_horizontal_line(x0 - x, y0 - y, 2 * x);
        if (options & U8G2_DRAW_LOWER_RIGHT) draw_horizontal_line(x0 - x, y0 + y, 2 * x);
        if (options & U8G2_DRAW_LOWER_LEFT)  draw_horizontal_line(x0 - x, y0 + y, 2 * x);
        
        if (options & U8G2_DRAW_UPPER_RIGHT) draw_horizontal_line(x0 - y, y0 - x, 2 * y);
        if (options & U8G2_DRAW_UPPER_LEFT)  draw_horizontal_line(x0 - y, y0 - x, 2 * y);
        if (options & U8G2_DRAW_LOWER_RIGHT) draw_horizontal_line(x0 - y, y0 + x, 2 * y);
        if (options & U8G2_DRAW_LOWER_LEFT)  draw_horizontal_line(x0 - y, y0 + x, 2 * y);
        
        y++;
        err += 1 + 2 * y;
        if (2 * (err - x) + 1 > 0) {
            x--;
            err += 1 - 2 * x;
        }
    }
}

/**
 * @brief UTF-8转Unicode
 */
static uint32_t utf8_to_unicode(const char** utf8) {
    const unsigned char* p = (const unsigned char*)*utf8;
    uint32_t unicode;
    
    if (*p < 0x80) {
        unicode = *p;
        (*utf8)++;
    } else if ((*p & 0xE0) == 0xC0) {
        unicode = ((*p & 0x1F) << 6) | (p[1] & 0x3F);
        (*utf8) += 2;
    } else if ((*p & 0xF0) == 0xE0) {
        unicode = ((*p & 0x0F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F);
        (*utf8) += 3;
    } else {
        unicode = *p;
        (*utf8)++;
    }
    
    return unicode;
}

/* 内置6x10 ASCII字体数据（简化版） */
static const uint8_t font_6x10[] = {
    /* 空格 */ 0x00,0x00,0x00,0x00,0x00,
    /* ! */    0x5F,0x00,
    /* " */    0x07,0x00,
    /* # */    0x7F,0x14,0x7F,0x14,0x7F,
    /* $ */    0x24,0x2E,0x2A,0x6B,0x00,
    /* % */    0x26,0x31,0x0D,0x26,0x00,
    /* & */    0x32,0x0F,0x0A,0x74,0x00,
    /* ' */    0x40,0x00,
    /* ( */    0x3E,0x41,0x00,
    /* ) */    0x41,0x3E,0x00,
    /* * */    0x22,0x1C,0x22,0x00,
    /* + */    0x14,0x7F,0x14,0x00,
    /* , */    0x40,0x00,
    /* - */    0x40,0x00,
    /* . */    0x60,0x00,
    /* / */    0x61,0x16,0x08,0x00,
    /* 0 */    0x3E,0x51,0x49,0x3E,0x00,
    /* 1 */    0x42,0x7F,0x40,0x00,
    /* 2 */    0x42,0x61,0x51,0x4E,0x00,
    /* 3 */    0x21,0x41,0x45,0x4B,0x00,
    /* 4 */    0x1F,0x12,0x7F,0x10,0x00,
    /* 5 */    0x27,0x45,0x45,0x39,0x00,
    /* 6 */    0x3C,0x4A,0x49,0x30,0x00,
    /* 7 */    0x01,0x71,0x09,0x07,0x00,
    /* 8 */    0x36,0x49,0x49,0x36,0x00,
    /* 9 */    0x06,0x49,0x29,0x1E,0x00,
    /* : */    0x36,0x00,
    /* ; */    0x36,0x00,
    /* < */    0x18,0x24,0x00,
    /* = */    0x7F,0x00,
    /* > */    0x24,0x18,0x00,
    /* ? */    0x02,0x01,0x51,0x09,0x06,0x00,
    /* @ */    0x3E,0x41,0x5D,0x55,0x1E,0x00,
    /* A */    0x7E,0x11,0x11,0x11,0x7E,0x00,
    /* B */    0x7F,0x49,0x49,0x36,0x00,
    /* C */    0x3E,0x41,0x41,0x22,0x00,
    /* D */    0x7F,0x41,0x41,0x3E,0x00,
    /* E */    0x7F,0x49,0x49,0x41,0x00,
    /* F */    0x7F,0x09,0x09,0x01,0x00,
    /* G */    0x3E,0x41,0x49,0x7A,0x00,
    /* H */    0x7F,0x08,0x08,0x7F,0x00,
    /* I */    0x41,0x7F,0x41,0x00,
    /* J */    0x20,0x40,0x41,0x3F,0x00,
    /* K */    0x7F,0x08,0x14,0x22,0x41,0x00,
    /* L */    0x7F,0x01,0x01,0x01,0x00,
    /* M */    0x7F,0x40,0x20,0x40,0x7F,0x00,
    /* N */    0x7F,0x20,0x10,0x08,0x7F,0x00,
    /* O */    0x3E,0x41,0x41,0x41,0x3E,0x00,
    /* P */    0x7F,0x09,0x09,0x06,0x00,
    /* Q */    0x3E,0x41,0x51,0x21,0x5E,0x00,
    /* R */    0x7F,0x09,0x19,0x29,0x46,0x00,
    /* S */    0x26,0x49,0x49,0x32,0x00,
    /* T */    0x01,0x7F,0x01,0x00,
    /* U */    0x3F,0x40,0x40,0x3F,0x00,
    /* V */    0x1F,0x60,0x40,0x60,0x1F,0x00,
    /* W */    0x7F,0x40,0x20,0x40,0x7F,0x00,
    /* X */    0x63,0x14,0x08,0x14,0x63,0x00,
    /* Y */    0x07,0x08,0x70,0x08,0x07,0x00,
    /* Z */    0x61,0x51,0x49,0x45,0x43,0x00,
    /* [ */    0x7F,0x41,0x00,
    /* \ */    0x41,0x36,0x08,0x00,
    /* ] */    0x41,0x7F,0x00,
    /* ^ */    0x04,0x02,0x01,0x00,
    /* _ */    0x7F,0x00,
    /* ` */    0x20,0x10,0x00,
    /* a */    0x3E,0x41,0x41,0x3E,0x00,
    /* b */    0x7F,0x08,0x08,0x70,0x00,
    /* c */    0x3E,0x41,0x22,0x00,
    /* d */    0x70,0x08,0x08,0x7F,0x00,
    /* e */    0x3E,0x41,0x51,0x31,0x00,
    /* f */    0x01,0x7E,0x11,0x11,0x00,
    /* g */    0x3E,0x41,0x41,0x3E,0x60,0x00,
    /* h */    0x7F,0x08,0x08,0x70,0x00,
    /* i */    0x41,0x7E,0x40,0x00,
    /* j */    0x20,0x40,0x41,0x3F,0x00,
    /* k */    0x7F,0x08,0x14,0x41,0x00,
    /* l */    0x41,0x7F,0x01,0x00,
    /* m */    0x7E,0x20,0x1E,0x20,0x7E,0x00,
    /* n */    0x7E,0x20,0x10,0x70,0x00,
    /* o */    0x3E,0x20,0x20,0x3E,0x00,
    /* p */    0x7E,0x08,0x08,0x70,0x00,
    /* q */    0x3E,0x20,0x20,0x7E,0x60,0x00,
    /* r */    0x7E,0x10,0x20,0x20,0x00,
    /* s */    0x32,0x49,0x49,0x26,0x00,
    /* t */    0x02,0x3E,0x42,0x00,
    /* u */    0x3F,0x40,0x40,0x3F,0x00,
    /* v */    0x1F,0x20,0x40,0x20,0x1F,0x00,
    /* w */    0x3F,0x40,0x30,0x40,0x3F,0x00,
    /* x */    0x31,0x0A,0x04,0x31,0x00,
    /* y */    0x1F,0x20,0x40,0x20,0x1F,0x60,0x00,
    /* z */    0x31,0x49,0x45,0x43,0x00,
    /* { */    0x7E,0x11,0x11,0x7E,0x00,
    /* | */    0x7F,0x00,
    /* } */    0x7E,0x08,0x08,0x7E,0x00,
    /* ~ */    0x10,0x00,
};

#define FONT_DATA(font_char) (&font_6x10[(font_char - 0x20) * 6])

/**
 * @brief 绘制6x10字符
 */
static int draw_char(int x, int y, uint8_t c) {
    if (c < 0x20 || c > 0x7E) return 0;
    
    const uint8_t* data = FONT_DATA(c);
    
    for (int i = 0; i < 5; i++) {
        uint8_t col = data[i];
        for (int j = 0; j < 8; j++) {
            if (col & (0x80 >> j)) {
                set_pixel(x + i, y + j, g_u8g2.color);
            }
        }
    }
    
    return 6;  /* 字符宽度 + 间距 */
}

/****************************************************************************
 * Lua Functions
 ****************************************************************************/

static int iot_u8g2_setup(lua_State* L);
static int iot_u8g2_clearBuffer(lua_State* L);
static int iot_u8g2_sendBuffer(lua_State* L);
static int iot_u8g2_setColorIndex(lua_State* L);
static int iot_u8g2_setFont(lua_State* L);
static int iot_u8g2_drawStr(lua_State* L);
static int iot_u8g2_drawPixel(lua_State* L);
static int iot_u8g2_drawLine(lua_State* L);
static int iot_u8g2_drawBox(lua_State* L);
static int iot_u8g2_drawFrame(lua_State* L);
static int iot_u8g2_drawCircle(lua_State* L);
static int iot_u8g2_drawDisc(lua_State* L);
static int iot_u8g2_drawRBox(lua_State* L);
static int iot_u8g2_drawRFrame(lua_State* L);
static int iot_u8g2_drawXBM(lua_State* L);
static int iot_u8g2_getDisplayWidth(lua_State* L);
static int iot_u8g2_getDisplayHeight(lua_State* L);

static const luaL_Reg u8g2_lib[] = {
    {"setup",           iot_u8g2_setup},
    {"clearBuffer",     iot_u8g2_clearBuffer},
    {"sendBuffer",      iot_u8g2_sendBuffer},
    {"setColorIndex",   iot_u8g2_setColorIndex},
    {"setFont",         iot_u8g2_setFont},
    {"drawStr",         iot_u8g2_drawStr},
    {"drawPixel",       iot_u8g2_drawPixel},
    {"drawLine",        iot_u8g2_drawLine},
    {"drawBox",         iot_u8g2_drawBox},
    {"drawFrame",       iot_u8g2_drawFrame},
    {"drawCircle",      iot_u8g2_drawCircle},
    {"drawDisc",        iot_u8g2_drawDisc},
    {"drawRBox",        iot_u8g2_drawRBox},
    {"drawRFrame",      iot_u8g2_drawRFrame},
    {"drawXBM",         iot_u8g2_drawXBM},
    {"getDisplayWidth", iot_u8g2_getDisplayWidth},
    {"getDisplayHeight",iot_u8g2_getDisplayHeight},
    {NULL, NULL}
};

/**
 * @brief 初始化u8g2
 * @api u8g2.setup(lcd_handle, width, height)
 * @param width int 屏幕宽度
 * @param height int 屏幕高度
 * @param bpp int 每像素位数，默认1
 * @return bool 是否成功
 */
static int iot_u8g2_setup(lua_State* L) {
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);
    int bpp = lua_isnoneornil(L, 3) ? 1 : luaL_checkinteger(L, 3);
    
    if (width <= 0 || width > U8G2_MAX_WIDTH || height <= 0 || height > U8G2_MAX_HEIGHT) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "invalid display size");
        return 2;
    }
    
    /* 释放旧缓冲区 */
    if (g_u8g2.buffer) {
        cm_free(g_u8g2.buffer);
    }
    
    g_u8g2.width = width;
    g_u8g2.height = height;
    g_u8g2.bpp = bpp;
    g_u8g2.color = 1;
    g_u8g2.page_height = 8;
    g_u8g2.current_page = 0;
    
    /* 分配帧缓冲 */
    int buf_size;
    if (bpp == 1) {
        buf_size = height * ((width + 7) / 8);
    } else {
        buf_size = height * ((width + 3) / 4);
    }
    
    g_u8g2.buffer = (uint8_t*)cm_malloc(buf_size);
    if (!g_u8g2.buffer) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "memory error");
        return 2;
    }
    
    memset(g_u8g2.buffer, 0, buf_size);
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 清除帧缓冲
 * @api u8g2.clearBuffer()
 */
static int iot_u8g2_clearBuffer(lua_State* L) {
    if (g_u8g2.buffer) {
        int buf_size;
        if (g_u8g2.bpp == 1) {
            buf_size = g_u8g2.height * ((g_u8g2.width + 7) / 8);
        } else {
            buf_size = g_u8g2.height * ((g_u8g2.width + 3) / 4);
        }
        memset(g_u8g2.buffer, 0, buf_size);
    }
    return 0;
}

/**
 * @brief 发送帧缓冲到LCD
 * @api u8g2.sendBuffer()
 * @note 这里需要与LCD模块配合，实际应用中需要调用LCD驱动
 */
static int iot_u8g2_sendBuffer(lua_State* L) {
    /* TODO: 与LCD模块集成，实际发送缓冲区数据 */
    /* 
     * extern void cm_lcd_write_buf(uint8_t* data, int len);
     * cm_lcd_write_buf(g_u8g2.buffer, buf_size);
     */
    return 0;
}

/**
 * @brief 设置颜色索引
 * @api u8g2.setColorIndex(color)
 * @param color int 颜色值 (0=黑/清除, 1=白/设置)
 */
static int iot_u8g2_setColorIndex(lua_State* L) {
    int color = luaL_checkinteger(L, 1);
    g_u8g2.color = (color != 0) ? 1 : 0;
    return 0;
}

/**
 * @brief 设置字体
 * @api u8g2.setFont(font_name)
 * @param font_name string 字体名称
 */
static int iot_u8g2_setFont(lua_State* L) {
    const char* font_name = luaL_checkstring(L, 1);
    
    /* 目前只支持内置字体 */
    if (strcmp(font_name, "u8g2.font_6x10_tf") != 0) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, "unsupported font");
        return 2;
    }
    
    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 绘制字符串
 * @api u8g2.drawStr(x, y, str)
 * @param x int X坐标
 * @param y int Y坐标
 * @param str string 字符串
 * @return int 绘制的宽度
 */
static int iot_u8g2_drawStr(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    const char* str = luaL_checkstring(L, 3);
    
    int total_width = 0;
    const char* p = str;
    
    while (*p) {
        uint32_t code = utf8_to_unicode(&p);
        
        /* 目前只支持ASCII */
        if (code >= 0x20 && code <= 0x7E) {
            total_width += draw_char(x + total_width, y, (uint8_t)code);
        }
    }
    
    lua_pushinteger(L, total_width);
    return 1;
}

/**
 * @brief 绘制像素
 * @api u8g2.drawPixel(x, y)
 * @param x int X坐标
 * @param y int Y坐标
 */
static int iot_u8g2_drawPixel(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    
    set_pixel(x, y, g_u8g2.color);
    return 0;
}

/**
 * @brief 绘制线条
 * @api u8g2.drawLine(x1, y1, x2, y2)
 * @param x1 int 起点X
 * @param y1 int 起点Y
 * @param x2 int 终点X
 * @param y2 int 终点Y
 */
static int iot_u8g2_drawLine(lua_State* L) {
    int x1 = luaL_checkinteger(L, 1);
    int y1 = luaL_checkinteger(L, 2);
    int x2 = luaL_checkinteger(L, 3);
    int y2 = luaL_checkinteger(L, 4);
    
    draw_line(x1, y1, x2, y2);
    return 0;
}

/**
 * @brief 绘制实心矩形
 * @api u8g2.drawBox(x, y, w, h)
 * @param x int X坐标
 * @param y int Y坐标
 * @param w int 宽度
 * @param h int 高度
 */
static int iot_u8g2_drawBox(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    
    for (int i = 0; i < h; i++) {
        draw_horizontal_line(x, y + i, w);
    }
    return 0;
}

/**
 * @brief 绘制空心矩形
 * @api u8g2.drawFrame(x, y, w, h)
 * @param x int X坐标
 * @param y int Y坐标
 * @param w int 宽度
 * @param h int 高度
 */
static int iot_u8g2_drawFrame(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    
    draw_horizontal_line(x, y, w);
    draw_horizontal_line(x, y + h - 1, w);
    draw_vertical_line(x, y, h);
    draw_vertical_line(x + w - 1, y, h);
    return 0;
}

/**
 * @brief 绘制圆形
 * @api u8g2.drawCircle(x, y, r, options)
 * @param x int 圆心X
 * @param y int 圆心Y
 * @param r int 半径
 * @param options int 绘制选项
 */
static int iot_u8g2_drawCircle(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int r = luaL_checkinteger(L, 3);
    int options = lua_isnoneornil(L, 4) ? U8G2_DRAW_ALL : luaL_checkinteger(L, 4);
    
    draw_circle(x, y, r, options);
    return 0;
}

/**
 * @brief 绘制实心圆
 * @api u8g2.drawDisc(x, y, r, options)
 * @param x int 圆心X
 * @param y int 圆心Y
 * @param r int 半径
 * @param options int 绘制选项
 */
static int iot_u8g2_drawDisc(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int r = luaL_checkinteger(L, 3);
    int options = lua_isnoneornil(L, 4) ? U8G2_DRAW_ALL : luaL_checkinteger(L, 4);
    
    draw_filled_circle(x, y, r, options);
    return 0;
}

/**
 * @brief 绘制圆角实心矩形
 * @api u8g2.drawRBox(x, y, w, h, r)
 */
static int iot_u8g2_drawRBox(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int r = luaL_checkinteger(L, 5);
    
    /* 简化为普通矩形 */
    draw_filled_circle(x + r, y + r, r, U8G2_DRAW_UPPER_RIGHT);
    draw_filled_circle(x + w - r - 1, y + r, r, U8G2_DRAW_UPPER_LEFT);
    draw_filled_circle(x + r, y + h - r - 1, r, U8G2_DRAW_LOWER_RIGHT);
    draw_filled_circle(x + w - r - 1, y + h - r - 1, r, U8G2_DRAW_LOWER_LEFT);
    
    for (int i = 0; i < h - 2 * r; i++) {
        draw_horizontal_line(x, y + r + i, w);
    }
    return 0;
}

/**
 * @brief 绘制圆角空心矩形
 * @api u8g2.drawRFrame(x, y, w, h, r)
 */
static int iot_u8g2_drawRFrame(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    int r = luaL_checkinteger(L, 5);
    
    draw_circle(x + r, y + r, r, U8G2_DRAW_UPPER_RIGHT);
    draw_circle(x + w - r - 1, y + r, r, U8G2_DRAW_UPPER_LEFT);
    draw_circle(x + r, y + h - r - 1, r, U8G2_DRAW_LOWER_RIGHT);
    draw_circle(x + w - r - 1, y + h - r - 1, r, U8G2_DRAW_LOWER_LEFT);
    
    draw_horizontal_line(x + r, y, w - 2 * r);
    draw_horizontal_line(x + r, y + h - 1, w - 2 * r);
    draw_vertical_line(x, y + r, h - 2 * r);
    draw_vertical_line(x + w - 1, y + r, h - 2 * r);
    return 0;
}

/**
 * @brief 绘制XBM位图
 * @api u8g2.drawXBM(x, y, w, h, data)
 * @param x int X坐标
 * @param y int Y坐标
 * @param w int 位图宽度
 * @param h int 位图高度
 * @param data string 位图数据
 */
static int iot_u8g2_drawXBM(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int w = luaL_checkinteger(L, 3);
    int h = luaL_checkinteger(L, 4);
    size_t data_len;
    const char* data = luaL_checklstring(L, 5, &data_len);
    
    int bytes_per_row = (w + 7) / 8;
    
    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            int byte_idx = row * bytes_per_row + (col / 8);
            int bit_idx = 7 - (col % 8);
            
            if (byte_idx < (int)data_len) {
                uint8_t pixel = (data[byte_idx] >> bit_idx) & 1;
                if (pixel) {
                    set_pixel(x + col, y + row, g_u8g2.color);
                }
            }
        }
    }
    return 0;
}

/**
 * @brief 获取屏幕宽度
 * @api u8g2.getDisplayWidth()
 */
static int iot_u8g2_getDisplayWidth(lua_State* L) {
    lua_pushinteger(L, g_u8g2.width);
    return 1;
}

/**
 * @brief 获取屏幕高度
 * @api u8g2.getDisplayHeight()
 */
static int iot_u8g2_getDisplayHeight(lua_State* L) {
    lua_pushinteger(L, g_u8g2.height);
    return 1;
}

LUAMOD_API int luaopen_u8g2(lua_State* L) {
    luaL_newlibtable(L, u8g2_lib);
    luaL_setfuncs(L, u8g2_lib, 0);
    
    /* 绘制选项常量 */
    lua_pushinteger(L, U8G2_DRAW_UPPER_RIGHT);
    lua_setfield(L, -2, "DRAW_UPPER_RIGHT");
    
    lua_pushinteger(L, U8G2_DRAW_UPPER_LEFT);
    lua_setfield(L, -2, "DRAW_UPPER_LEFT");
    
    lua_pushinteger(L, U8G2_DRAW_LOWER_RIGHT);
    lua_setfield(L, -2, "DRAW_LOWER_RIGHT");
    
    lua_pushinteger(L, U8G2_DRAW_LOWER_LEFT);
    lua_setfield(L, -2, "DRAW_LOWER_LEFT");
    
    lua_pushinteger(L, U8G2_DRAW_ALL);
    lua_setfield(L, -2, "DRAW_ALL");
    
    /* 内置字体常量 */
    lua_pushstring(L, "u8g2.font_6x10_tf");
    lua_setfield(L, -2, "font_6x10_tf");
    
    return 1;
}