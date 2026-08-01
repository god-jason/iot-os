/**
 * @file filter_ma.c
 * @brief 滑动窗口平均 (MA) 滤波器实现
 *
 * 维护一个定长环形缓冲区，输出为窗口内所有样本的算术平均。
 * 采用增量式求和避免每步遍历，O(1) 时间复杂度。
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#include "filters.h"
#include <stdlib.h>
#include <string.h>

int iot_filter_ma_init(iot_filter_ma_t* f, int window)
{
    if (!f || window < 2) return IOT_FILTER_ERR;

    f->buffer = (float*)calloc((size_t)window, sizeof(float));
    if (!f->buffer) return IOT_FILTER_ERR;

    f->size      = window;
    f->index     = 0;
    f->count     = 0;
    f->sum       = 0.0f;
    f->need_free = true;
    return IOT_FILTER_OK;
}

int iot_filter_ma_update(iot_filter_ma_t* f, float input, float* output)
{
    if (!f || !f->buffer || !output) return IOT_FILTER_ERR;

    /* 窗口未满：追加 */
    if (f->count < f->size) {
        f->buffer[f->count] = input;
        f->sum   += input;
        f->count++;
        *output   = f->sum / (float)f->count;
        return IOT_FILTER_OK;
    }

    /* 窗口已满：滚动 */
    f->sum -= f->buffer[f->index];
    f->buffer[f->index] = input;
    f->sum += input;

    f->index = (f->index + 1) % f->size;
    *output  = f->sum / (float)f->size;
    return IOT_FILTER_OK;
}

int iot_filter_ma_get(const iot_filter_ma_t* f, float* output)
{
    if (!f || !output || !f->buffer || f->count == 0) return IOT_FILTER_ERR;
    *output = f->sum / (float)f->count;
    return IOT_FILTER_OK;
}

void iot_filter_ma_reset(iot_filter_ma_t* f)
{
    if (!f || !f->buffer) return;
    memset(f->buffer, 0, (size_t)f->size * sizeof(float));
    f->index = 0;
    f->count = 0;
    f->sum   = 0.0f;
}

void iot_filter_ma_deinit(iot_filter_ma_t* f)
{
    if (!f) return;
    if (f->buffer && f->need_free) {
        free(f->buffer);
        f->buffer = NULL;
    }
    f->need_free = false;
}
