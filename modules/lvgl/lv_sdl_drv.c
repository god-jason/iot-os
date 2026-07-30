/**
 * @file lv_sdl_drv.c
 * @brief SDL2 显示与输入驱动实现
 *
 * 创建 SDL 窗口，分配 LVGL 显示缓冲区，注册显示驱动和鼠标输入驱动。
 * 主循环中处理 SDL 事件（鼠标移动/按键/键盘/窗口关闭），
 * 并周期调用 lv_tick_inc / lv_task_handler。
 *
 * @author  TRAE
 * @date    2026.07.30
 */

#include "lv_sdl_drv.h"
#include "lvgl_port.h"
#include "iot_log.h"

#include <SDL.h>
#include <stdlib.h>

/* 默认分辨率 */
#define SDL_DRV_DEFAULT_HOR_RES  800
#define SDL_DRV_DEFAULT_VER_RES  480

/* 刷新周期（毫秒） */
#define SDL_DRV_REFR_PERIOD      5

/* SDL 上下文 */
static struct {
    bool          inited;
    SDL_Window*   window;
    SDL_Renderer* renderer;
    SDL_Texture*  texture;
    int           hor_res;
    int           ver_res;
    lv_color_t*   buf1;          /* 显示缓冲区 1 */
    lv_color_t*   buf2;          /* 显示缓冲区 2（双缓冲，可选） */
    lv_disp_drv_t disp_drv;
    lv_disp_draw_buf_t draw_buf;
    lv_indev_drv_t indev_drv;     /* 鼠标输入驱动 */
    lv_indev_drv_t kb_indev_drv;  /* 键盘输入驱动 */
    bool          mouse_pressed;
    int           mouse_x;
    int           mouse_y;
    uint32_t      last_tick;     /* 上次 tick 时间 */
    /* 键盘状态 */
    uint32_t      kb_key;        /* 当前按键(LVGL 按键码) */
    bool          kb_pressed;    /* 按键是否按下 */
} s_sdl;

/*===========================================================
 * 显示驱动回调
 *===========================================================*/

/**
 * @brief LVGL 显示刷新回调
 *        将 LVGL 渲染好的区域更新到 SDL 纹理并提交渲染
 */
static void sdl_disp_flush_cb(lv_disp_drv_t* drv, const lv_area_t* area, lv_color_t* color_p)
{
    (void)drv;
    if (!s_sdl.texture || !area || !color_p) {
        if (drv && drv->draw_buf) {
            lv_disp_flush_ready(drv);
        }
        return;
    }

    int w = area->x2 - area->x1 + 1;
    int h = area->y2 - area->y1 + 1;

    /* 将 LVGL 缓冲区数据更新到 SDL 纹理的对应区域 */
    SDL_Rect dst_rect = { area->x1, area->y1, w, h };
    SDL_UpdateTexture(s_sdl.texture, &dst_rect, color_p, w * sizeof(lv_color_t));

    /* 渲染 */
    SDL_RenderClear(s_sdl.renderer);
    SDL_RenderCopy(s_sdl.renderer, s_sdl.texture, NULL, NULL);
    SDL_RenderPresent(s_sdl.renderer);

    /* 通知 LVGL 刷新完成 */
    lv_disp_flush_ready(drv);
}

/*===========================================================
 * 鼠标输入驱动回调
 *===========================================================*/

/**
 * @brief LVGL 鼠标输入读取回调
 */
static void sdl_mouse_read_cb(lv_indev_drv_t* drv, lv_indev_data_t* data)
{
    (void)drv;
    data->point.x = s_sdl.mouse_x;
    data->point.y = s_sdl.mouse_y;
    data->state = s_sdl.mouse_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

/**
 * @brief LVGL 键盘输入读取回调
 *        将缓存的按键状态传给 LVGL，支持字符输入与控制键
 */
static void sdl_keyboard_read_cb(lv_indev_drv_t* drv, lv_indev_data_t* data)
{
    (void)drv;
    data->key = s_sdl.kb_key;
    data->state = s_sdl.kb_pressed ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
}

/**
 * @brief 将 SDL 按键码映射为 LVGL 按键码
 *        对于可打印字符直接返回 ASCII，控制键映射到 LVGL 定义
 */
static uint32_t sdl_key_to_lv_key(SDL_Keycode sdl_key)
{
    switch (sdl_key) {
    case SDLK_BACKSPACE:  return LV_KEY_BACKSPACE;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
    case SDLK_RETURN2:    return LV_KEY_ENTER;
    case SDLK_ESCAPE:     return LV_KEY_ESC;
    case SDLK_DELETE:     return LV_KEY_DEL;
    case SDLK_HOME:       return LV_KEY_HOME;
    case SDLK_END:        return LV_KEY_END;
    case SDLK_TAB:        return LV_KEY_NEXT;
    case SDLK_UP:         return LV_KEY_UP;
    case SDLK_DOWN:       return LV_KEY_DOWN;
    case SDLK_LEFT:       return LV_KEY_LEFT;
    case SDLK_RIGHT:      return LV_KEY_RIGHT;
    default:
        /* 可打印 ASCII 字符直接返回 */
        if (sdl_key >= 0x20 && sdl_key <= 0x7E) {
            return (uint32_t)sdl_key;
        }
        return 0;
    }
}

/*===========================================================
 * SDL 事件处理
 *===========================================================*/

/**
 * @brief 处理 SDL 事件，映射到 LVGL 输入
 * @return 返回 false 表示收到退出事件
 */
static bool sdl_process_events(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            return false;

        case SDL_WINDOWEVENT:
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                return false;
            }
            break;

        case SDL_MOUSEMOTION:
            s_sdl.mouse_x = event.motion.x;
            s_sdl.mouse_y = event.motion.y;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                s_sdl.mouse_pressed = true;
                s_sdl.mouse_x = event.button.x;
                s_sdl.mouse_y = event.button.y;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                s_sdl.mouse_pressed = false;
                s_sdl.mouse_x = event.button.x;
                s_sdl.mouse_y = event.button.y;
            }
            break;

        case SDL_KEYDOWN: {
            uint32_t key = sdl_key_to_lv_key(event.key.keysym.sym);
            if (key != 0) {
                s_sdl.kb_key = key;
                s_sdl.kb_pressed = true;
            }
            break;
        }

        case SDL_KEYUP:
            s_sdl.kb_pressed = false;
            break;

        default:
            break;
        }
    }
    return true;
}

/*===========================================================
 * 公开接口
 *===========================================================*/

bool lv_sdl_drv_init(int hor_res, int ver_res)
{
    if (s_sdl.inited) {
        LOG_WARN("SDL driver already initialized");
        return true;
    }

    /* 确保 LVGL 已初始化 */
    lv_init();

    if (hor_res <= 0) hor_res = SDL_DRV_DEFAULT_HOR_RES;
    if (ver_res <= 0) ver_res = SDL_DRV_DEFAULT_VER_RES;
    s_sdl.hor_res = hor_res;
    s_sdl.ver_res = ver_res;

    /* 初始化 SDL */
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        LOG_ERROR("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    /* 创建窗口 */
    s_sdl.window = SDL_CreateWindow(
        "IoT-OS LVGL",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        hor_res, ver_res,
        SDL_WINDOW_SHOWN
    );
    if (!s_sdl.window) {
        LOG_ERROR("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return false;
    }

    /* 创建渲染器 */
    s_sdl.renderer = SDL_CreateRenderer(s_sdl.window, -1, SDL_RENDERER_ACCELERATED);
    if (!s_sdl.renderer) {
        LOG_WARN("Hardware accel unavailable, fallback to software renderer");
        s_sdl.renderer = SDL_CreateRenderer(s_sdl.window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!s_sdl.renderer) {
        LOG_ERROR("SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(s_sdl.window);
        SDL_Quit();
        return false;
    }

    /* 创建纹理（LVGL 颜色格式：RGB565） */
    s_sdl.texture = SDL_CreateTexture(
        s_sdl.renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        hor_res, ver_res
    );
    if (!s_sdl.texture) {
        LOG_ERROR("SDL_CreateTexture failed: %s", SDL_GetError());
        SDL_DestroyRenderer(s_sdl.renderer);
        SDL_DestroyWindow(s_sdl.window);
        SDL_Quit();
        return false;
    }

    /* 分配 LVGL 显示缓冲区 */
    size_t buf_size = hor_res * ver_res * sizeof(lv_color_t);
    s_sdl.buf1 = (lv_color_t*)malloc(buf_size);
    if (!s_sdl.buf1) {
        LOG_ERROR("malloc buf1 failed");
        SDL_DestroyTexture(s_sdl.texture);
        SDL_DestroyRenderer(s_sdl.renderer);
        SDL_DestroyWindow(s_sdl.window);
        SDL_Quit();
        return false;
    }
    memset(s_sdl.buf1, 0, buf_size);

    /* 初始化 LVGL 显示缓冲区 */
    lv_disp_draw_buf_init(&s_sdl.draw_buf, s_sdl.buf1, NULL, hor_res * ver_res);

    /* 初始化显示驱动 */
    lv_disp_drv_init(&s_sdl.disp_drv);
    s_sdl.disp_drv.hor_res = hor_res;
    s_sdl.disp_drv.ver_res = ver_res;
    s_sdl.disp_drv.draw_buf = &s_sdl.draw_buf;
    s_sdl.disp_drv.flush_cb = sdl_disp_flush_cb;
    s_sdl.disp_drv.antialiasing = 1;

    lv_disp_t* disp = lv_disp_drv_register(&s_sdl.disp_drv);
    if (!disp) {
        LOG_ERROR("lv_disp_drv_register failed");
        free(s_sdl.buf1);
        SDL_DestroyTexture(s_sdl.texture);
        SDL_DestroyRenderer(s_sdl.renderer);
        SDL_DestroyWindow(s_sdl.window);
        SDL_Quit();
        return false;
    }

    /* 初始化鼠标输入驱动 */
    lv_indev_drv_init(&s_sdl.indev_drv);
    s_sdl.indev_drv.type = LV_INDEV_TYPE_POINTER;
    s_sdl.indev_drv.read_cb = sdl_mouse_read_cb;
    lv_indev_drv_register(&s_sdl.indev_drv);

    /* 初始化键盘输入驱动（用于 textarea 等文本输入） */
    lv_indev_drv_init(&s_sdl.kb_indev_drv);
    s_sdl.kb_indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    s_sdl.kb_indev_drv.read_cb = sdl_keyboard_read_cb;
    lv_indev_drv_register(&s_sdl.kb_indev_drv);

    /* 初始化 LVGL tick */
    /* LVGL v8 使用 lv_tick_inc 手动递增 */

    s_sdl.inited = true;
    s_sdl.mouse_pressed = false;
    s_sdl.mouse_x = 0;
    s_sdl.mouse_y = 0;
    s_sdl.kb_key = 0;
    s_sdl.kb_pressed = false;
    s_sdl.last_tick = SDL_GetTicks();

    LOG_INFO("SDL driver init OK: %dx%d", hor_res, ver_res);
    return true;
}

bool lv_sdl_drv_is_inited(void)
{
    return s_sdl.inited;
}

bool lv_sdl_drv_loop(void)
{
    if (!s_sdl.inited) {
        return true;  /* 未初始化不阻止调用方 */
    }

    /* 处理 SDL 事件（非阻塞） */
    if (!sdl_process_events()) {
        LOG_INFO("SDL window closed");
        return false;
    }

    /* 递增 LVGL tick */
    uint32_t now = SDL_GetTicks();
    uint32_t elapsed = now - s_sdl.last_tick;
    if (elapsed > 0) {
        lv_tick_inc(elapsed);
        s_sdl.last_tick = now;
    }

    return true;
}

void lv_sdl_drv_deinit(void)
{
    if (!s_sdl.inited) {
        return;
    }

    if (s_sdl.buf1) {
        free(s_sdl.buf1);
        s_sdl.buf1 = NULL;
    }
    if (s_sdl.buf2) {
        free(s_sdl.buf2);
        s_sdl.buf2 = NULL;
    }
    if (s_sdl.texture) {
        SDL_DestroyTexture(s_sdl.texture);
        s_sdl.texture = NULL;
    }
    if (s_sdl.renderer) {
        SDL_DestroyRenderer(s_sdl.renderer);
        s_sdl.renderer = NULL;
    }
    if (s_sdl.window) {
        SDL_DestroyWindow(s_sdl.window);
        s_sdl.window = NULL;
    }

    SDL_Quit();
    s_sdl.inited = false;
    LOG_INFO("SDL driver deinit");
}
