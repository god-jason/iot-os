/**
 * @file model.c
 * @brief 深度学习模型加载与推理实现
 *
 * 平台后端映射:
 *   - ESP32 系列: 使用 TensorFlow Lite Micro 库
 *     * 集成方式: 在 platform xmake.lua 中添加 tflite-micro 依赖
 *     * 模型格式: .tflite (flatbuffer)
 *     * 预处理: 通过 model_quantize_* 辅助函数在 Lua 层完成
 *
 *   - Linux / Yopen: 优先使用 ONNX Runtime，次选 TFLite
 *     * 集成方式: 系统包管理器安装或源码编译
 *     * 模型格式: .onnx / .tflite
 *
 *   - Windows: 优先使用 ONNX Runtime，次选 TFLite
 *     * 集成方式: vcpkg 或预编译二进制
 *
 *   - WASM / 其他 MCU: 无硬件加速能力，返回 MODEL_BACKEND_NONE
 *
 * 内部架构:
 *   model_ctx 结构体内部持有一个 void* backend_ctx 指向具体后端上下文。
 *   初始化时根据平台宏自动选择后端，通过函数指针表实现多态。
 *
 * @author 杰神 & CodeBuddy
 * @date   2026.08.01
 */
#include "model.h"
#include "iot_log.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* 平台判定 */
#if defined(PLATFORM_ESP32) || defined(PLATFORM_ESP32C3) || \
    defined(PLATFORM_ESP32S3) || defined(PLATFORM_ESP32C2)
  #define HAS_TFLITE_MICRO   1
  #define DEFAULT_BACKEND    MODEL_BACKEND_TFLITE
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_YOPEN) || \
      defined(PLATFORM_WINDOWS) || defined(PLATFORM_WIN32)
  /* 桌面/Linux 平台优先 ONNX，也可选择 TFLite */
  #define HAS_ONNX_RUNTIME   1
  #define HAS_TFLITE         1
  #define DEFAULT_BACKEND    MODEL_BACKEND_ONNX
#else
  #define DEFAULT_BACKEND    MODEL_BACKEND_NONE
#endif

/*===========================================================
 * 后端函数指针表 (虚函数表)
 *===========================================================*/

typedef int  (*backend_load_fn)(void* ctx, const uint8_t* data, size_t size,
                                model_precision_t precision);
typedef void (*backend_free_fn)(void* ctx);
typedef int  (*backend_invoke_fn)(void* ctx);
typedef int  (*backend_tensor_count_fn)(void* ctx, int is_input);
typedef int  (*backend_tensor_info_fn)(void* ctx, int index, int is_input,
                                       model_tensor_info_t* info);
typedef int  (*backend_set_input_fn)(void* ctx, int index,
                                     const void* data, size_t size);
typedef int  (*backend_get_output_fn)(void* ctx, int index,
                                      void* data, size_t* size);

typedef struct {
    backend_load_fn        load;
    backend_free_fn        free;
    backend_invoke_fn      invoke;
    backend_tensor_count_fn tensor_count;
    backend_tensor_info_fn  tensor_info;
    backend_set_input_fn    set_input;
    backend_get_output_fn   get_output;
} model_backend_ops_t;

/*===========================================================
 * 模型上下文结构
 *===========================================================*/

struct model_ctx {
    model_backend_t    backend_type;   /* 后端类型 */
    model_precision_t  precision;      /* 精度 */
    void*              backend_ctx;    /* 后端私有上下文 */
    model_backend_ops_t ops;           /* 操作函数表 */
    int64_t            last_infer_us;  /* 上次推理耗时 */
    char               model_name[64]; /* 模型名称 (从路径提取) */
    size_t             model_size;     /* 模型数据大小 */
};

/*===========================================================
 * 内部工具函数
 *===========================================================*/

static void extract_model_name(const char* path, char* out, size_t out_size)
{
    const char* filename = strrchr(path, '/');
    if (!filename) filename = strrchr(path, '\\');
    filename = filename ? filename + 1 : path;
    snprintf(out, out_size, "%s", filename);
}

static int64_t get_time_us(void)
{
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_WIN32)
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (counter.QuadPart * 1000000LL) / freq.QuadPart;
#elif defined(PLATFORM_ESP32) || defined(PLATFORM_ESP32C3) || \
      defined(PLATFORM_ESP32S3) || defined(PLATFORM_ESP32C2)
    return (int64_t)esp_timer_get_time();
#else
    /* POSIX */
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (int64_t)ts.tv_sec * 1000000LL + ts.tv_nsec / 1000;
#endif
}

/*===========================================================
 * 后端实现 - 空后端 (无硬件支持)
 *===========================================================*/

static int backend_none_load(void* ctx, const uint8_t* data, size_t size,
                             model_precision_t precision)
{
    (void)ctx; (void)data; (void)size; (void)precision;
    LOG_ERROR("model", "No inference backend available on this platform");
    return -1;
}

static void backend_none_free(void* ctx) { (void)ctx; }
static int backend_none_invoke(void* ctx) { (void)ctx; return -1; }
static int backend_none_tensor_count(void* ctx, int is_input)
{ (void)ctx; (void)is_input; return 0; }
static int backend_none_tensor_info(void* ctx, int index, int is_input,
                                    model_tensor_info_t* info)
{ (void)ctx; (void)index; (void)is_input; (void)info; return -1; }
static int backend_none_set_input(void* ctx, int index, const void* data,
                                   size_t size)
{ (void)ctx; (void)index; (void)data; (void)size; return -1; }
static int backend_none_get_output(void* ctx, int index, void* data,
                                    size_t* size)
{ (void)ctx; (void)index; (void)data; (void)size; return -1; }

static model_backend_ops_t backend_none_ops = {
    backend_none_load,
    backend_none_free,
    backend_none_invoke,
    backend_none_tensor_count,
    backend_none_tensor_info,
    backend_none_set_input,
    backend_none_get_output
};

/*===========================================================
 * 后端实现 - TFLite Micro (ESP32)
 *
 * 集成说明 (ESP32 平台):
 *   1. 在 platform/esp32 的 xmake.lua 中添加:
 *      add_requires("tflite-micro")
 *      add_packages("tflite-micro")
 *   2. 包含 tflite-micro 相关头文件
 *   3. 模型需预先量化 (INT8 全整数量化)
 *   4. 使用 esp_partition API 或 iotfs 读取模型文件
 *===========================================================*/

#if HAS_TFLITE_MICRO || HAS_TFLITE

/* 后端上下文 */
typedef struct {
    const uint8_t* model_data;      /* 模型数据 */
    size_t         model_size;      /* 模型大小 */
    void*          interpreter;     /* TFLite Micro 解释器 */
    void*          input_tensor;    /* 输入 Tensor 指针 */
    void*          output_tensor;   /* 输出 Tensor 指针 */
    uint8_t*       arena;           /* Tensor Arena (工作内存) */
    size_t         arena_size;      /* Arena 大小 */
} tf_backend_ctx_t;

#define TFLITE_ARENA_SIZE  (128 * 1024)  /* 默认 128KB tensor arena */

static int backend_tflite_load(void* ctx_p, const uint8_t* data, size_t size,
                               model_precision_t precision)
{
    tf_backend_ctx_t* ctx = (tf_backend_ctx_t*)ctx_p;
    (void)precision;

    ctx->model_data = data;
    ctx->model_size = size;

    /* 分配 Tensor Arena */
    ctx->arena_size = TFLITE_ARENA_SIZE;
    ctx->arena = (uint8_t*)calloc(1, ctx->arena_size);
    if (!ctx->arena) {
        LOG_ERROR("model", "TFLite: failed to allocate arena (%zu bytes)",
                  ctx->arena_size);
        return -1;
    }

    /*
     * TODO: 实际集成 TFLite Micro 时的代码
     *
     * const tflite::Model* model = tflite::GetModel(data);
     * if (model->version() != TFLITE_SCHEMA_VERSION) {
     *     LOG_ERROR("model", "TFLite: schema version mismatch");
     *     return -1;
     * }
     *
     * static tflite::AllOpsResolver resolver;
     * static tflite::MicroInterpreter static_interpreter(
     *     model, resolver, ctx->arena, ctx->arena_size);
     * ctx->interpreter = &static_interpreter;
     *
     * TfLiteStatus status = interpreter->AllocateTensors();
     * if (status != kTfLiteOk) {
     *     LOG_ERROR("model", "TFLite: tensor allocation failed");
     *     return -1;
     * }
     *
     * ctx->input_tensor = interpreter->input(0);
     * ctx->output_tensor = interpreter->output(0);
     */

    LOG_INFO("model", "TFLite: model loaded (%zu bytes), arena=%zu bytes",
             size, ctx->arena_size);
    return 0;
}

static void backend_tflite_free(void* ctx_p)
{
    tf_backend_ctx_t* ctx = (tf_backend_ctx_t*)ctx_p;
    if (ctx->arena) {
        free(ctx->arena);
        ctx->arena = NULL;
    }
    memset(ctx, 0, sizeof(tf_backend_ctx_t));
}

static int backend_tflite_invoke(void* ctx_p)
{
    tf_backend_ctx_t* ctx = (tf_backend_ctx_t*)ctx_p;
    (void)ctx;

    /*
     * TODO: 实际集成
     * TfLiteStatus status = interpreter->Invoke();
     * return (status == kTfLiteOk) ? 0 : -1;
     */
    LOG_DEBUG("model", "TFLite: invoke (stub)");
    return 0;
}

static int backend_tflite_tensor_count(void* ctx_p, int is_input)
{
    (void)ctx_p;
    /* 简化: 返回单输入单输出 */
    return 1;
}

static int backend_tflite_tensor_info(void* ctx_p, int index, int is_input,
                                       model_tensor_info_t* info)
{
    tf_backend_ctx_t* ctx = (tf_backend_ctx_t*)ctx_p;
    (void)ctx;

    if (index != 0 || !info) return -1;

    memset(info, 0, sizeof(model_tensor_info_t));
    snprintf(info->name, sizeof(info->name), "%s", is_input ? "input" : "output");

    /*
     * TODO: 实际集成
     * TfLiteTensor* tensor = is_input ? interpreter->input(index)
     *                                 : interpreter->output(index);
     * info->ndim = tensor->dims->size;
     * for (int i = 0; i < tensor->dims->size; i++)
     *     info->dims[i] = tensor->dims->data[i];
     * info->size_bytes = tensor->bytes;
     * info->data = tensor->data.data;
     */

    /* 占位维度: [1, 128] */
    info->ndim = 2;
    info->dims[0] = 1;
    info->dims[1] = 128;
    info->dtype = MODEL_DTYPE_UINT8;
    info->size_bytes = 128;

    return 0;
}

static int backend_tflite_set_input(void* ctx_p, int index,
                                     const void* data, size_t size)
{
    tf_backend_ctx_t* ctx = (tf_backend_ctx_t*)ctx_p;
    if (index != 0 || !data) return -1;

    /*
     * TODO: 实际集成
     * TfLiteTensor* input = interpreter->input(index);
     * if (size > (size_t)input->bytes) return -1;
     * memcpy(input->data.uint8, data, size);
     */
    (void)ctx;
    LOG_DEBUG("model", "TFLite: set_input (stub, %zu bytes)", size);
    return 0;
}

static int backend_tflite_get_output(void* ctx_p, int index,
                                      void* data, size_t* size)
{
    tf_backend_ctx_t* ctx = (tf_backend_ctx_t*)ctx_p;
    if (index != 0 || !data || !size) return -1;

    /*
     * TODO: 实际集成
     * TfLiteTensor* output = interpreter->output(index);
     * size_t copy_size = (*size < output->bytes) ? *size : output->bytes;
     * memcpy(data, output->data.uint8, copy_size);
     * *size = copy_size;
     */
    (void)ctx;
    memset(data, 0, *size);
    return 0;
}

static model_backend_ops_t backend_tflite_ops = {
    backend_tflite_load,
    backend_tflite_free,
    backend_tflite_invoke,
    backend_tflite_tensor_count,
    backend_tflite_tensor_info,
    backend_tflite_set_input,
    backend_tflite_get_output
};

#endif /* HAS_TFLITE_MICRO || HAS_TFLITE */

/*===========================================================
 * 后端实现 - ONNX Runtime (Linux / Windows)
 *
 * 集成说明:
 *   1. 安装 ONNX Runtime:
 *      Linux:   sudo apt install libonnxruntime-dev
 *      Windows: vcpkg install onnxruntime
 *   2. 包含头文件: #include <onnxruntime/core/session/onnxruntime_c_api.h>
 *   3. 链接: -lonnxruntime
 *
 * ONNX Runtime API 使用:
 *   - OrtCreateEnv / OrtCreateSession / OrtRun
 *   - 支持 CPU 和 GPU 后端 (CUDA / DirectML / OpenVINO)
 *===========================================================*/

#if HAS_ONNX_RUNTIME

typedef struct {
    void*           ort_env;        /* OrtEnv */
    void*           ort_session;    /* OrtSession */
    void*           ort_allocator;  /* OrtAllocator */
    void*           ort_memory_info;/* OrtMemoryInfo */
    char*           input_names[8];
    char*           output_names[8];
    int             input_count;
    int             output_count;
    size_t*         input_sizes;
    size_t*         output_sizes;
    uint8_t**       input_buffers;
    uint8_t**       output_buffers;
    const uint8_t*  model_data;     /* 模型文件数据 */
    size_t          model_size;
} onnx_backend_ctx_t;

static int backend_onnx_load(void* ctx_p, const uint8_t* data, size_t size,
                             model_precision_t precision)
{
    onnx_backend_ctx_t* ctx = (onnx_backend_ctx_t*)ctx_p;
    (void)precision;

    ctx->model_data = data;
    ctx->model_size = size;

    /*
     * TODO: 实际集成 ONNX Runtime 时的代码
     *
     * OrtStatus* status;
     * const OrtApi* api = OrtGetApiBase()->GetApi(ORT_API_VERSION);
     *
     * // 创建环境
     * api->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "iot-model", &ctx->ort_env);
     *
     * // 创建会话选项
     * OrtSessionOptions* session_options;
     * api->CreateSessionOptions(&session_options);
     * api->SetSessionGraphOptimizationLevel(session_options,
     *     ORT_ENABLE_ALL);
     * api->SetIntraOpNumThreads(session_options, 1);
     *
     * // 从内存创建会话
     * api->CreateSession(ctx->ort_env, data, size,
     *                    session_options, &ctx->ort_session);
     * api->ReleaseSessionOptions(session_options);
     *
     * // 获取输入输出数量
     * api->SessionGetInputCount(ctx->ort_session, &ctx->input_count);
     * api->SessionGetOutputCount(ctx->ort_session, &ctx->output_count);
     *
     * // 分配缓冲区
     * ctx->input_buffers = calloc(ctx->input_count, sizeof(uint8_t*));
     * ctx->output_buffers = calloc(ctx->output_count, sizeof(uint8_t*));
     * // ... 获取每个 tensor 的大小并分配 ...
     */

    LOG_INFO("model", "ONNX: model loaded (%zu bytes)", size);
    return 0;
}

static void backend_onnx_free(void* ctx_p)
{
    onnx_backend_ctx_t* ctx = (onnx_backend_ctx_t*)ctx_p;

    /*
     * TODO: 释放 ONNX 资源
     * api->ReleaseSession(ctx->ort_session);
     * api->ReleaseEnv(ctx->ort_env);
     */

    if (ctx->input_buffers) {
        for (int i = 0; i < ctx->input_count; i++)
            free(ctx->input_buffers[i]);
        free(ctx->input_buffers);
    }
    if (ctx->output_buffers) {
        for (int i = 0; i < ctx->output_count; i++)
            free(ctx->output_buffers[i]);
        free(ctx->output_buffers);
    }
    memset(ctx, 0, sizeof(onnx_backend_ctx_t));
}

static int backend_onnx_invoke(void* ctx_p)
{
    onnx_backend_ctx_t* ctx = (onnx_backend_ctx_t*)ctx_p;
    (void)ctx;

    /*
     * TODO: 实际集成
     * api->Run(ctx->ort_session, NULL,
     *          ctx->input_names, ctx->input_tensors, ctx->input_count,
     *          ctx->output_names, ctx->output_count, ctx->output_tensors);
     */
    LOG_DEBUG("model", "ONNX: invoke (stub)");
    return 0;
}

static int backend_onnx_tensor_count(void* ctx_p, int is_input)
{
    onnx_backend_ctx_t* ctx = (onnx_backend_ctx_t*)ctx_p;
    return is_input ? ctx->input_count : ctx->output_count;
}

static int backend_onnx_tensor_info(void* ctx_p, int index, int is_input,
                                     model_tensor_info_t* info)
{
    onnx_backend_ctx_t* ctx = (onnx_backend_ctx_t*)ctx_p;
    (void)ctx;

    if (!info || index < 0) return -1;

    memset(info, 0, sizeof(model_tensor_info_t));
    snprintf(info->name, sizeof(info->name), "%s_%d",
             is_input ? "input" : "output", index);

    /*
     * TODO: 实际集成
     * OrtTypeInfo* type_info;
     * api->SessionGetInputTypeInfo(ctx->ort_session, index, &type_info);
     * // 解析 type_info 获取 shape 和 dtype
     */

    return 0;
}

static int backend_onnx_set_input(void* ctx_p, int index,
                                   const void* data, size_t size)
{
    onnx_backend_ctx_t* ctx = (onnx_backend_ctx_t*)ctx_p;
    if (index < 0 || index >= ctx->input_count || !data) return -1;

    /*
     * TODO: 实际集成
     * if (size > ctx->input_sizes[index]) size = ctx->input_sizes[index];
     * memcpy(ctx->input_buffers[index], data, size);
     */
    LOG_DEBUG("model", "ONNX: set_input[%d] (stub, %zu bytes)", index, size);
    return 0;
}

static int backend_onnx_get_output(void* ctx_p, int index,
                                    void* data, size_t* size)
{
    onnx_backend_ctx_t* ctx = (onnx_backend_ctx_t*)ctx_p;
    if (index < 0 || index >= ctx->output_count || !data || !size) return -1;

    /*
     * TODO: 实际集成
     * size_t copy_size = (*size < ctx->output_sizes[index])
     *                  ? *size : ctx->output_sizes[index];
     * memcpy(data, ctx->output_buffers[index], copy_size);
     * *size = copy_size;
     */
    (void)ctx;
    memset(data, 0, *size);
    return 0;
}

static model_backend_ops_t backend_onnx_ops = {
    backend_onnx_load,
    backend_onnx_free,
    backend_onnx_invoke,
    backend_onnx_tensor_count,
    backend_onnx_tensor_info,
    backend_onnx_set_input,
    backend_onnx_get_output
};

#endif /* HAS_ONNX_RUNTIME */

/*===========================================================
 * 平台文件读取 (通过 iotfs 接口)
 *===========================================================*/

static uint8_t* read_file_data(const char* path, size_t* out_size)
{
    if (!path || !out_size) return NULL;

    FILE* fp = fopen(path, "rb");
    if (!fp) {
        LOG_ERROR("model", "Cannot open file: %s", path);
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    if (fsize <= 0) {
        fclose(fp);
        LOG_ERROR("model", "Empty or invalid file: %s", path);
        return NULL;
    }
    fseek(fp, 0, SEEK_SET);

    uint8_t* data = (uint8_t*)malloc((size_t)fsize);
    if (!data) {
        fclose(fp);
        LOG_ERROR("model", "OOM reading model file");
        return NULL;
    }

    size_t read = fread(data, 1, (size_t)fsize, fp);
    fclose(fp);

    if (read != (size_t)fsize) {
        free(data);
        LOG_ERROR("model", "Partial read: %zu / %ld bytes", read, fsize);
        return NULL;
    }

    *out_size = (size_t)fsize;
    LOG_INFO("model", "Read model file: %s (%zu bytes)", path, *out_size);
    return data;
}

/*===========================================================
 * 公共 API 实现
 *===========================================================*/

model_backend_t model_get_available_backend(void)
{
#if HAS_ONNX_RUNTIME
    return MODEL_BACKEND_ONNX;
#elif HAS_TFLITE_MICRO || HAS_TFLITE
    return MODEL_BACKEND_TFLITE;
#else
    return MODEL_BACKEND_NONE;
#endif
}

const char* model_backend_name(model_backend_t backend)
{
    static const char* names[] = {
        "auto", "tflite", "onnx", "none"
    };
    if (backend > MODEL_BACKEND_NONE) return "unknown";
    return names[backend];
}

model_t* model_load_from_file(const char* path, model_backend_t backend,
                               model_precision_t precision)
{
    if (!path) return NULL;

    size_t model_size = 0;
    uint8_t* model_data = read_file_data(path, &model_size);
    if (!model_data) return NULL;

    model_t* model = model_load_from_memory(model_data, model_size,
                                            backend, precision);
    if (!model) {
        free(model_data);
    } else {
        extract_model_name(path, model->model_name, sizeof(model->model_name));
    }
    /* model_data 所有权转移给后端，不要在这里 free */
    return model;
}

model_t* model_load_from_memory(const uint8_t* model_data, size_t model_size,
                                 model_backend_t backend, model_precision_t precision)
{
    if (!model_data || model_size == 0) return NULL;

    /* 自动选择后端 */
    if (backend == MODEL_BACKEND_AUTO) {
        backend = DEFAULT_BACKEND;
    }

    if (backend == MODEL_BACKEND_NONE) {
        LOG_ERROR("model", "No inference backend available");
        return NULL;
    }

    /* 分配模型上下文 */
    model_t* model = (model_t*)calloc(1, sizeof(model_t));
    if (!model) {
        LOG_ERROR("model", "OOM allocating model context");
        return NULL;
    }

    model->backend_type = backend;
    model->precision     = precision;
    model->model_size    = model_size;

    /* 选择后端并分配私有上下文 */
    switch (backend) {
#if HAS_TFLITE_MICRO || HAS_TFLITE
    case MODEL_BACKEND_TFLITE: {
        tf_backend_ctx_t* tf_ctx = (tf_backend_ctx_t*)calloc(1, sizeof(tf_backend_ctx_t));
        if (!tf_ctx) goto oom;
        model->backend_ctx = tf_ctx;
        model->ops = backend_tflite_ops;
        break;
    }
#endif
#if HAS_ONNX_RUNTIME
    case MODEL_BACKEND_ONNX: {
        onnx_backend_ctx_t* onnx_ctx = (onnx_backend_ctx_t*)calloc(1, sizeof(onnx_backend_ctx_t));
        if (!onnx_ctx) goto oom;
        model->backend_ctx = onnx_ctx;
        model->ops = backend_onnx_ops;
        break;
    }
#endif
    default:
        model->backend_ctx = NULL;
        model->ops = backend_none_ops;
        break;
    }

    /* 调用后端的 load */
    if (model->ops.load(model->backend_ctx, model_data,
                        model_size, precision) != 0) {
        LOG_ERROR("model", "Backend load failed");
        model->ops.free(model->backend_ctx);
        free(model->backend_ctx);
        free(model);
        return NULL;
    }

    LOG_INFO("model", "Model loaded: backend=%s, precision=%d, size=%zu",
             model_backend_name(backend), precision, model_size);
    return model;

oom:
    LOG_ERROR("model", "OOM allocating backend context");
    free(model);
    return NULL;
}

void model_free(model_t* model)
{
    if (!model) return;

    if (model->ops.free) {
        model->ops.free(model->backend_ctx);
    }
    free(model->backend_ctx);
    memset(model, 0, sizeof(model_t));
    free(model);
}

model_t* model_reload(model_t* model, const char* path)
{
    if (!model || !path) {
        if (model) model_free(model);
        return NULL;
    }

    model_backend_t backend = model->backend_type;
    model_precision_t precision = model->precision;
    model_free(model);

    return model_load_from_file(path, backend, precision);
}

int model_invoke(model_t* model)
{
    if (!model || !model->ops.invoke) return -1;

    int64_t start = get_time_us();
    int result = model->ops.invoke(model->backend_ctx);
    int64_t end = get_time_us();

    model->last_infer_us = end - start;
    return result;
}

int model_invoke_async(model_t* model,
                       void (*callback)(void* user_data, int result),
                       void* user_data)
{
    if (!model || !callback) return -1;

    /*
     * TODO: 使用平台的 task 机制异步运行
     * iot_task_create(task_name, invoke_task_func,
     *                 stack_size, priority, &task_ctx);
     * 其中 task_ctx 包含 model 指针和 callback
     * 完成后在 invoke_task_func 中调用 callback
     */
    LOG_WARN("model", "Async invoke not implemented, falling back to sync");
    int result = model_invoke(model);
    callback(user_data, result);
    return 0;
}

int model_get_input_count(model_t* model)
{
    if (!model || !model->ops.tensor_count) return 0;
    return model->ops.tensor_count(model->backend_ctx, 1);
}

int model_get_output_count(model_t* model)
{
    if (!model || !model->ops.tensor_count) return 0;
    return model->ops.tensor_count(model->backend_ctx, 0);
}

int model_get_input_info(model_t* model, int index, model_tensor_info_t* info)
{
    if (!model || !model->ops.tensor_info) return -1;
    return model->ops.tensor_info(model->backend_ctx, index, 1, info);
}

int model_get_output_info(model_t* model, int index, model_tensor_info_t* info)
{
    if (!model || !model->ops.tensor_info) return -1;
    return model->ops.tensor_info(model->backend_ctx, index, 0, info);
}

int model_set_input(model_t* model, int index, const void* data, size_t size)
{
    if (!model || !model->ops.set_input) return -1;
    return model->ops.set_input(model->backend_ctx, index, data, size);
}

int model_get_output(model_t* model, int index, void* data, size_t* size)
{
    if (!model || !model->ops.get_output) return -1;
    return model->ops.get_output(model->backend_ctx, index, data, size);
}

int64_t model_get_last_inference_time_us(model_t* model)
{
    if (!model) return -1;
    return model->last_infer_us;
}

int model_get_info_json(model_t* model, char* buf, size_t bufsize)
{
    if (!model || !buf || bufsize == 0) return -1;

    model_tensor_info_t in_info, out_info;
    int in_ok = model_get_input_info(model, 0, &in_info);
    int out_ok = model_get_output_info(model, 0, &out_info);

    int n = snprintf(buf, bufsize,
        "{"
        "\"backend\":\"%s\","
        "\"precision\":%d,"
        "\"model_size\":%zu,"
        "\"last_infer_us\":%lld,"
        "\"inputs\":[%s],"
        "\"outputs\":[%s]"
        "}",
        model_backend_name(model->backend_type),
        model->precision,
        model->model_size,
        (long long)model->last_infer_us,
        in_ok == 0 ?
            (snprintf(NULL, 0,
                "{\"name\":\"%s\",\"dtype\":%d,\"ndim\":%d,\"dims\":[%d,%d,%d,%d],\"size\":%zu}",
                in_info.name, in_info.dtype, in_info.ndim,
                in_info.ndim > 0 ? in_info.dims[0] : 0,
                in_info.ndim > 1 ? in_info.dims[1] : 0,
                in_info.ndim > 2 ? in_info.dims[2] : 0,
                in_info.ndim > 3 ? in_info.dims[3] : 0,
                in_info.size_bytes) ? NULL : "")
            : "\"unknown\"",
        out_ok == 0 ?
            (snprintf(NULL, 0,
                "{\"name\":\"%s\",\"dtype\":%d,\"ndim\":%d,\"dims\":[%d,%d,%d,%d],\"size\":%zu}",
                out_info.name, out_info.dtype, out_info.ndim,
                out_info.ndim > 0 ? out_info.dims[0] : 0,
                out_info.ndim > 1 ? out_info.dims[1] : 0,
                out_info.ndim > 2 ? out_info.dims[2] : 0,
                out_info.ndim > 3 ? out_info.dims[3] : 0,
                out_info.size_bytes) ? NULL : "")
            : "\"unknown\""
    );

    return (n > 0 && (size_t)n < bufsize) ? 0 : -1;
}

/*===========================================================
 * 量化辅助函数
 *===========================================================*/

int model_quantize_f32_to_int8(const float* float_data, int count,
                                float scale, int zero_point, int8_t* int8_out)
{
    if (!float_data || !int8_out || count <= 0) return -1;
    if (scale <= 0.0f) return -1;

    for (int i = 0; i < count; i++) {
        float q = roundf(float_data[i] / scale) + (float)zero_point;
        if (q > 127.0f) q = 127.0f;
        if (q < -128.0f) q = -128.0f;
        int8_out[i] = (int8_t)q;
    }
    return 0;
}

int model_dequantize_int8_to_f32(const int8_t* int8_data, int count,
                                  float scale, int zero_point, float* float_out)
{
    if (!int8_data || !float_out || count <= 0) return -1;
    if (scale <= 0.0f) return -1;

    for (int i = 0; i < count; i++) {
        float_out[i] = scale * ((float)int8_data[i] - (float)zero_point);
    }
    return 0;
}
