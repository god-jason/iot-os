/**
 * @file filter_kalman.c
 * @brief 一维卡尔曼滤波器实现
 *
 * 经典 1D Kalman 滤波器，适用于传感器数据（温度、距离、角度等）的
 * 平滑与估计。
 *
 * 状态方程: x[k] = x[k-1]
 * 观测方程: z[k] = x[k] + v[k]
 *
 * 可调参数:
 *   q - 过程噪声协方差（越大越信任观测值）
 *   r - 测量噪声协方差（越大越平滑，响应越慢）
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#include "filters.h"

int filter_kalman_init(filter_kalman_t* f, float q, float r)
{
    if (!f || q < 0.0f || r <= 0.0f) return FILTER_ERR;
    f->q           = q;
    f->r           = r;
    f->x           = 0.0f;
    f->p           = 1.0f;
    f->k           = 0.0f;
    f->initialized = false;
    return FILTER_OK;
}

int filter_kalman_update(filter_kalman_t* f, float measurement, float* output)
{
    if (!f || !output) return FILTER_ERR;

    if (!f->initialized) {
        f->x           = measurement;
        f->initialized = true;
        *output        = measurement;
        return FILTER_OK;
    }

    /* Predict: x_hat = x, P = P + Q */
    f->p += f->q;

    /* Update: K = P / (P + R) */
    f->k  = f->p / (f->p + f->r);

    /* Update: x = x + K * (z - x) */
    f->x += f->k * (measurement - f->x);

    /* Update: P = (1 - K) * P */
    f->p = (1.0f - f->k) * f->p;

    *output = f->x;
    return FILTER_OK;
}

int filter_kalman_get(const filter_kalman_t* f, float* output)
{
    if (!f || !output || !f->initialized) return FILTER_ERR;
    *output = f->x;
    return FILTER_OK;
}

void filter_kalman_reset(filter_kalman_t* f)
{
    if (f) {
        f->x           = 0.0f;
        f->p           = 1.0f;
        f->k           = 0.0f;
        f->initialized = false;
    }
}
