/**
 * @file filters.c
 * @brief 数字滤波工具函数实现
 *
 * 提供滤波器常用的辅助计算函数：
 * - 根据截止频率/时间常数计算低通 alpha
 * - float 数组排序（供中值滤波器等使用）
 *
 * 各滤波算法的具体实现已拆分到独立源文件：
 * filter_ema.c / filter_ma.c / filter_median.c /
 * filter_kalman.c / filter_iir.c / filter_lowpass.c /
 * filter_complementary.c
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#include "filters.h"
#include <stdlib.h>

/**
 * @brief 根据截止频率和采样周期计算低通 alpha
 * @param cutoff_hz   截止频率 (Hz)
 * @param sample_rate 采样率 (Hz)
 * @return alpha 值
 */
float filter_calc_alpha_from_freq(float cutoff_hz, float sample_rate)
{
    if (sample_rate <= 0.0f) return 1.0f;
    float dt = 1.0f / sample_rate;
    float alpha = 2.0f * 3.141592653589f * dt * cutoff_hz;
    /* 钳制到有效范围 */
    if (alpha > 1.0f) alpha = 1.0f;
    if (alpha < 0.0f) alpha = 0.0f;
    return alpha;
}

/**
 * @brief 根据时间常数计算低通 alpha
 * @param tau         时间常数 (s)，即 RC 值
 * @param sample_rate 采样率 (Hz)
 * @return alpha 值
 */
float filter_calc_alpha_from_tau(float tau, float sample_rate)
{
    if (sample_rate <= 0.0f || tau <= 0.0f) return 1.0f;
    float dt = 1.0f / sample_rate;
    float alpha = dt / (dt + tau);
    if (alpha > 1.0f) alpha = 1.0f;
    if (alpha < 0.0f) alpha = 0.0f;
    return alpha;
}

/**
 * @brief 对 float 数组排序（插入排序，小数据量比 qsort 更快）
 * @param arr  待排序数组
 * @param len  数组长度
 */
void filter_sort_floats(float* arr, int len)
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
