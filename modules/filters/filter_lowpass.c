/**
 * @file filter_lowpass.c
 * @brief 一阶低通滤波器（RC 低通）实现
 *
 * 公式: y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
 * 本质上是 EMA 的特化，alpha 由截止频率计算:
 *   alpha = dt / (dt + RC)
 * 或  alpha = 2 * pi * dt * cutoff_freq  （当 alpha 较小时）
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#include "filters.h"

int filter_lowpass_init(filter_lowpass_t* f, float alpha)
{
    if (!f || alpha < 0.0f || alpha > 1.0f) return FILTER_ERR;
    f->alpha       = alpha;
    f->value       = 0.0f;
    f->initialized = false;
    return FILTER_OK;
}

int filter_lowpass_update(filter_lowpass_t* f, float input, float* output)
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

int filter_lowpass_get(const filter_lowpass_t* f, float* output)
{
    if (!f || !output || !f->initialized) return FILTER_ERR;
    *output = f->value;
    return FILTER_OK;
}

void filter_lowpass_reset(filter_lowpass_t* f)
{
    if (f) {
        f->value       = 0.0f;
        f->initialized = false;
    }
}
