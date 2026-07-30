/**
 * @file filter_complementary.c
 * @brief 互补滤波器实现
 *
 * 融合两种传感器的数据：一个用于长时间稳定（低频可信），
 * 另一个用于短时精确（高频可信）。
 *
 * 公式: y = alpha * (y + high_freq_input) + (1 - alpha) * low_freq_input
 *
 * 典型应用: IMU 姿态估计（陀螺仪 + 加速度计融合）
 *   angle = alpha * (angle + gyro * dt) + (1 - alpha) * accel_angle
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#include "filters.h"

int filter_complementary_init(filter_complementary_t* f, float alpha)
{
    if (!f || alpha < 0.0f || alpha > 1.0f) return FILTER_ERR;
    f->alpha       = alpha;
    f->value       = 0.0f;
    f->initialized = false;
    return FILTER_OK;
}

int filter_complementary_update(filter_complementary_t* f,
                                float low_pass_input, float high_pass_delta,
                                float* output)
{
    if (!f || !output) return FILTER_ERR;

    if (!f->initialized) {
        f->value       = low_pass_input;
        f->initialized = true;
    } else {
        f->value = f->alpha * (f->value + high_pass_delta)
                 + (1.0f - f->alpha) * low_pass_input;
    }
    *output = f->value;
    return FILTER_OK;
}

int filter_complementary_get(const filter_complementary_t* f, float* output)
{
    if (!f || !output || !f->initialized) return FILTER_ERR;
    *output = f->value;
    return FILTER_OK;
}

void filter_complementary_reset(filter_complementary_t* f)
{
    if (f) {
        f->value       = 0.0f;
        f->initialized = false;
    }
}
