/**
 * @file model.h
 * @brief 深度学习模型加载与推理接口
 *
 * 提供统一的模型加载、推理、资源释放接口。
 * 底层根据平台自动选择推理后端:
 *   - ESP32 系列: TensorFlow Lite Micro (TFLite Micro)
 *   - Linux / Yopen: ONNX Runtime 或 TFLite
 *   - Windows: ONNX Runtime 或 TFLite
 *   - 其他平台: 无后端 (返回错误)
 *
 * 设计原则:
 *   1. 统一的 API，不依赖具体后端细节
 *   2. 模型文件从 iotfs 文件系统加载
 *   3. 支持 INT8 / FP32 两种精度
 *   4. 内存使用 iot_malloc/iot_free 管理
 *
 * @author 杰神 & CodeBuddy
 * @date   2026.08.01
 */
#ifndef IOT_MODEL_H
#define IOT_MODEL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

/* 推理后端类型 */
typedef enum {
    MODEL_BACKEND_AUTO   = 0,  /* 自动选择最优后端 */
    MODEL_BACKEND_TFLITE = 1,  /* TFLite (ESP32 / Linux) */
    MODEL_BACKEND_ONNX   = 2,  /* ONNX Runtime (Linux / Windows) */
    MODEL_BACKEND_NONE   = 3   /* 无可用的推理后端 */
} model_backend_t;

/* 模型精度类型 */
typedef enum {
    MODEL_PRECISION_AUTO = 0,  /* 自动检测 */
    MODEL_PRECISION_FP32 = 1,  /* 32 位浮点 */
    MODEL_PRECISION_FP16 = 2,  /* 16 位浮点 */
    MODEL_PRECISION_INT8 = 3,  /* 8 位整数量化 */
    MODEL_PRECISION_INT16= 4   /* 16 位整数量化 */
} model_precision_t;

/* Tensor 数据类型 */
typedef enum {
    MODEL_DTYPE_FLOAT32 = 0,
    MODEL_DTYPE_FLOAT16 = 1,
    MODEL_DTYPE_INT32   = 2,
    MODEL_DTYPE_INT8    = 3,
    MODEL_DTYPE_UINT8   = 4,
    MODEL_DTYPE_INT16   = 5,
    MODEL_DTYPE_BOOL    = 6,
    MODEL_DTYPE_STRING  = 7
} model_dtype_t;

/* Tensor 信息 */
typedef struct {
    char     name[64];          /* Tensor 名称 */
    model_dtype_t dtype;        /* 数据类型 */
    int      dims[8];           /* 维度数组 */
    int      ndim;              /* 维度数 */
    size_t   size_bytes;        /* 数据大小 (字节) */
    void*    data;              /* 数据指针 (模型内部管理) */
} model_tensor_info_t;

/* 模型句柄 (不透明指针) */
typedef struct model_ctx model_t;

/*===========================================================
 * 后台管理
 *===========================================================*/

/**
 * @brief 获取当前平台可用的推理后端
 * @return 可用的后端类型
 */
model_backend_t model_get_available_backend(void);

/**
 * @brief 获取后端名称
 * @param backend 后端类型
 * @return 名称字符串
 */
const char* model_backend_name(model_backend_t backend);

/*===========================================================
 * 模型生命周期
 *===========================================================*/

/**
 * @brief 从文件路径加载模型
 * @param path 模型文件路径 (如 "/app/models/anomaly.tflite")
 * @param backend 指定后端 (MODEL_BACKEND_AUTO 自动选择)
 * @param precision 期望精度 (MODEL_PRECISION_AUTO 自动检测)
 * @return 模型句柄，失败返回 NULL
 */
model_t* model_load_from_file(const char* path, model_backend_t backend,
                               model_precision_t precision);

/**
 * @brief 从内存加载模型
 * @param model_data 模型数据缓冲区
 * @param model_size 数据大小
 * @param backend 指定后端
 * @param precision 期望精度
 * @return 模型句柄，失败返回 NULL
 */
model_t* model_load_from_memory(const uint8_t* model_data, size_t model_size,
                                 model_backend_t backend, model_precision_t precision);

/**
 * @brief 释放模型资源
 * @param model 模型句柄
 */
void model_free(model_t* model);

/**
 * @brief 重新加载模型 (用于 OTA 更新场景)
 * @param model 现有模型句柄 (会被释放并重建)
 * @param path 新模型文件路径
 * @return 新的模型句柄，失败返回 NULL (原 model 已释放)
 */
model_t* model_reload(model_t* model, const char* path);

/*===========================================================
 * 推理接口
 *===========================================================*/

/**
 * @brief 执行推理
 * @param model 模型句柄
 * @return 0 成功，-1 失败
 */
int model_invoke(model_t* model);

/**
 * @brief 执行推理 (异步，在独立任务中运行)
 * @param model 模型句柄
 * @param callback 完成回调 (参数: void* user_data, int result)
 * @param user_data 用户数据
 * @return 0 异步任务已提交，-1 失败
 */
int model_invoke_async(model_t* model,
                       void (*callback)(void* user_data, int result),
                       void* user_data);

/*===========================================================
 * Tensor 操作
 *===========================================================*/

/**
 * @brief 获取模型输入 Tensor 数量
 * @param model 模型句柄
 * @return 输入数
 */
int model_get_input_count(model_t* model);

/**
 * @brief 获取模型输出 Tensor 数量
 * @param model 模型句柄
 * @return 输出数
 */
int model_get_output_count(model_t* model);

/**
 * @brief 获取输入 Tensor 信息
 * @param model 模型句柄
 * @param index 索引 (0-based)
 * @param info 输出信息结构
 * @return 0 成功，-1 失败
 */
int model_get_input_info(model_t* model, int index, model_tensor_info_t* info);

/**
 * @brief 获取输出 Tensor 信息
 * @param model 模型句柄
 * @param index 索引 (0-based)
 * @param info 输出信息结构
 * @return 0 成功，-1 失败
 */
int model_get_output_info(model_t* model, int index, model_tensor_info_t* info);

/**
 * @brief 设置输入 Tensor 数据
 * @param model 模型句柄
 * @param index 输入索引
 * @param data 输入数据
 * @param size 数据大小 (字节)
 * @return 0 成功，-1 失败
 */
int model_set_input(model_t* model, int index, const void* data, size_t size);

/**
 * @brief 获取输出 Tensor 数据
 * @param model 模型句柄
 * @param index 输出索引
 * @param data 输出缓冲区
 * @param size 缓冲区大小 (输入/输出: 传入容量，传出实际大小)
 * @return 0 成功，-1 失败
 */
int model_get_output(model_t* model, int index, void* data, size_t* size);

/*===========================================================
 * 性能与状态
 *===========================================================*/

/**
 * @brief 获取上次推理耗时 (微秒)
 * @param model 模型句柄
 * @return 耗时 (us)，失败返回 -1
 */
int64_t model_get_last_inference_time_us(model_t* model);

/**
 * @brief 获取模型信息字符串 (JSON 格式)
 * @param model 模型句柄
 * @param buf 输出缓冲区
 * @param bufsize 缓冲区大小
 * @return 0 成功，-1 失败
 */
int model_get_info_json(model_t* model, char* buf, size_t bufsize);

/*===========================================================
 * 预/后处理辅助
 *===========================================================*/

/**
 * @brief 将 float 数组转换为量化 INT8 数据
 * @param float_data 浮点数据
 * @param count 元素个数
 * @param scale 量化比例
 * @param zero_point 零点
 * @param int8_out 输出缓冲区
 * @return 0 成功，-1 失败
 */
int model_quantize_f32_to_int8(const float* float_data, int count,
                                float scale, int zero_point, int8_t* int8_out);

/**
 * @brief 将量化 INT8 数据转换为 float 数组
 * @param int8_data 量化数据
 * @param count 元素个数
 * @param scale 量化比例
 * @param zero_point 零点
 * @param float_out 输出缓冲区
 * @return 0 成功，-1 失败
 */
int model_dequantize_int8_to_f32(const int8_t* int8_data, int count,
                                  float scale, int zero_point, float* float_out);

#ifdef __cplusplus
}
#endif

#endif /* IOT_MODEL_H */
