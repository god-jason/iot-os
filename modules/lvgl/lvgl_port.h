#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#include "platform.h"

#ifndef cm_malloc
#define cm_malloc(size) iot_malloc(size)
#define cm_free(ptr)    iot_free(ptr)
#endif

#define LV_CONF_SKIP
#include "../../vendor/lvgl/lvgl.h"

/* LVGL 7 layout constants used by Lua bindings (LVGL 8 compatibility) */
#ifndef LV_LAYOUT_NONE
#define LV_LAYOUT_NONE          0
#endif
#ifndef LV_LAYOUT_COL_MID
#define LV_LAYOUT_COL_MID       1
#define LV_LAYOUT_COL_LEFT      2
#define LV_LAYOUT_COL_RIGHT     3
#define LV_LAYOUT_ROW_MID       4
#define LV_LAYOUT_ROW_LEFT      5
#define LV_LAYOUT_ROW_RIGHT     6
#define LV_LAYOUT_CENTER        7
#define LV_LAYOUT_PRETTY_MID    8
#define LV_LAYOUT_PRETTY_TOP    9
#define LV_LAYOUT_GOOD_MID      10
#define LV_LAYOUT_GOOD_TOP      11
#endif

/* LVGL 7 button helpers (LVGL 8 compatibility) */
typedef lv_state_t lv_btn_state_t;

static inline lv_obj_t* lvgl_btn_find_label(lv_obj_t* btn)
{
    uint32_t i;
    uint32_t cnt = lv_obj_get_child_cnt(btn);
    for (i = 0; i < cnt; i++) {
        lv_obj_t* child = lv_obj_get_child(btn, i);
        if (lv_obj_check_type(child, &lv_label_class)) {
            return child;
        }
    }
    return NULL;
}

static inline void lv_btn_set_text(lv_obj_t* btn, const char* text)
{
    lv_obj_t* label = lvgl_btn_find_label(btn);
    if (!label) {
        label = lv_label_create(btn);
        lv_obj_center(label);
    }
    lv_label_set_text(label, text);
}

static inline const char* lv_btn_get_text(lv_obj_t* btn)
{
    lv_obj_t* label = lvgl_btn_find_label(btn);
    return label ? lv_label_get_text(label) : "";
}

static inline void lv_btn_set_state(lv_obj_t* btn, lv_btn_state_t state)
{
    lv_obj_add_state(btn, state);
}

static inline lv_btn_state_t lv_btn_get_state(lv_obj_t* btn)
{
    return lv_obj_get_state(btn);
}

static inline void lv_btn_toggle(lv_obj_t* btn)
{
    if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
        lv_obj_clear_state(btn, LV_STATE_CHECKED);
    } else {
        lv_obj_add_state(btn, LV_STATE_CHECKED);
    }
}

static inline void lv_btn_set_checkable(lv_obj_t* btn, bool en)
{
    if (en) {
        lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    } else {
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_CHECKABLE);
    }
}

static inline void lv_btn_set_layout(lv_obj_t* btn, uint32_t layout)
{
    lv_obj_set_layout(btn, layout);
}

/* LVGL 7 input device helper (LVGL 8 compatibility) */
static inline lv_indev_t* lv_indev_get_default(void)
{
    return lv_indev_get_next(NULL);
}

/* LVGL 7 checkbox helpers (LVGL 8 compatibility) */
#define LV_CHECKBOX_STATE_UNCHECKED       0
#define LV_CHECKBOX_STATE_CHECKED         LV_STATE_CHECKED
#define LV_CHECKBOX_STATE_DISABLED        LV_STATE_DISABLED
#define LV_CHECKBOX_STATE_INDETERMINATE   0

typedef lv_state_t lv_checkbox_state_t;

static inline void lv_checkbox_set_checked(lv_obj_t* obj, bool checked)
{
    if (checked) {
        lv_obj_add_state(obj, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(obj, LV_STATE_CHECKED);
    }
}

static inline bool lv_checkbox_is_checked(const lv_obj_t* obj)
{
    return lv_obj_has_state(obj, LV_STATE_CHECKED);
}

static inline void lv_checkbox_set_state(lv_obj_t* obj, lv_checkbox_state_t state)
{
    lv_obj_clear_state(obj, LV_STATE_CHECKED | LV_STATE_DISABLED);
    if (state & LV_STATE_CHECKED) {
        lv_obj_add_state(obj, LV_STATE_CHECKED);
    }
    if (state & LV_STATE_DISABLED) {
        lv_obj_add_state(obj, LV_STATE_DISABLED);
    }
}

static inline lv_checkbox_state_t lv_checkbox_get_state(const lv_obj_t* obj)
{
    lv_state_t state = lv_obj_get_state(obj);
    if (state & LV_STATE_DISABLED) {
        return LV_CHECKBOX_STATE_DISABLED;
    }
    if (state & LV_STATE_CHECKED) {
        return LV_CHECKBOX_STATE_CHECKED;
    }
    return LV_CHECKBOX_STATE_UNCHECKED;
}

/* LVGL 7 switch helpers (LVGL 8 compatibility) */
static inline void lvgl_switch_apply_state(lv_obj_t* sw, bool checked, lv_anim_enable_t anim)
{
    uint32_t anim_time = lv_obj_get_style_anim_time(sw, LV_PART_MAIN);
    if (anim == LV_ANIM_OFF) {
        lv_obj_set_style_anim_time(sw, 0, LV_PART_MAIN);
    }
    if (checked) {
        lv_obj_add_state(sw, LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(sw, LV_STATE_CHECKED);
    }
    if (anim == LV_ANIM_OFF) {
        lv_obj_set_style_anim_time(sw, anim_time, LV_PART_MAIN);
    }
}

static inline void lv_switch_on(lv_obj_t* sw, lv_anim_enable_t anim)
{
    lvgl_switch_apply_state(sw, true, anim);
}

static inline void lv_switch_off(lv_obj_t* sw, lv_anim_enable_t anim)
{
    lvgl_switch_apply_state(sw, false, anim);
}

static inline void lv_switch_toggle(lv_obj_t* sw, lv_anim_enable_t anim)
{
    lvgl_switch_apply_state(sw, !lv_obj_has_state(sw, LV_STATE_CHECKED), anim);
}

static inline bool lv_switch_get_state(const lv_obj_t* sw)
{
    return lv_obj_has_state(sw, LV_STATE_CHECKED);
}

/* LVGL 7 constants (LVGL 8 compatibility) */
#ifndef LV_BTN_STATE_NONE
#define LV_BTN_STATE_NONE                0
#define LV_BTN_STATE_RELEASED            LV_STATE_DEFAULT
#define LV_BTN_STATE_PRESSED             LV_STATE_PRESSED
#define LV_BTN_STATE_CHECKED_RELEASED    LV_STATE_CHECKED
#define LV_BTN_STATE_CHECKED_PRESSED     (LV_STATE_CHECKED | LV_STATE_PRESSED)
#define LV_BTN_STATE_DISABLED            LV_STATE_DISABLED
#define LV_BTN_STATE_CHECKED_DISABLED    (LV_STATE_CHECKED | LV_STATE_DISABLED)
#endif

#ifndef LV_LABEL_LONG_CROP
#define LV_LABEL_LONG_CROP               LV_LABEL_LONG_CLIP
#define LV_LABEL_LONG_EXPAND             LV_LABEL_LONG_WRAP
#define LV_LABEL_LONG_HIERARCHY          LV_LABEL_LONG_WRAP
#endif

#ifndef LV_CHART_SERIES_TYPE_SHIFT
#define LV_CHART_SERIES_TYPE_SHIFT       LV_CHART_UPDATE_MODE_SHIFT
#define LV_CHART_SERIES_TYPE_CIRCULAR    LV_CHART_UPDATE_MODE_CIRCULAR
#endif

#ifndef LV_OBJ_TYPE_NULL
#define LV_OBJ_TYPE_NULL                 0
#define LV_OBJ_TYPE_DROPDOWN_LIST        1
#define LV_OBJ_TYPE_KEYBOARD             2
#define LV_OBJ_TYPE_MESSAGE_BOX          3
#endif

#ifndef LV_EVENT_FOCUS_KEY
#define LV_EVENT_FOCUS_KEY               LV_EVENT_KEY
#endif

#ifndef LV_KEY_TAB
#define LV_KEY_TAB                       LV_KEY_NEXT
#define LV_KEY_PAGE_UP                   4
#define LV_KEY_PAGE_DOWN                 5
#define LV_KEY_PLAY                      6
#define LV_KEY_PAUSE                     7
#define LV_KEY_STOP                      8
#define LV_KEY_FWD                       12
#define LV_KEY_BWD                       13
#define LV_KEY_VOLUME_UP                 14
#define LV_KEY_VOLUME_DOWN               15
#endif

#ifndef LV_GESTURE_NONE
#define LV_GESTURE_NONE                  LV_DIR_NONE
#define LV_GESTURE_LEFT                  LV_DIR_LEFT
#define LV_GESTURE_RIGHT                 LV_DIR_RIGHT
#define LV_GESTURE_UP                    LV_DIR_TOP
#define LV_GESTURE_DOWN                  LV_DIR_BOTTOM
#endif

#ifndef LV_CHECKBOX_STATE_CHECKED
#define LV_CHECKBOX_STATE_CHECKED        LV_STATE_CHECKED
#define LV_CHECKBOX_STATE_UNCHECKED      0
#define LV_CHECKBOX_STATE_DISABLED       LV_STATE_DISABLED
#define LV_CHECKBOX_STATE_INDETERMINATE  LV_STATE_CHECKED
#endif

/* LVGL 7 theme ID constants (LVGL 8 compatibility) */
#ifndef LV_THEME_COLOR_PRIMARY
#define LV_THEME_COLOR_PRIMARY          0
#define LV_THEME_COLOR_SECONDARY        1
#define LV_THEME_COLOR_TERTIARY         2
#define LV_THEME_COLOR_BACKGROUND       3
#define LV_THEME_COLOR_FOREGROUND       4
#define LV_THEME_COLOR_HIGHLIGHT        5
#define LV_THEME_COLOR_SELECTED         6
#define LV_THEME_COLOR_ERROR            7
#define LV_THEME_COLOR_WARNING          8
#define LV_THEME_COLOR_SUCCESS          9
#define LV_THEME_FONT_SMALL             0
#define LV_THEME_FONT_MEDIUM            1
#define LV_THEME_FONT_LARGE             2
#define LV_THEME_FONT_TITLE             3
#define LV_THEME_SIZE_BUTTON_HEIGHT     0
#define LV_THEME_SIZE_SLIDER_WIDTH      1
#define LV_THEME_SIZE_SLIDER_HEIGHT     2
#endif

#endif /* LVGL_PORT_H */
