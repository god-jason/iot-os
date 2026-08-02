/**
 * @file lvgl_port.h
 * @brief LVGL 9 端口层头文件
 *
 * 定义LVGL与IoT系统的端口层接口，包括内存管理宏映射、按钮辅助函数等。
 * 适配 LVGL 9.5.0 API，移除了旧的 LVGL 7/8 兼容层。
 *
 * @author  杰神 & TRAE & ChatGPT
 * @date    2026.07.31
 */

#ifndef IOT_LVGL_PORT_H
#define IOT_LVGL_PORT_H

#include "iot.h"
#include <stdint.h>

/* 内存管理映射：LVGL 9 使用 lv_malloc/lv_free，映射到 IoT 系统的 iot_malloc/iot_free */
#ifndef cm_malloc
#define cm_malloc(size) iot_malloc(size)
#define cm_free(ptr)    iot_free(ptr)
#endif

/* 包含 LVGL 9 主头文件（使用 lv_conf.h 配置） */
#define LV_CONF_INCLUDE_SIMPLE
#include "../../vendor/lvgl-9.5.0/lvgl.h"

/* ==================== 颜色兼容层 ==================== */
/* LVGL 9 中 lv_color_t 是 struct {red, green, blue}，不再有 full 成员。
 * 提供整数值与 lv_color_t 之间的转换辅助函数。 */

static inline lv_color_t lv_color_from_u32(uint32_t v) {
    lv_color_t c;
    c.red = (v >> 16) & 0xFF;
    c.green = (v >> 8) & 0xFF;
    c.blue = v & 0xFF;
    return c;
}

/* lv_color_to_u32 由 LVGL 9 提供，无需额外定义 */

/* LVGL 9 移除的颜色兼容函数 */
/* 调整颜色亮度（lvl: 0=最暗, 255=最亮, 127=原色） */
static inline lv_color_t lv_color_change_lightness(lv_color_t c, lv_opa_t lvl) {
    if (lvl > 127) {
        return lv_color_lighten(c, (lv_opa_t)((lvl - 127) * 2));
    } else if (lvl < 127) {
        return lv_color_darken(c, (lv_opa_t)((127 - lvl) * 2));
    }
    return c;
}

/* lv_color_mix 由 LVGL 9 的 lv_color_op.h 提供 */

/* 转换为1位颜色（基于亮度阈值） */
static inline uint8_t lv_color_to1(lv_color_t c) {
    return lv_color_luminance(c) > 127 ? 1 : 0;
}

/* ==================== 按钮辅助函数 ==================== */
/* LVGL 9 的 button 控件不再有 set_text/get_text 等函数，需要手动管理 label */

static inline lv_obj_t* iot_lvgl_btn_find_label(lv_obj_t* btn)
{
    uint32_t i;
    uint32_t cnt = lv_obj_get_child_count(btn);
    for (i = 0; i < cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(btn, i);
        if (lv_obj_check_type(child, &lv_label_class)) {
            return child;
        }
    }
    return NULL;
}

static inline void iot_lv_btn_set_text(lv_obj_t* btn, const char* text)
{
    lv_obj_t* label = iot_lvgl_btn_find_label(btn);
    if (!label) {
        label = lv_label_create(btn);
        lv_obj_center(label);
    }
    lv_label_set_text(label, text);
}

static inline const char* iot_lv_btn_get_text(lv_obj_t* btn)
{
    lv_obj_t* label = iot_lvgl_btn_find_label(btn);
    return label ? lv_label_get_text(label) : "";
}

static inline void iot_lv_btn_toggle(lv_obj_t* btn)
{
    if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
        lv_obj_remove_state(btn, LV_STATE_CHECKED);
    } else {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
    }
}

static inline void iot_lv_btn_set_checkable(lv_obj_t* btn, bool en)
{
    if (en) {
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    } else {
        lv_obj_remove_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    }
}

/* 按钮状态类型（LVGL 9 使用 lv_state_t） */
typedef lv_state_t iot_lv_btn_state_t;

static inline void iot_lv_btn_set_state(lv_obj_t* btn, iot_lv_btn_state_t state)
{
    /* 先清除 PRESSED/CHECKED 位，再按 state 设置 */
    lv_obj_remove_state(btn, LV_STATE_PRESSED | LV_STATE_CHECKED);
    if (state & LV_STATE_PRESSED) {
        lv_obj_add_state(btn, LV_STATE_PRESSED);
    }
    if (state & LV_STATE_CHECKED) {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
    }
}

static inline iot_lv_btn_state_t iot_lv_btn_get_state(lv_obj_t* btn)
{
    return (iot_lv_btn_state_t)lv_obj_get_state(btn);
}

static inline void iot_lv_btn_set_layout(lv_obj_t* btn, uint32_t layout)
{
    lv_obj_set_layout(btn, (lv_layout_t)layout);
}

/* ==================== 复选框辅助函数 ==================== */
/* LVGL 9 的 checkbox 不再有 set_checked/is_checked/set_state/get_state */

static inline void iot_lv_checkbox_set_checked(lv_obj_t* obj, bool checked)
{
    if (checked) {
        lv_obj_add_state(obj, LV_STATE_CHECKED);
    } else {
        lv_obj_remove_state(obj, LV_STATE_CHECKED);
    }
}

static inline bool iot_lv_checkbox_is_checked(const lv_obj_t* obj)
{
    return lv_obj_has_state(obj, LV_STATE_CHECKED);
}

/* 复选框状态类型 */
typedef lv_state_t iot_lv_checkbox_state_t;

static inline void iot_lv_checkbox_set_state(lv_obj_t* obj, iot_lv_checkbox_state_t state)
{
    lv_obj_remove_state(obj, LV_STATE_CHECKED | LV_STATE_DISABLED);
    if (state & LV_STATE_CHECKED) {
        lv_obj_add_state(obj, LV_STATE_CHECKED);
    }
    if (state & LV_STATE_DISABLED) {
        lv_obj_add_state(obj, LV_STATE_DISABLED);
    }
}

static inline iot_lv_checkbox_state_t iot_lv_checkbox_get_state(lv_obj_t* obj)
{
    return (iot_lv_checkbox_state_t)lv_obj_get_state(obj);
}

/* ==================== 开关辅助函数 ==================== */
/* LVGL 9 的 switch 不再有 on/off/toggle/get_state */

static inline void iot_lvgl_switch_apply_state(lv_obj_t* sw, bool checked, lv_anim_enable_t anim)
{
    uint32_t anim_time = lv_obj_get_style_anim_duration(sw, LV_PART_MAIN);
    if (anim == LV_ANIM_OFF) {
        lv_obj_set_style_anim_duration(sw, 0, LV_PART_MAIN);
    }
    if (checked) {
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    } else {
        lv_obj_remove_state(sw, LV_STATE_CHECKED);
    }
    if (anim == LV_ANIM_OFF) {
        lv_obj_set_style_anim_duration(sw, anim_time, LV_PART_MAIN);
    }
}

static inline void iot_lv_switch_on(lv_obj_t* sw, lv_anim_enable_t anim)
{
    iot_lvgl_switch_apply_state(sw, true, anim);
}

static inline void iot_lv_switch_off(lv_obj_t* sw, lv_anim_enable_t anim)
{
    iot_lvgl_switch_apply_state(sw, false, anim);
}

static inline void iot_lv_switch_toggle(lv_obj_t* sw, lv_anim_enable_t anim)
{
    iot_lvgl_switch_apply_state(sw, !lv_obj_has_state(sw, LV_STATE_CHECKED), anim);
}

static inline bool iot_lv_switch_get_state(const lv_obj_t* sw)
{
    return lv_obj_has_state(sw, LV_STATE_CHECKED);
}

/* ==================== 主题常量兼容定义 ==================== */
/* LVGL 9 移除了 LV_THEME_COLOR_xxx / LV_THEME_FONT_xxx / LV_THEME_SIZE_xxx 枚举，
 * 这里自定义本地枚举值，供 Lua 绑定通过 theme:set_color/get_color 等接口使用。 */
#define LV_THEME_COLOR_PRIMARY            0
#define LV_THEME_COLOR_SECONDARY          1
#define LV_THEME_COLOR_TERTIARY           2
#define LV_THEME_COLOR_BACKGROUND         3
#define LV_THEME_COLOR_FOREGROUND         4
#define LV_THEME_COLOR_HIGHLIGHT          5
#define LV_THEME_COLOR_SELECTED           6
#define LV_THEME_COLOR_ERROR              7
#define LV_THEME_COLOR_WARNING            8
#define LV_THEME_COLOR_SUCCESS            9

#define LV_THEME_FONT_SMALL               0
#define LV_THEME_FONT_MEDIUM              1
#define LV_THEME_FONT_LARGE               2
#define LV_THEME_FONT_TITLE               3

#define LV_THEME_SIZE_BUTTON_HEIGHT       0
#define LV_THEME_SIZE_SLIDER_WIDTH        1
#define LV_THEME_SIZE_SLIDER_HEIGHT       2

/* ==================== 其它兼容定义 ==================== */
/* LVGL 9 移除的部件/事件/布局/状态等常量，提供本地替代值以保持 Lua 接口兼容 */
#define LV_PART_TICKS                     LV_PART_ITEMS
#define LV_BTN_STATE_NONE                 LV_STATE_DEFAULT
#define LV_LABEL_LONG_EXPAND              LV_LABEL_LONG_WRAP
#define LV_LABEL_LONG_HIERARCHY           LV_LABEL_LONG_WRAP
#define LV_LABEL_LONG_CROP                LV_LABEL_LONG_MODE_CLIP
#define LV_IMG_SIZE_MODE_REAL             0
#define LV_IMG_SIZE_MODE_VIRTUAL          1
#define LV_CHART_SERIES_TYPE_SHIFT        0
#define LV_CHART_SERIES_TYPE_CIRCULAR     1
#define LV_OBJ_TYPE_NULL                  0
#define LV_OBJ_TYPE_DROPDOWN_LIST         1
#define LV_OBJ_TYPE_KEYBOARD              2
#define LV_OBJ_TYPE_MESSAGE_BOX           3
#define LV_EVENT_FOCUS_KEY                LV_EVENT_KEY
#define LV_EVENT_DRAW_PART_BEGIN          LV_EVENT_DRAW_TASK_ADDED
#define LV_EVENT_DRAW_PART_END            LV_EVENT_DRAW_TASK_ADDED
/* LVGL 9 中已移除的旧布局常量，统一映射到 LV_LAYOUT_NONE */
#define LV_LAYOUT_COL_MID                 LV_LAYOUT_NONE
#define LV_LAYOUT_COL_LEFT                LV_LAYOUT_NONE
#define LV_LAYOUT_COL_RIGHT               LV_LAYOUT_NONE
#define LV_LAYOUT_ROW_MID                 LV_LAYOUT_NONE
#define LV_LAYOUT_ROW_LEFT                LV_LAYOUT_NONE
#define LV_LAYOUT_ROW_RIGHT               LV_LAYOUT_NONE
#define LV_LAYOUT_CENTER                  LV_LAYOUT_NONE
#define LV_LAYOUT_PRETTY_MID              LV_LAYOUT_NONE
#define LV_LAYOUT_PRETTY_TOP              LV_LAYOUT_NONE
#define LV_LAYOUT_GOOD_MID                LV_LAYOUT_NONE
#define LV_LAYOUT_GOOD_TOP                LV_LAYOUT_NONE
/* 复选框 INDETERMINATE 状态在 LVGL 9 中已移除，映射到 CHECKED */
#define LV_CHECKBOX_STATE_INDETERMINATE   LV_STATE_CHECKED
/* LVGL 9 将 LV_DISP_ROT_* 重命名为 LV_DISPLAY_ROTATION_*，提供旧名称兼容 */
#define LV_DISP_ROT_NONE                  LV_DISPLAY_ROTATION_0
#define LV_DISP_ROT_90                    LV_DISPLAY_ROTATION_90
#define LV_DISP_ROT_180                   LV_DISPLAY_ROTATION_180
#define LV_DISP_ROT_270                   LV_DISPLAY_ROTATION_270

/* ==================== 常量兼容定义 ==================== */
/* 复选框状态常量 */
#define LV_CHECKBOX_STATE_UNCHECKED       0
#define LV_CHECKBOX_STATE_CHECKED         LV_STATE_CHECKED
#define LV_CHECKBOX_STATE_DISABLED        LV_STATE_DISABLED

/* 按钮状态常量 */
#define LV_BTN_STATE_RELEASED             LV_STATE_DEFAULT
#define LV_BTN_STATE_PRESSED              LV_STATE_PRESSED
#define LV_BTN_STATE_CHECKED_RELEASED     LV_STATE_CHECKED
#define LV_BTN_STATE_CHECKED_PRESSED      (LV_STATE_CHECKED | LV_STATE_PRESSED)
#define LV_BTN_STATE_DISABLED             LV_STATE_DISABLED
#define LV_BTN_STATE_CHECKED_DISABLED     (LV_STATE_CHECKED | LV_STATE_DISABLED)

/* ==================== 按键常量兼容定义 ==================== */
/* LVGL 9 移除了部分按键常量，这里提供兼容定义。
 * LV_KEY_TAB 与 LV_KEY_NEXT 同为 '\t'(9)。
 * 其余 PAGE/MEDIA/VOLUME 键为 IoT 扩展键，使用不与 LVGL 9 标准键
 * (2,3,8,9,10,11,17-20,27,127) 冲突的自定义值，由应用层自行处理。 */
#define LV_KEY_TAB                        LV_KEY_NEXT
#define LV_KEY_PAGE_UP                    0x30
#define LV_KEY_PAGE_DOWN                  0x31
#define LV_KEY_PLAY                       0x32
#define LV_KEY_PAUSE                      0x33
#define LV_KEY_STOP                       0x34
#define LV_KEY_FWD                        0x35
#define LV_KEY_BWD                        0x36
#define LV_KEY_VOLUME_UP                  0x37
#define LV_KEY_VOLUME_DOWN                0x38

/* ==================== 手势方向常量兼容定义 ==================== */
/* LVGL 9 使用 LV_DIR_* 替代旧的 LV_GESTURE_*，UP/DOWN 改名为 TOP/BOTTOM */
#define LV_GESTURE_NONE                   LV_DIR_NONE
#define LV_GESTURE_LEFT                   LV_DIR_LEFT
#define LV_GESTURE_RIGHT                  LV_DIR_RIGHT
#define LV_GESTURE_UP                     LV_DIR_TOP
#define LV_GESTURE_DOWN                   LV_DIR_BOTTOM

/* ==================== 标签文本选择函数兼容定义 ==================== */
/* LVGL 9 将 lv_label_set_text_sel_start/end 重命名为 _selection_start/end */
#define lv_label_set_text_sel_start       lv_label_set_text_selection_start
#define lv_label_set_text_sel_end         lv_label_set_text_selection_end
#define lv_label_get_text_sel_start       lv_label_get_text_selection_start
#define lv_label_get_text_sel_end         lv_label_get_text_selection_end

#endif /* IOT_LVGL_PORT_H */
