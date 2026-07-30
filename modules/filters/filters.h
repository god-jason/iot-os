/**
 * @file filters.h
 * @brief 数字信号滤波算法库头文件
 *
 * 提供嵌入式场景常用的数字滤波算法 C 语言 API，包括：
 * - 指数移动平均 (EMA)
 * - 滑动窗口平均 (MA)
 * - 中值滤波 (Median)
 * - 一维卡尔曼滤波 (Kalman)
 * - 一阶/二阶 IIR 数字滤波器
 * - 低通滤波 (Low-Pass / 一阶 RC)
 * - 互补滤波 (Complementary)
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#ifndef IOT_FILTERS_H
#define IOT_FILTERS_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 通用返回值
 *===========================================================*/

#define FILTER_OK     0
#define FILTER_ERR   -1

/*===========================================================
 * 滤波器类型枚举
 *===========================================================*/

typedef enum {
    FILTER_TYPE_EMA = 0,
    FILTER_TYPE_MA,
    FILTER_TYPE_MEDIAN,
    FILTER_TYPE_KALMAN,
    FILTER_TYPE_IIR,
    FILTER_TYPE_LOWPASS,
    FILTER_TYPE_COMPLEMENTARY
} filter_type_t;

/*===========================================================
 * 滤波器结构体定义
 *===========================================================*/

/**
 * @brief EMA (指数移动平均) 滤波器
 *
 * 公式: y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
 * alpha 越大，响应越快，但平滑效果越弱。
 */
typedef struct {
    float alpha;           /**< 平滑系数 (0.0 ~ 1.0) */
    float value;           /**< 当前滤波输出 */
    bool  initialized;     /**< 是否已初始化 */
} filter_ema_t;

/**
 * @brief MA (滑动窗口平均) 滤波器
 *
 * 维护一个定长环形缓冲区，输出为窗口内所有样本的算术平均。
 * 采用增量式求和避免每步遍历，O(1) 时间复杂度。
 */
typedef struct {
    float* buffer;         /**< 环形缓冲区（外部分配或 malloc） */
    int    size;           /**< 窗口大小 */
    int    index;          /**< 当前写入位置 */
    int    count;          /**< 已收集的样本数 (< size 时窗口未满) */
    float  sum;            /**< 当前窗口内样本总和（增量维护） */
    bool   need_free;      /**< 析构时是否需要释放 buffer */
} filter_ma_t;

/**
 * @brief 中值滤波器
 *
 * 排序并取中位数，对脉冲噪声（椒盐噪声）抑制效果极佳。
 * 每步 O(n log n)，适用于窗口 ≤ 50 的场景。
 */
typedef struct {
    float* buffer;         /**< 环形缓冲区 */
    float* sorted;         /**< 排序临时缓冲区 */
    int    size;           /**< 窗口大小（建议奇数） */
    int    index;          /**< 当前写入位置 */
    int    count;          /**< 已收集样本数 */
    bool   need_free;      /**< 析构时是否需要释放 buffer/sorted */
} filter_median_t;

/**
 * @brief 一维卡尔曼滤波器
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
 */
typedef struct {
    float q;               /**< 过程噪声协方差 (process noise) */
    float r;               /**< 测量噪声协方差 (measurement noise) */
    float x;               /**< 状态估计值 */
    float p;               /**< 估计误差协方差 */
    float k;               /**< 卡尔曼增益（缓存，仅调试用） */
    bool  initialized;     /**< 是否已初始化 */
} filter_kalman_t;

/**
 * @brief IIR 数字滤波器
 *
 * 支持一阶和二阶 IIR 滤波器。
 *
 * 差分方程:
 *   y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
 *
 *   order=1 时只使用 b0,b1 和 a1
 *   order=2 时使用全部系数
 */
typedef struct {
    float b[3];            /**< 分子系数 {b0, b1, b2} */
    float a[3];            /**< 分母系数 {a0, a1, a2}，a[0] 恒为 1.0（归一化后） */
    float x[3];            /**< 输入历史 {x[n], x[n-1], x[n-2]} */
    float y[3];            /**< 输出历史 {y[n], y[n-1], y[n-2]} */
    int   order;           /**< 滤波器阶数 (1 或 2) */
    bool  initialized;     /**< 是否已初始化 */
} filter_iir_t;

/**
 * @brief 一阶低通滤波器（RC 低通）
 *
 * 公式: y[n] = alpha * x[n] + (1 - alpha) * y[n-1]
 * 本质上是 EMA 的特化，alpha 由截止频率计算:
 *   alpha = dt / (dt + RC)
 * 或  alpha = 2 * pi * dt * cutoff_freq  （当 alpha 较小时）
 */
typedef struct {
    float alpha;           /**< 平滑系数 */
    float value;           /**< 当前滤波输出 */
    bool  initialized;     /**< 是否已初始化 */
} filter_lowpass_t;

/**
 * @brief 互补滤波器
 *
 * 融合两种传感器的数据：一个用于长时间稳定（低频可信），
 * 另一个用于短时精确（高频可信）。
 *
 * 公式: y = alpha * (y + high_freq_input) + (1 - alpha) * low_freq_input
 *
 * 典型应用: IMU 姿态估计（陀螺仪 + 加速度计融合）
 *   angle = alpha * (angle + gyro * dt) + (1 - alpha) * accel_angle
 */
typedef struct {
    float alpha;           /**< 互补系数 (0.0 ~ 1.0) */
    float value;           /**< 当前滤波输出 */
    bool  initialized;     /**< 是否已初始化 */
} filter_complementary_t;

/*===========================================================
 * EMA 接口
 *===========================================================*/

int  filter_ema_init(filter_ema_t* f, float alpha);
int  filter_ema_update(filter_ema_t* f, float input, float* output);
int  filter_ema_get(const filter_ema_t* f, float* output);
void filter_ema_reset(filter_ema_t* f);

/*===========================================================
 * MA 接口
 *===========================================================*/

int  filter_ma_init(filter_ma_t* f, int window);
int  filter_ma_update(filter_ma_t* f, float input, float* output);
int  filter_ma_get(const filter_ma_t* f, float* output);
void filter_ma_reset(filter_ma_t* f);
void filter_ma_deinit(filter_ma_t* f);

/*===========================================================
 * 中值滤波接口
 *===========================================================*/

int  filter_median_init(filter_median_t* f, int window);
int  filter_median_update(filter_median_t* f, float input, float* output);
int  filter_median_get(const filter_median_t* f, float* output);
void filter_median_reset(filter_median_t* f);
void filter_median_deinit(filter_median_t* f);

/*===========================================================
 * 卡尔曼滤波接口
 *===========================================================*/

int  filter_kalman_init(filter_kalman_t* f, float q, float r);
int  filter_kalman_update(filter_kalman_t* f, float measurement, float* output);
int  filter_kalman_get(const filter_kalman_t* f, float* output);
void filter_kalman_reset(filter_kalman_t* f);

/*===========================================================
 * IIR 滤波接口
 *===========================================================*/

int  filter_iir_init(filter_iir_t* f, const float* b, const float* a, int order);
int  filter_iir_update(filter_iir_t* f, float input, float* output);
int  filter_iir_get(const filter_iir_t* f, float* output);
void filter_iir_reset(filter_iir_t* f);

/*===========================================================
 * 低通滤波接口
 *===========================================================*/

int  filter_lowpass_init(filter_lowpass_t* f, float alpha);
int  filter_lowpass_update(filter_lowpass_t* f, float input, float* output);
int  filter_lowpass_get(const filter_lowpass_t* f, float* output);
void filter_lowpass_reset(filter_lowpass_t* f);

/*===========================================================
 * 互补滤波接口
 *===========================================================*/

int  filter_complementary_init(filter_complementary_t* f, float alpha);
int  filter_complementary_update(filter_complementary_t* f,
                                 float low_pass_input, float high_pass_delta,
                                 float* output);
int  filter_complementary_get(const filter_complementary_t* f, float* output);
void filter_complementary_reset(filter_complementary_t* f);

/*===========================================================
 * 通用 / 工具函数
 *===========================================================*/

/**
 * @brief 根据截止频率和采样周期计算低通 alpha
 * @param cutoff_hz   截止频率 (Hz)
 * @param sample_rate 采样率 (Hz)
 * @return alpha 值
 */
float filter_calc_alpha_from_freq(float cutoff_hz, float sample_rate);

/**
 * @brief 根据时间常数计算低通 alpha
 * @param tau         时间常数 (s)，即 RC 值
 * @param sample_rate 采样率 (Hz)
 * @return alpha 值
 */
float filter_calc_alpha_from_tau(float tau, float sample_rate);

/**
 * @brief 对 float 数组排序（供中值滤波器内部使用）
 * @param arr  待排序数组
 * @param len  数组长度
 */
void filter_sort_floats(float* arr, int len);

#ifdef __cplusplus
}
#endif

#endif /* IOT_FILTERS_H */
