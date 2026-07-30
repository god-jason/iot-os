/**
 * @file filter_ema.c
 * @brief 指数移动平均 (EMA) 滤波器实现
 *
 * 公式: y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
 * alpha 越大，响应越快，但平滑效果越弱。
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#include "filters.h"

int filter_ema_init(filter_ema_t* f, float alpha)
{
    if (!f || alpha < 0.0f || alpha > 1.0f) return FILTER_ERR;
    f->alpha       = alpha;
    f->value       = 0.0f;
    f->initialized = false;
    return FILTER_OK;
}

int filter_ema_update(filter_ema_t* f, float input, float* output)
{
    if (!f || !output) return FILTER_ERR;

    if (!f->initialized) {
        f->value       = input;
        f->initialized = true;
    } else {
        f->value = f->alpha * input + (1.0f - f->alpha) * f->value;
    }
    *output = f->value;
    return FILTER_OK;
}

int filter_ema_get(const filter_ema_t* f, float* output)
{
    if (!f || !output || !f->initialized) return FILTER_ERR;
    *output = f->value;
    return FILTER_OK;
}

void filter_ema_reset(filter_ema_t* f)
{
    if (f) {
        f->value       = 0.0f;
        f->initialized = false;
    }
}
