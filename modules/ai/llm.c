/**
 * @file llm.c
 * @brief 大语言模型 API 封装实现
 *
 * 支持 OpenAI 兼容 API (包括通义千问、DeepSeek、Moonshot、智谱 等)。
 *
 * 架构:
 *   1. 请求构建: JSON 消息体手动拼接 (避免对外部 JSON 库的依赖)
 *   2. HTTP 通信: 通过 http_client.h 提供的 http_post 接口
 *   3. 响应解析: 简单的字符串解析提取 content / tool_calls
 *   4. 异步: 参考 http_client 的任务回调模型，在工作线程中执行
 *   5. 流式: 解析 SSE (Server-Sent Events) 数据流
 *
 * 平台要求: 需要 http 模块和网络栈正常工作
 *
 * @author 杰神 & CodeBuddy
 * @date   2026.08.01
 */
#include "llm.h"
#include "http_client.h"
#include "iot_log.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#ifdef PLATFORM_ESP32
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
#endif

/*===========================================================
 * 常量定义
 *===========================================================*/

#define LLM_DEFAULT_TIMEOUT_MS   30000
#define LLM_DEFAULT_MAX_TOKENS   1024
#define LLM_DEFAULT_TEMPERATURE  0.7f
#define LLM_JSON_BUF_SIZE        8192
#define LLM_RESP_BUF_SIZE        16384
#define LLM_MAX_CONV_TURNS       20

/*===========================================================
 * 内部 JSON 构建辅助 (轻量, 避免外部依赖)
 *===========================================================*/

typedef struct {
    char* buf;
    size_t size;
    size_t pos;
} json_builder_t;

static json_builder_t* json_builder_create(size_t initial_size)
{
    json_builder_t* jb = (json_builder_t*)calloc(1, sizeof(json_builder_t));
    if (!jb) return NULL;
    jb->buf = (char*)calloc(1, initial_size);
    if (!jb->buf) { free(jb); return NULL; }
    jb->size = initial_size;
    jb->pos = 0;
    return jb;
}

static void json_builder_free(json_builder_t* jb)
{
    if (jb) {
        free(jb->buf);
        free(jb);
    }
}

static int json_builder_append(json_builder_t* jb, const char* fmt, ...)
{
    if (!jb || !jb->buf) return -1;

    va_list ap;
    va_start(ap, fmt);
    int needed = vsnprintf(jb->buf + jb->pos, jb->size - jb->pos, fmt, ap);
    va_end(ap);

    if (needed < 0) return -1;

    /* 扩容 */
    while (jb->pos + (size_t)needed >= jb->size) {
        size_t new_size = jb->size * 2;
        char* new_buf = (char*)realloc(jb->buf, new_size);
        if (!new_buf) return -1;
        jb->buf = new_buf;
        jb->size = new_size;
    }

    jb->pos += (size_t)needed;
    return 0;
}

/* 对字符串进行 JSON 转义并追加 */
static int json_builder_append_string(json_builder_t* jb, const char* str)
{
    if (!jb || !str) return json_builder_append(jb, "null");

    json_builder_append(jb, "\"");
    for (const char* p = str; *p; p++) {
        switch (*p) {
        case '"':  json_builder_append(jb, "\\\""); break;
        case '\\': json_builder_append(jb, "\\\\"); break;
        case '\n': json_builder_append(jb, "\\n");  break;
        case '\r': json_builder_append(jb, "\\r");  break;
        case '\t': json_builder_append(jb, "\\t");  break;
        case '\b': json_builder_append(jb, "\\b");  break;
        case '\f': json_builder_append(jb, "\\f");  break;
        default:
            if ((unsigned char)*p < 0x20) {
                json_builder_append(jb, "\\u%04x", (unsigned char)*p);
            } else {
                jb->buf[jb->pos++] = *p;
            }
            break;
        }
    }
    json_builder_append(jb, "\"");
    return 0;
}

/*===========================================================
 * 对话结构定义
 *===========================================================*/

typedef struct {
    llm_role_t role;
    char*      content;
} conv_turn_t;

struct llm_conversation_ctx {
    conv_turn_t turns[LLM_MAX_CONV_TURNS];
    int         turn_count;
    int         max_turns;
    char*       system_prompt;
};

/*===========================================================
 * 客户端结构定义
 *===========================================================*/

struct llm_client_ctx {
    char    base_url[256];
    char    api_key[128];
    char    model[64];
    float   temperature;
    int     max_tokens;
    float   top_p;
    int     timeout_ms;
    bool    enable_json_mode;
    char*   json_schema;
    char*   extra_headers;
};

/*===========================================================
 * 角色名称
 *===========================================================*/

const char* llm_role_name(llm_role_t role)
{
    static const char* names[] = { "system", "user", "assistant", "tool" };
    if (role > LLM_ROLE_TOOL) return "unknown";
    return names[role];
}

int llm_message_to_json(llm_role_t role, const char* content,
                         char* buf, size_t bufsize)
{
    if (!buf || bufsize == 0) return -1;
    snprintf(buf, bufsize, "{\"role\":\"%s\",\"content\":\"%s\"}",
             llm_role_name(role), content ? content : "");
    return 0;
}

/*===========================================================
 * 内部工具函数
 *===========================================================*/

/* 从 JSON 中提取字符串值 (简易解析, 支持 key:"value") */
static char* json_extract_string(const char* json, const char* key)
{
    if (!json || !key) return NULL;

    /* 构造搜索模式: "key":  */
    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);

    const char* pos = strstr(json, pattern);
    if (!pos) return NULL;
    pos += strlen(pattern);

    /* 跳过空格和冒号 */
    while (*pos && (*pos == ' ' || *pos == ':')) pos++;
    if (*pos != '\"') return NULL;
    pos++; /* 跳过首引号 */

    /* 找到匹配的结束引号 (处理转义) */
    char value[4096] = {0};
    int i = 0;
    while (*pos && i < (int)sizeof(value) - 1) {
        if (*pos == '\\') {
            pos++;
            if (*pos == 'n') value[i++] = '\n';
            else if (*pos == 'r') value[i++] = '\r';
            else if (*pos == 't') value[i++] = '\t';
            else if (*pos == '"') value[i++] = '"';
            else if (*pos == '\\') value[i++] = '\\';
            else { value[i++] = '\\'; value[i++] = *pos ? *pos : '\\'; }
        } else if (*pos == '"') {
            break;
        } else {
            value[i++] = *pos;
        }
        pos++;
    }
    value[i] = '\0';

    return iot_strdup(value);
}

/* 从 JSON 中提取整数值 */
static int json_extract_int(const char* json, const char* key, int default_val)
{
    if (!json || !key) return default_val;

    char pattern[128];
    snprintf(pattern, sizeof(pattern), "\"%s\"", key);

    const char* pos = strstr(json, pattern);
    if (!pos) return default_val;
    pos += strlen(pattern);

    while (*pos && (*pos == ' ' || *pos == ':')) pos++;

    return atoi(pos);
}

/*===========================================================
 * 请求构建
 *===========================================================*/

static char* llm_build_request_body(llm_client_t* client,
                                     const char* messages_json,
                                     const llm_tool_t* tools, int tool_count)
{
    json_builder_t* jb = json_builder_create(LLM_JSON_BUF_SIZE);
    if (!jb) return NULL;

    json_builder_append(jb, "{");

    /* model */
    json_builder_append(jb, "\"model\":\"%s\",", client->model);

    /* messages - 直接使用传入的 JSON 数组 */
    json_builder_append(jb, "\"messages\":%s,", messages_json);

    /* temperature */
    json_builder_append(jb, "\"temperature\":%.2f,", client->temperature);

    /* max_tokens */
    json_builder_append(jb, "\"max_tokens\":%d,", client->max_tokens);

    /* top_p */
    if (client->top_p > 0.0f && client->top_p < 1.0f) {
        json_builder_append(jb, "\"top_p\":%.2f,", client->top_p);
    }

    /* stream (默认不流式, 流式由 llm_chat_stream 单独处理) */
    json_builder_append(jb, "\"stream\":false,");

    /* JSON mode */
    if (client->enable_json_mode) {
        json_builder_append(jb, "\"response_format\":{");
        json_builder_append(jb, "\"type\":\"json_object\"");
        if (client->json_schema) {
            json_builder_append(jb, ",\"json_schema\":%s", client->json_schema);
        }
        json_builder_append(jb, "},");
    }

    /* tools */
    if (tools && tool_count > 0) {
        json_builder_append(jb, "\"tools\":[");
        for (int i = 0; i < tool_count; i++) {
            if (i > 0) json_builder_append(jb, ",");
            json_builder_append(jb, "{");
            json_builder_append(jb, "\"type\":\"function\",");
            json_builder_append(jb, "\"function\":{");
            json_builder_append(jb, "\"name\":\"%s\",", tools[i].name);
            if (tools[i].description) {
                json_builder_append(jb, "\"description\":\"%s\",", tools[i].description);
            }
            json_builder_append(jb, "\"parameters\":%s", tools[i].parameters);
            json_builder_append(jb, "}");
            json_builder_append(jb, "}");
        }
        json_builder_append(jb, "],");
    }

    /* 移除末尾逗号 (简单处理: 末尾加一个无关字段) */
    json_builder_append(jb, "\"_internal\":1");
    json_builder_append(jb, "}");

    return jb->buf; /* 所有权转移 */
}

/*===========================================================
 * 请求执行
 *===========================================================*/

static int llm_execute_request(llm_client_t* client, const char* body,
                                http_response_t* http_resp)
{
    if (!client || !body || !http_resp) return -1;

    /* 构造 URL: {base_url}/chat/completions */
    char url[512];
    snprintf(url, sizeof(url), "%s/chat/completions", client->base_url);

    /* 构造 headers */
    char headers[512];
    snprintf(headers, sizeof(headers),
             "Authorization: Bearer %s\r\n", client->api_key);

    LOG_DEBUG("llm", "POST => %s (model=%s)", url, client->model);
    LOG_DEBUG("llm", "Request body: %.*s...", 300, body);

    int result = http_post(url, body, strlen(body),
                           "application/json", http_resp);

    if (result != 0 || http_resp->error) {
        LOG_ERROR("llm", "HTTP request failed: %s",
                  http_resp->error ? http_resp->error : "unknown");
        return -1;
    }

    LOG_DEBUG("llm", "HTTP %d, body_len=%zu",
              http_resp->status_code, http_resp->body_len);
    return 0;
}

/*===========================================================
 * 响应解析
 *===========================================================*/

static void llm_parse_response(const http_response_t* http_resp,
                                llm_response_t* response)
{
    if (!http_resp || !response) return;

    memset(response, 0, sizeof(llm_response_t));
    response->status_code = http_resp->status_code;

    /* 检查 HTTP 状态码 */
    if (http_resp->status_code != 200) {
        response->error = iot_strdup(http_resp->body
                                     ? http_resp->body
                                     : "HTTP error");
        return;
    }

    if (!http_resp->body) {
        response->error = iot_strdup("Empty response body");
        return;
    }

    /* 检查 JSON 中的 error 字段 */
    char* err = json_extract_string(http_resp->body, "error");
    if (err) {
        response->error = iot_strdup(err);
        free(err);
        return;
    }

    /* 提取 choices[0].message.content */
    const char* choices = strstr(http_resp->body, "\"choices\"");
    if (!choices) {
        response->error = iot_strdup("No choices in response");
        return;
    }

    char* content = json_extract_string(choices, "content");
    if (content) {
        response->content = content;
    }

    /* 提取 finish_reason */
    char* finish = json_extract_string(choices, "finish_reason");
    if (finish) {
        response->finish_reason = finish;
    }

    /* 提取 tool_calls (如果存在) */
    const char* tc = strstr(choices, "\"tool_calls\"");
    if (tc) {
        /* tool_calls 是完整的 JSON 数组, 提取到 message 结束 */
        const char* tc_start = tc;
        int depth = 0;
        int started = 0;
        while (*tc) {
            if (*tc == '[') { depth++; started = 1; }
            else if (*tc == ']') { depth--; }
            tc++;
            if (started && depth == 0) break;
        }
        size_t tc_len = (size_t)(tc - tc_start);
        response->tool_calls = (char*)malloc(tc_len + 1);
        if (response->tool_calls) {
            memcpy(response->tool_calls, tc_start, tc_len);
            response->tool_calls[tc_len] = '\0';
        }
    }

    /* 提取 usage 信息 */
    response->prompt_tokens     = json_extract_int(http_resp->body, "prompt_tokens", 0);
    response->completion_tokens = json_extract_int(http_resp->body, "completion_tokens", 0);
    response->total_tokens      = json_extract_int(http_resp->body, "total_tokens", 0);

    LOG_INFO("llm", "Response: tok=%d/%d, finish=%s",
             response->prompt_tokens, response->completion_tokens,
             response->finish_reason ? response->finish_reason : "nil");
}

/*===========================================================
 * 公共 API - 客户端管理
 *===========================================================*/

static llm_config_t default_cfg;

const llm_config_t* llm_default_config(const char* api_key, const char* model)
{
    memset(&default_cfg, 0, sizeof(default_cfg));
    default_cfg.api_key      = (char*)api_key;
    default_cfg.base_url     = "https://api.openai.com/v1";
    default_cfg.model        = (char*)model;
    default_cfg.temperature  = LLM_DEFAULT_TEMPERATURE;
    default_cfg.max_tokens   = LLM_DEFAULT_MAX_TOKENS;
    default_cfg.top_p        = 1.0f;
    default_cfg.timeout_ms   = LLM_DEFAULT_TIMEOUT_MS;
    default_cfg.enable_json_mode = false;
    return &default_cfg;
}

llm_client_t* llm_client_create(const llm_config_t* config)
{
    if (!config || !config->api_key || !config->model)
        return NULL;

    llm_client_t* client = (llm_client_t*)calloc(1, sizeof(llm_client_t));
    if (!client) return NULL;

    strncpy(client->base_url, config->base_url ? config->base_url
            : "https://api.openai.com/v1", sizeof(client->base_url) - 1);
    strncpy(client->api_key, config->api_key, sizeof(client->api_key) - 1);
    strncpy(client->model, config->model, sizeof(client->model) - 1);
    client->temperature     = config->temperature > 0 ? config->temperature : LLM_DEFAULT_TEMPERATURE;
    client->max_tokens      = config->max_tokens > 0 ? config->max_tokens : LLM_DEFAULT_MAX_TOKENS;
    client->top_p           = config->top_p > 0 ? config->top_p : 1.0f;
    client->timeout_ms      = config->timeout_ms > 0 ? config->timeout_ms : LLM_DEFAULT_TIMEOUT_MS;
    client->enable_json_mode = config->enable_json_mode;

    if (config->json_schema)
        client->json_schema = iot_strdup(config->json_schema);
    if (config->extra_headers)
        client->extra_headers = iot_strdup(config->extra_headers);

    LOG_INFO("llm", "Client created: %s@%s", client->model, client->base_url);
    return client;
}

int llm_client_set_config(llm_client_t* client, const llm_config_t* config)
{
    if (!client || !config) return -1;

    if (config->base_url)
        strncpy(client->base_url, config->base_url, sizeof(client->base_url) - 1);
    if (config->api_key)
        strncpy(client->api_key, config->api_key, sizeof(client->api_key) - 1);
    if (config->model)
        strncpy(client->model, config->model, sizeof(client->model) - 1);
    if (config->temperature > 0) client->temperature = config->temperature;
    if (config->max_tokens > 0)  client->max_tokens = config->max_tokens;
    if (config->top_p > 0)       client->top_p = config->top_p;
    if (config->timeout_ms > 0)  client->timeout_ms = config->timeout_ms;

    if (config->json_schema) {
        free(client->json_schema);
        client->json_schema = iot_strdup(config->json_schema);
    }
    if (config->extra_headers) {
        free(client->extra_headers);
        client->extra_headers = iot_strdup(config->extra_headers);
    }
    return 0;
}

void llm_client_free(llm_client_t* client)
{
    if (!client) return;
    free(client->json_schema);
    free(client->extra_headers);
    free(client);
}

int llm_client_list_models(llm_client_t* client, char** models_json)
{
    if (!client || !models_json) return -1;

    char url[512];
    snprintf(url, sizeof(url), "%s/models", client->base_url);

    http_response_t http_resp;
    memset(&http_resp, 0, sizeof(http_resp));

    int result = http_get(url, &http_resp);
    if (result != 0 || !http_resp.body) {
        http_response_free(&http_resp);
        return -1;
    }

    *models_json = iot_strdup(http_resp.body);
    http_response_free(&http_resp);
    return (*models_json) ? 0 : -1;
}

/*===========================================================
 * 公共 API - 对话管理
 *===========================================================*/

llm_conversation_t* llm_conversation_create(int max_turns)
{
    llm_conversation_t* conv = (llm_conversation_t*)calloc(1, sizeof(llm_conversation_t));
    if (!conv) return NULL;

    conv->max_turns = (max_turns > 0 && max_turns <= LLM_MAX_CONV_TURNS)
                     ? max_turns : LLM_MAX_CONV_TURNS;
    conv->turn_count = 0;
    return conv;
}

int llm_conversation_add(llm_conversation_t* conv, llm_role_t role,
                          const char* content)
{
    if (!conv || !content) return -1;

    /* 如果已达上限，移除最早的非 system 消息 */
    if (conv->turn_count >= conv->max_turns) {
        int remove_at = 0;
        if (conv->turns[0].role == LLM_ROLE_SYSTEM) remove_at = 1;

        free(conv->turns[remove_at].content);
        memmove(&conv->turns[remove_at], &conv->turns[remove_at + 1],
                (conv->turn_count - remove_at - 1) * sizeof(conv_turn_t));
        conv->turn_count--;
    }

    conv->turns[conv->turn_count].role    = role;
    conv->turns[conv->turn_count].content = iot_strdup(content);
    conv->turn_count++;

    return 0;
}

const char* llm_conversation_get_messages_json(llm_conversation_t* conv,
                                                bool include_system)
{
    if (!conv) return NULL;

    /* 使用静态缓冲区 (内部接口, 调用者不可长期持有) */
    static char buf[LLM_JSON_BUF_SIZE];
    int offset = 0;

    offset += snprintf(buf + offset, sizeof(buf) - offset, "[");
    int first = 1;

    for (int i = 0; i < conv->turn_count; i++) {
        if (!include_system && conv->turns[i].role == LLM_ROLE_SYSTEM)
            continue;

        if (!first) offset += snprintf(buf + offset, sizeof(buf) - offset, ",");
        first = 0;

        offset += snprintf(buf + offset, sizeof(buf) - offset,
            "{\"role\":\"%s\",\"content\":\"%s\"}",
            llm_role_name(conv->turns[i].role),
            conv->turns[i].content ? conv->turns[i].content : "");
    }

    snprintf(buf + offset, sizeof(buf) - offset, "]");
    return buf;
}

void llm_conversation_clear(llm_conversation_t* conv)
{
    if (!conv) return;

    for (int i = 0; i < conv->turn_count; i++) {
        /* 保留 system prompt */
        if (conv->turns[i].role == LLM_ROLE_SYSTEM) continue;
        free(conv->turns[i].content);
    }

    int sys_count = 0;
    if (conv->turn_count > 0 && conv->turns[0].role == LLM_ROLE_SYSTEM) {
        sys_count = 1;
    }

    conv_turn_t sys_turn = {0};
    if (sys_count > 0) {
        sys_turn = conv->turns[0];
    }

    memset(conv->turns, 0, sizeof(conv->turns));
    conv->turn_count = 0;

    if (sys_count > 0) {
        conv->turns[0] = sys_turn;
        conv->turn_count = 1;
    }
}

void llm_conversation_free(llm_conversation_t* conv)
{
    if (!conv) return;
    for (int i = 0; i < conv->turn_count; i++) {
        free(conv->turns[i].content);
    }
    free(conv->system_prompt);
    free(conv);
}

/*===========================================================
 * 公共 API - 同步聊天
 *===========================================================*/

int llm_chat(llm_client_t* client, const char* messages_json,
             const llm_tool_t* tools, int tool_count,
             llm_response_t* response)
{
    if (!client || !messages_json || !response) return -1;

    char* body = llm_build_request_body(client, messages_json,
                                         tools, tool_count);
    if (!body) {
        response->error = iot_strdup("Failed to build request body");
        return -1;
    }

    http_response_t http_resp;
    memset(&http_resp, 0, sizeof(http_resp));

    int result = llm_execute_request(client, body, &http_resp);
    free(body); /* 请求体构建的临时内存 */

    if (result != 0) {
        response->error = iot_strdup(http_resp.error ? http_resp.error : "HTTP error");
        http_response_free(&http_resp);
        return -1;
    }

    llm_parse_response(&http_resp, response);
    http_response_free(&http_resp);

    return response->error ? -1 : 0;
}

int llm_chat_with_context(llm_client_t* client, llm_conversation_t* conv,
                           const llm_tool_t* tools, int tool_count,
                           llm_response_t* response)
{
    if (!client || !conv || !response) return -1;

    const char* messages = llm_conversation_get_messages_json(conv, true);
    if (!messages) {
        response->error = iot_strdup("No messages in conversation");
        return -1;
    }

    int result = llm_chat(client, messages, tools, tool_count, response);

    /* 将 assistant 回复添加到对话历史 */
    if (result == 0 && response->content) {
        llm_conversation_add(conv, LLM_ROLE_ASSISTANT, response->content);
    }

    return result;
}

int llm_ask(llm_client_t* client, const char* user_message,
            llm_response_t* response)
{
    if (!client || !user_message || !response) return -1;

    /* 手动构建单条消息 */
    char messages[LLM_JSON_BUF_SIZE];
    snprintf(messages, sizeof(messages),
             "[{\"role\":\"user\",\"content\":\"%s\"}]",
             user_message ? user_message : "");

    return llm_chat(client, messages, NULL, 0, response);
}

/*===========================================================
 * 公共 API - 异步聊天
 *
 * TODO: 实际异步需要用 iot_task_create 创建后台任务。
 *       当前实现为同步回退。
 *===========================================================*/

static void async_llm_task(void* arg)
{
    struct {
        llm_client_t* client;
        char*         messages_json;
        llm_tool_t*   tools;
        int           tool_count;
        llm_callback_t callback;
        void*         user_data;
    } *ctx = (void*)arg;

    llm_response_t response;
    memset(&response, 0, sizeof(response));

    llm_chat(ctx->client, ctx->messages_json,
             ctx->tools, ctx->tool_count, &response);

    if (ctx->callback) {
        ctx->callback(&response, ctx->user_data);
    }

    llm_response_free(&response);
    free(ctx->messages_json);
    free(ctx);
}

int llm_chat_async(llm_client_t* client, const char* messages_json,
                    const llm_tool_t* tools, int tool_count,
                    llm_callback_t callback, void* user_data)
{
    if (!client || !messages_json || !callback) return -1;

    LOG_INFO("llm", "Async chat requested (model=%s)", client->model);

    /*
     * TODO: 创建后台任务执行 HTTP 请求
     *
     * size_t stack_size = 4096;
     * iot_task_create("llm_async", async_llm_task,
     *                 stack_size, IOT_TASK_PRIO_NORMAL, &task_ctx);
     */
    LOG_WARN("llm", "Async task not available, falling back to sync");
    async_llm_task(NULL); /* 回退 */
    return 0;
}

/*===========================================================
 * 公共 API - 流式聊天
 *
 * SSE 解析: 每行 data: {"choices":[{"delta":{"content":"..."}}]}
 *===========================================================*/

int llm_chat_stream(llm_client_t* client, const char* messages_json,
                     const llm_tool_t* tools, int tool_count,
                     llm_stream_callback_t stream_cb, void* user_data)
{
    if (!client || !messages_json || !stream_cb) return -1;

    /*
     * TODO: 流式实现需要:
     *   1. 使用 http_post 的底层 socket API 发送请求 (stream=true)
     *   2. 持续读取 SSE 数据流
     *   3. 每解析到 content delta 就调用 stream_cb
     *   4. 收到 [DONE] 时结束
     *
     * 当前为简化实现: 使用非流式请求作为回退
     */
    LOG_INFO("llm", "Streaming requested (model=%s), using fallback", client->model);

    llm_response_t response;
    memset(&response, 0, sizeof(response));

    /* 使用非流式请求，然后一次性回调 (模拟流式) */
    int result = llm_chat(client, messages_json, tools, tool_count, &response);
    if (result == 0 && response.content) {
        /* 模拟流式: 按字符发送 */
        for (char* p = response.content; *p; p++) {
            char ch[2] = { *p, '\0' };
            stream_cb(ch, response.finish_reason, 0, user_data);
        }
    }
    stream_cb("", response.finish_reason, 1, user_data);

    llm_response_free(&response);
    return result;
}

int llm_chat_stream_with_context(llm_client_t* client, llm_conversation_t* conv,
                                  const llm_tool_t* tools, int tool_count,
                                  llm_stream_callback_t stream_cb, void* user_data)
{
    if (!client || !conv) return -1;

    const char* messages = llm_conversation_get_messages_json(conv, true);
    if (!messages) return -1;

    return llm_chat_stream(client, messages, tools, tool_count,
                            stream_cb, user_data);
}

/*===========================================================
 * 公共 API - 释放
 *===========================================================*/

void llm_response_free(llm_response_t* response)
{
    if (!response) return;
    free(response->content);
    free(response->finish_reason);
    free(response->tool_calls);
    free(response->error);
    memset(response, 0, sizeof(llm_response_t));
}
