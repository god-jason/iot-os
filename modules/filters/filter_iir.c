/**
 * @file filter_iir.c
 * @brief IIR 数字滤波器实现
 *
 * 支持一阶和二阶 IIR 滤波器。
 *
 * 差分方程:
 *   y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
 *
 *   order=1 时只使用 b0,b1 和 a1
 *   order=2 时使用全部系数
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#include "filters.h"
#include <string.h>

int filter_iir_init(filter_iir_t* f, const float* b, const float* a, int order)
{
    if (!f || !b || !a || (order != 1 && order != 2)) return FILTER_ERR;

    /* 系数归一化：使 a[0] = 1.0 */
    float a0_inv = 1.0f / a[0];
    for (int i = 0; i <= order; i++) {
        f->b[i] = b[i] * a0_inv;
        f->a[i] = a[i] * a0_inv;
    }
    /* 高阶系数清零 */
    if (order == 1) {
        f->b[2] = 0.0f; f->a[2] = 0.0f;
    }
    /* 历史清零 */
    memset(f->x, 0, sizeof(f->x));
    memset(f->y, 0, sizeof(f->y));
    f->order       = order;
    f->initialized = false;
    return FILTER_OK;
}

int filter_iir_update(filter_iir_t* f, float input, float* output)
{
    if (!f || !output) return FILTER_ERR;

    /* 移位输入/输出历史 */
    f->x[2] = f->x[1];
    f->x[1] = f->x[0];
    f->x[0] = input;

    f->y[2] = f->y[1];
    f->y[1] = f->y[0];

    /* 差分方程: y[n] = sum(bi*x[n-i]) - sum(aj*y[n-j]), j>=1 */
    if (f->order == 1) {
        f->y[0] = f->b[0] * f->x[0] + f->b[1] * f->x[1]
                - f->a[1] * f->y[1];
    } else {
        f->y[0] = f->b[0] * f->x[0] + f->b[1] * f->x[1] + f->b[2] * f->x[2]
                - f->a[1] * f->y[1] - f->a[2] * f->y[2];
    }

    if (!f->initialized) f->initialized = true;
    *output = f->y[0];
    return FILTER_OK;
}

int filter_iir_get(const filter_iir_t* f, float* output)
{
    if (!f || !output || !f->initialized) return FILTER_ERR;
    *output = f->y[0];
    return FILTER_OK;
}

void filter_iir_reset(filter_iir_t* f)
{
    if (f) {
        memset(f->x, 0, sizeof(f->x));
        memset(f->y, 0, sizeof(f->y));
        f->initialized = false;
    }
}
