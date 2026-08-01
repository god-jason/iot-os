/**
 * @file filter_median.c
 * @brief 中值滤波器实现
 *
 * 排序并取中位数，对脉冲噪声（椒盐噪声）抑制效果极佳。
 * 每步 O(n log n)，适用于窗口 ≤ 50 的场景。
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#include "filters.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief 插入排序（小数据量比 qsort 更快 + 无递归栈开销）
 */
static void sort_floats(float* arr, int len)
{
    int i, j;
    for (i = 1; i < len; i++) {
        float key = arr[i];
        for (j = i - 1; j >= 0 && arr[j] > key; j--) {
            arr[j + 1] = arr[j];
        }
        arr[j + 1] = key;
    }
}

int iot_filter_median_init(iot_filter_median_t* f, int window)
{
    if (!f || window < 3) return IOT_FILTER_ERR;

    f->buffer = (float*)calloc((size_t)window, sizeof(float));
    f->sorted = (float*)calloc((size_t)window, sizeof(float));
    if (!f->buffer || !f->sorted) {
        free(f->buffer);
        free(f->sorted);
        f->buffer = NULL;
        f->sorted = NULL;
        return IOT_FILTER_ERR;
    }

    f->size      = window;
    f->index     = 0;
    f->count     = 0;
    f->need_free = true;
    return IOT_FILTER_OK;
}

int iot_filter_median_update(iot_filter_median_t* f, float input, float* output)
{
    if (!f || !f->buffer || !f->sorted || !output) return IOT_FILTER_ERR;

    /* 窗口未满 */
    if (f->count < f->size) {
        f->buffer[f->count] = input;
        f->count++;
    } else {
        /* 环形写入 */
        f->buffer[f->index] = input;
        f->index = (f->index + 1) % f->size;
    }

    /* 复制 + 排序取中位数 */
    int valid = (f->count < f->size) ? f->count : f->size;
    memcpy(f->sorted, f->buffer, (size_t)valid * sizeof(float));
    sort_floats(f->sorted, valid);

    if (valid % 2 == 1) {
        *output = f->sorted[valid / 2];
    } else {
        *output = (f->sorted[valid / 2 - 1] + f->sorted[valid / 2]) * 0.5f;
    }
    return IOT_FILTER_OK;
}

int iot_filter_median_get(const iot_filter_median_t* f, float* output)
{
    if (!f || !output || !f->buffer || f->count == 0) return IOT_FILTER_ERR;

    int valid = (f->count < f->size) ? f->count : f->size;
    memcpy(f->sorted, f->buffer, (size_t)valid * sizeof(float));
    sort_floats(f->sorted, valid);

    if (valid % 2 == 1) {
        *output = f->sorted[valid / 2];
    } else {
        *output = (f->sorted[valid / 2 - 1] + f->sorted[valid / 2]) * 0.5f;
    }
    return IOT_FILTER_OK;
}

void iot_filter_median_reset(iot_filter_median_t* f)
{
    if (!f || !f->buffer) return;
    memset(f->buffer, 0, (size_t)f->size * sizeof(float));
    memset(f->sorted, 0, (size_t)f->size * sizeof(float));
    f->index = 0;
    f->count = 0;
}

void iot_filter_median_deinit(iot_filter_median_t* f)
{
    if (!f) return;
    if (f->need_free) {
        free(f->buffer);
        free(f->sorted);
    }
    f->buffer = NULL;
    f->sorted = NULL;
    f->need_free = false;
}
