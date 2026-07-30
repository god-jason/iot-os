/**
 * @file lv_sdl_drv.h
 * @brief SDL2 显示与输入驱动
 *
 * 提供 LVGL 在桌面平台（Windows/Linux/macOS）上运行的 SDL2 窗口驱动，
 * 包括显示缓冲区刷新和鼠标输入处理。
 *
 * 使用方式：
 *   lv_sdl_drv_init();          // 初始化 SDL 窗口与 LVGL 显示/输入驱动
 *   lv_sdl_drv_loop();          // 在主循环中调用（处理事件 + lv_task_handler）
 *   lv_sdl_drv_deinit();        // 清理资源
 *
 * @author  TRAE
 * @date    2026.07.30
 */

#ifndef LV_SDL_DRV_H
#define LV_SDL_DRV_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 初始化 SDL 窗口及 LVGL 显示/输入驱动
 * @param hor_res 水平分辨率（像素），<=0 时默认 800
 * @param ver_res 垂直分辨率（像素），<=0 时默认 480
 * @return 成功返回 true
 */
bool lv_sdl_drv_init(int hor_res, int ver_res);

/**
 * @brief 检查 SDL 驱动是否已初始化
 */
bool lv_sdl_drv_is_inited(void);

/**
 * @brief SDL 事件处理（非阻塞，仅处理当前队列中的事件）
 * @return 返回 false 表示窗口已关闭
 */
bool lv_sdl_drv_loop(void);

/**
 * @brief 反初始化，释放 SDL 资源
 */
void lv_sdl_drv_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* LV_SDL_DRV_H */
