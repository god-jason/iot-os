/**
 * @file llm.h
 * @brief 大语言模型 API 封装
 *
 * 封装与主流大模型平台的 HTTP 通信，支持:
 *   - OpenAI 兼容 API (OpenAI / Azure / 通义千问 / DeepSeek 等)
 *   - 同步调用 (阻塞) 和 异步调用 (回调)
 *   - 流式 (SSE) 和非流式响应
 *   - 多轮对话 (上下文管理)
 *   - 函数调用 (Function Calling / Tool Use)
 *   - JSON 模式 (Structured Output)
 *
 * 依赖:
 *   - modules/http (HTTP 客户端)
 *   - modules/jsonc (JSON 编解码)
 *
 * 平台要求: 支持 HTTP + JSON 的所有平台
 *
 * @author 杰神 & CodeBuddy
 * @date   2026.08.01
 */
#ifndef IOT_LLM_H
#define IOT_LLM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================
 * 类型定义
 *===========================================================*/

/* LLM 消息角色 */
typedef enum {
    LLM_ROLE_SYSTEM    = 0,
    LLM_ROLE_USER      = 1,
    LLM_ROLE_ASSISTANT = 2,
    LLM_ROLE_TOOL      = 3
} llm_role_t;

/* LLM 消息 */
typedef struct {
    llm_role_t role;        /* 角色 */
    char*      content;     /* 消息内容 (JSON 字符串) */
    char*      name;        /* 名称 (可选, 用于 tool role) */
    char*      tool_call_id;/* 工具调用 ID (可选) */
} llm_message_t;

/* LLM 工具定义 */
typedef struct {
    char* name;             /* 工具名称 */
    char* description;      /* 工具描述 */
    char* parameters;       /* 参数 JSON Schema 字符串 */
} llm_tool_t;

/* LLM 配置 */
typedef struct {
    char*    base_url;       /* API 端点 (如 "https://api.openai.com/v1") */
    char*    api_key;        /* API 密钥 */
    char*    model;          /* 模型名称 (如 "gpt-4o-mini") */
    float    temperature;    /* 温度 (0.0 ~ 2.0, 默认 0.7) */
    int      max_tokens;     /* 最大输出 token (默认 1024) */
    float    top_p;          /* nucleus 采样 (0.0 ~ 1.0) */
    int      timeout_ms;     /* 超时 (ms, 默认 30000) */
    bool     enable_json_mode; /* JSON 模式 (Structured Output) */
    char*    json_schema;    /* JSON Schema 字符串 (enable_json_mode 时使用) */
    char*    system_prompt;  /* 系统提示词 */
    char*    extra_headers;  /* 额外 HTTP 头 (JSON 格式, 如 {"X-Custom":"val"}) */
} llm_config_t;

/* LLM 客户端句柄 (不透明) */
typedef struct llm_client_ctx llm_client_t;

/* 对话上下文句柄 (不透明) */
typedef struct llm_conversation_ctx llm_conversation_t;

/* LLM 响应 */
typedef struct {
    char*     content;       /* 回复内容 */
    char*     finish_reason; /* 结束原因: "stop" / "length" / "tool_calls" */
    char*     tool_calls;    /* 工具调用请求 (JSON 数组) */
    int       prompt_tokens; /* 输入 token 数 */
    int       completion_tokens; /* 输出 token 数 */
    int       total_tokens;  /* 总 token 数 */
    int       status_code;   /* HTTP 状态码 */
    char*     error;         /* 错误信息 (NULL 表示成功) */
} llm_response_t;

/* 异步回调 */
typedef void (*llm_callback_t)(llm_response_t* response, void* user_data);

/* 流式回调 (每次收到一个 token 调用) */
typedef void (*llm_stream_callback_t)(const char* delta_content,
                                       const char* finish_reason,
                                       bool is_end,
                                       void* user_data);

/*===========================================================
 * 客户端管理
 *===========================================================*/

/**
 * @brief 创建 LLM 客户端
 * @param config 配置参数 (内部会拷贝, 调用者可释放)
 * @return 客户端句柄, 失败返回 NULL
 */
llm_client_t* llm_client_create(const llm_config_t* config);

/**
 * @brief 更新客户端配置
 * @param client 客户端句柄
 * @param config 新配置
 * @return 0 成功, -1 失败
 */
int llm_client_set_config(llm_client_t* client, const llm_config_t* config);

/**
 * @brief 释放 LLM 客户端
 * @param client 客户端句柄
 */
void llm_client_free(llm_client_t* client);

/**
 * @brief 获取可用模型列表 (需要 API 支持)
 * @param client 客户端句柄
 * @param models_json 输出: 模型列表 JSON 字符串 (调用者 free)
 * @return 0 成功, -1 失败
 */
int llm_client_list_models(llm_client_t* client, char** models_json);

/*===========================================================
 * 对话管理
 *===========================================================*/

/**
 * @brief 创建对话上下文
 * @param max_turns 最大保留轮次 (0 表示不限制)
 * @return 对话句柄
 */
llm_conversation_t* llm_conversation_create(int max_turns);

/**
 * @brief 添加消息到对话上下文
 * @param conv 对话句柄
 * @param role 角色
 * @param content 内容
 * @return 0 成功, -1 失败
 */
int llm_conversation_add(llm_conversation_t* conv, llm_role_t role,
                          const char* content);

/**
 * @brief 获取对话上下文中的消息 (构建请求 body 用)
 * @param conv 对话句柄
 * @param include_system 是否包含 system 消息
 * @return JSON 数组字符串 (内部管理, 不可 free)
 */
const char* llm_conversation_get_messages_json(llm_conversation_t* conv,
                                                bool include_system);

/**
 * @brief 清空对话上下文 (保留 system prompt)
 * @param conv 对话句柄
 */
void llm_conversation_clear(llm_conversation_t* conv);

/**
 * @brief 释放对话上下文
 * @param conv 对话句柄
 */
void llm_conversation_free(llm_conversation_t* conv);

/*===========================================================
 * Chat 接口 - 同步
 *===========================================================*/

/**
 * @brief 同步聊天 (阻塞)
 * @param client 客户端句柄
 * @param messages_json 消息 JSON 数组
 * @param tools 工具数组 (可为 NULL)
 * @param tool_count 工具数量
 * @param response 输出响应 (调用者需 llm_response_free)
 * @return 0 成功, -1 失败
 */
int llm_chat(llm_client_t* client, const char* messages_json,
             const llm_tool_t* tools, int tool_count,
             llm_response_t* response);

/**
 * @brief 同步聊天 (使用对话上下文)
 * @param client 客户端句柄
 * @param conv 对话上下文
 * @param tools 工具数组 (可为 NULL)
 * @param tool_count 工具数量
 * @param response 输出响应
 * @return 0 成功, -1 失败
 */
int llm_chat_with_context(llm_client_t* client, llm_conversation_t* conv,
                           const llm_tool_t* tools, int tool_count,
                           llm_response_t* response);

/**
 * @brief 单轮问答 (简便接口)
 * @param client 客户端句柄
 * @param user_message 用户消息
 * @param response 输出响应
 * @return 0 成功, -1 失败
 */
int llm_ask(llm_client_t* client, const char* user_message,
            llm_response_t* response);

/*===========================================================
 * Chat 接口 - 异步
 *===========================================================*/

/**
 * @brief 异步聊天 (非阻塞)
 * @param client 客户端句柄
 * @param messages_json 消息 JSON
 * @param tools 工具数组
 * @param tool_count 工具数量
 * @param callback 完成回调
 * @param user_data 用户数据
 * @return 0 已提交, -1 失败
 */
int llm_chat_async(llm_client_t* client, const char* messages_json,
                    const llm_tool_t* tools, int tool_count,
                    llm_callback_t callback, void* user_data);

/**
 * @brief 流式聊天
 * @param client 客户端句柄
 * @param messages_json 消息 JSON
 * @param tools 工具数组
 * @param tool_count 工具数量
 * @param stream_cb 流式回调 (每个 token)
 * @param user_data 用户数据
 * @return 0 已提交, -1 失败
 */
int llm_chat_stream(llm_client_t* client, const char* messages_json,
                     const llm_tool_t* tools, int tool_count,
                     llm_stream_callback_t stream_cb, void* user_data);

/**
 * @brief 流式聊天 (使用对话上下文)
 * @param client 客户端句柄
 * @param conv 对话上下文
 * @param tools 工具数组
 * @param tool_count 工具数量
 * @param stream_cb 流式回调
 * @param user_data 用户数据
 * @return 0 已提交, -1 失败
 */
int llm_chat_stream_with_context(llm_client_t* client, llm_conversation_t* conv,
                                  const llm_tool_t* tools, int tool_count,
                                  llm_stream_callback_t stream_cb, void* user_data);

/*===========================================================
 * 辅助接口
 *===========================================================*/

/**
 * @brief 释放 LLM 响应资源
 * @param response 响应指针
 */
void llm_response_free(llm_response_t* response);

/**
 * @brief 获取角色名称字符串
 * @param role 角色
 * @return 名称字符串 (如 "user", "assistant")
 */
const char* llm_role_name(llm_role_t role);

/**
 * @brief 构建单条消息 JSON
 * @param role 角色
 * @param content 内容
 * @param buf 输出缓冲区
 * @param bufsize 缓冲区大小
 * @return 0 成功, -1 失败
 */
int llm_message_to_json(llm_role_t role, const char* content,
                         char* buf, size_t bufsize);

/**
 * @brief 创建默认配置
 * @param api_key API 密钥
 * @param model 模型名称
 * @return 默认配置 (静态, 不可 free)
 */
const llm_config_t* llm_default_config(const char* api_key, const char* model);

#ifdef __cplusplus
}
#endif

#endif /* IOT_LLM_H */
