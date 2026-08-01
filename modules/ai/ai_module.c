/**
 * @file ai_module.c
 * @brief AI 模块 Lua 注册入口
 *
 * 将 model / llm / stt / tts 四个 C 模块注册到 Lua。
 * 在 Lua 中使用:
 *   local model = require("model")
 *   local llm   = require("llm")
 *   local stt   = require("stt")
 *   local tts   = require("tts")
 *
 * 注意: model 模块依赖平台后端 (TFLite / ONNX)，在无推理平台的设备
 *       上仍然可以注册但 predict 会返回错误。
 *       llm/stt/tts 依赖 http 模块，确保编译时已包含 http。
 *
 * @author 杰神 & CodeBuddy
 * @date   2026.08.01
 */
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "iot_log.h"

#include "model.h"
#include "llm.h"
#include "stt.h"
#include "tts.h"

/*===========================================================
 * model 模块 Lua API
 *===========================================================*/

/**
 * model.load([path], [backend], [precision])
 * 从文件加载模型
 */
static int lua_model_load(lua_State* L)
{
    const char* path = luaL_optstring(L, 1, NULL);
    iot_model_backend_t backend = (iot_model_backend_t)luaL_optinteger(L, 2, IOT_MODEL_BACKEND_AUTO);
    iot_model_precision_t precision = (iot_model_precision_t)luaL_optinteger(L, 3, IOT_MODEL_PRECISION_AUTO);

    if (!path) {
        lua_pushnil(L);
        lua_pushstring(L, "model path is required");
        return 2;
    }

    iot_model_t* model = iot_model_load_from_file(path, backend, precision);
    if (!model) {
        lua_pushnil(L);
        lua_pushstring(L, "failed to load model");
        return 2;
    }

    /* 将 model 指针封装为 userdata */
    iot_model_t** ud = (iot_model_t**)lua_newuserdata(L, sizeof(iot_model_t*));
    *ud = model;
    luaL_setmetatable(L, "model_handle");

    return 1;
}

/**
 * model.predict(model_handle, input_data)
 * 执行推理
 */
static int lua_model_predict(lua_State* L)
{
    iot_model_t** ud = (iot_model_t**)luaL_checkudata(L, 1, "model_handle");
    if (!ud || !*ud) {
        lua_pushnil(L);
        lua_pushstring(L, "invalid model handle");
        return 2;
    }

    /* 从 Lua table/string 读取输入数据 */
    size_t input_len;
    const char* input_data = NULL;

    if (lua_isstring(L, 2)) {
        input_data = lua_tolstring(L, 2, &input_len);
    } else {
        lua_pushnil(L);
        lua_pushstring(L, "input must be a string (raw bytes)");
        return 2;
    }

    iot_model_set_input(*ud, 0, input_data, input_len);

    if (iot_model_invoke(*ud) != 0) {
        lua_pushnil(L);
        lua_pushstring(L, "inference failed");
        return 2;
    }

    /* 获取输出 */
    int out_count = iot_model_get_output_count(*ud);
    if (out_count <= 0) {
        lua_pushnil(L);
        lua_pushstring(L, "no output tensor");
        return 2;
    }

    iot_model_tensor_info_t info;
    iot_model_get_output_info(*ud, 0, &info);

    uint8_t* out_buf = (uint8_t*)malloc(info.size_bytes);
    if (!out_buf) {
        lua_pushnil(L);
        lua_pushstring(L, "OOM");
        return 2;
    }

    size_t out_size = info.size_bytes;
    iot_model_get_output(*ud, 0, out_buf, &out_size);
    lua_pushlstring(L, (const char*)out_buf, out_size);
    free(out_buf);

    return 1;
}

/**
 * model.get_input_info(model_handle, index)
 */
static int lua_iot_model_get_input_info(lua_State* L)
{
    iot_model_t** ud = (iot_model_t**)luaL_checkudata(L, 1, "model_handle");
    int index = (int)luaL_optinteger(L, 2, 0);

    if (!ud || !*ud) {
        lua_pushnil(L);
        return 1;
    }

    iot_model_tensor_info_t info;
    if (iot_model_get_input_info(*ud, index, &info) != 0) {
        lua_pushnil(L);
        return 1;
    }

    lua_newtable(L);
    lua_pushstring(L, info.name);       lua_setfield(L, -2, "name");
    lua_pushinteger(L, info.dtype);     lua_setfield(L, -2, "dtype");
    lua_pushinteger(L, info.ndim);      lua_setfield(L, -2, "ndim");
    lua_pushinteger(L, (lua_Integer)info.size_bytes);
    lua_setfield(L, -2, "size_bytes");

    /* dims 数组 */
    lua_newtable(L);
    for (int i = 0; i < info.ndim; i++) {
        lua_pushinteger(L, info.dims[i]);
        lua_rawseti(L, -2, i + 1);
    }
    lua_setfield(L, -2, "dims");

    return 1;
}

/**
 * model.get_info(model_handle)
 */
static int lua_model_get_info(lua_State* L)
{
    iot_model_t** ud = (iot_model_t**)luaL_checkudata(L, 1, "model_handle");
    if (!ud || !*ud) {
        lua_pushnil(L);
        return 1;
    }

    char info[1024];
    if (iot_model_get_info_json(*ud, info, sizeof(info)) == 0) {
        lua_pushstring(L, info);
    } else {
        lua_pushstring(L, "{}");
    }
    return 1;
}

/**
 * model.backend()
 * 返回可用后端
 */
static int lua_model_backend(lua_State* L)
{
    iot_model_backend_t b = iot_model_get_available_backend();
    lua_pushinteger(L, b);
    lua_pushstring(L, iot_model_backend_name(b));
    return 2;
}

/**
 * model.close(model_handle)
 * 释放模型
 */
static int lua_model_close(lua_State* L)
{
    iot_model_t** ud = (iot_model_t**)luaL_checkudata(L, 1, "model_handle");
    if (ud && *ud) {
        iot_model_free(*ud);
        *ud = NULL;
    }
    return 0;
}

static const luaL_Reg model_funcs[] = {
    {"load",           lua_model_load},
    {"predict",        lua_model_predict},
    {"get_input_info", lua_iot_model_get_input_info},
    {"get_info",       lua_model_get_info},
    {"backend",        lua_model_backend},
    {"close",          lua_model_close},
    {NULL, NULL}
};

int luaopen_model_register(lua_State* L)
{
    /* 创建 model_handle 元表 */
    luaL_newmetatable(L, "model_handle");
    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, lua_model_close);
    lua_settable(L, -3);
    lua_pop(L, 1);

    luaL_newlib(L, model_funcs);
    return 1;
}

/*===========================================================
 * llm 模块 Lua API
 *===========================================================*/

static iot_llm_client_t* g_llm_client = NULL;

/**
 * llm.create(config_table)
 * 创建 LLM 客户端
 */
static int lua_llm_create(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    iot_llm_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    lua_getfield(L, 1, "base_url");
    cfg.base_url = (char*)lua_tostring(L, -1);
    lua_getfield(L, 1, "api_key");
    cfg.api_key = (char*)lua_tostring(L, -1);
    lua_getfield(L, 1, "model");
    cfg.model = (char*)lua_tostring(L, -1);

    lua_getfield(L, 1, "temperature");
    cfg.temperature = lua_isnumber(L, -1) ? (float)lua_tonumber(L, -1) : 0.7f;
    lua_getfield(L, 1, "max_tokens");
    cfg.max_tokens = lua_isinteger(L, -1) ? (int)lua_tointeger(L, -1) : 1024;
    lua_getfield(L, 1, "timeout_ms");
    cfg.timeout_ms = lua_isinteger(L, -1) ? (int)lua_tointeger(L, -1) : 30000;

    lua_pop(L, 7); /* 弹出字段值 */

    if (g_llm_client) iot_llm_client_free(g_llm_client);
    g_llm_client = iot_llm_client_create(&cfg);

    if (g_llm_client) {
        lua_pushboolean(L, 1);
    } else {
        lua_pushnil(L);
        lua_pushstring(L, "failed to create LLM client");
        return 2;
    }
    return 1;
}

/**
 * llm.chat(messages_json, [tools], [callback])
 * 同步/异步聊天
 */
static int lua_iot_llm_chat(lua_State* L)
{
    if (!g_llm_client) {
        lua_pushnil(L);
        lua_pushstring(L, "LLM client not created");
        return 2;
    }

    const char* messages = luaL_checkstring(L, 1);

    iot_llm_response_t response;
    memset(&response, 0, sizeof(response));

    if (iot_llm_chat(g_llm_client, messages, NULL, 0, &response) != 0) {
        lua_pushnil(L);
        lua_pushstring(L, response.error ? response.error : "chat failed");
        iot_llm_response_free(&response);
        return 2;
    }

    lua_newtable(L);
    if (response.content) {
        lua_pushstring(L, response.content);
        lua_setfield(L, -2, "content");
    }
    if (response.finish_reason) {
        lua_pushstring(L, response.finish_reason);
        lua_setfield(L, -2, "finish_reason");
    }
    if (response.tool_calls) {
        lua_pushstring(L, response.tool_calls);
        lua_setfield(L, -2, "tool_calls");
    }
    lua_pushinteger(L, response.total_tokens);
    lua_setfield(L, -2, "total_tokens");
    lua_pushinteger(L, response.prompt_tokens);
    lua_setfield(L, -2, "prompt_tokens");
    lua_pushinteger(L, response.completion_tokens);
    lua_setfield(L, -2, "completion_tokens");

    iot_llm_response_free(&response);
    return 1;
}

/**
 * llm.ask(question)
 * 单轮问答简便接口
 */
static int lua_iot_llm_ask(lua_State* L)
{
    if (!g_llm_client) {
        lua_pushnil(L);
        lua_pushstring(L, "LLM client not created");
        return 2;
    }

    const char* question = luaL_checkstring(L, 1);

    iot_llm_response_t response;
    memset(&response, 0, sizeof(response));

    if (iot_llm_ask(g_llm_client, question, &response) != 0) {
        lua_pushnil(L);
        lua_pushstring(L, response.error ? response.error : "ask failed");
        iot_llm_response_free(&response);
        return 2;
    }

    if (response.content) {
        lua_pushstring(L, response.content);
    } else {
        lua_pushstring(L, "");
    }

    lua_pushinteger(L, response.total_tokens);

    iot_llm_response_free(&response);
    return 2;
}

static const luaL_Reg llm_funcs[] = {
    {"create", lua_llm_create},
    {"chat",   lua_iot_llm_chat},
    {"ask",    lua_iot_llm_ask},
    {NULL, NULL}
};

int luaopen_llm_register(lua_State* L)
{
    luaL_newlib(L, llm_funcs);
    return 1;
}

/*===========================================================
 * stt 模块 Lua API
 *===========================================================*/

static iot_stt_t* g_stt = NULL;

static int lua_iot_stt_create(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    iot_stt_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    lua_getfield(L, 1, "provider");
    cfg.provider = (iot_stt_provider_t)lua_tointeger(L, -1);
    lua_getfield(L, 1, "api_key");
    strncpy(cfg.api_key, lua_tostring(L, -1) ? lua_tostring(L, -1) : "", sizeof(cfg.api_key) - 1);
    lua_getfield(L, 1, "language");
    strncpy(cfg.language, lua_tostring(L, -1) ? lua_tostring(L, -1) : "zh", sizeof(cfg.language) - 1);
    lua_getfield(L, 1, "sample_rate");
    cfg.sample_rate = lua_isinteger(L, -1) ? (int)lua_tointeger(L, -1) : 16000;
    lua_getfield(L, 1, "enable_vad");
    cfg.enable_vad = lua_toboolean(L, -1);
    lua_pop(L, 5);

    if (g_stt) iot_stt_free(g_stt);
    g_stt = iot_stt_create(&cfg);

    lua_pushboolean(L, g_stt != NULL);
    return 1;
}

static int lua_iot_stt_recognize_file(lua_State* L)
{
    if (!g_stt) {
        lua_pushnil(L); lua_pushstring(L, "STT not created"); return 2;
    }
    const char* path = luaL_checkstring(L, 1);
    iot_stt_result_t result;
    memset(&result, 0, sizeof(result));
    if (iot_stt_recognize_file(g_stt, path, &result) != 0) {
        lua_pushnil(L);
        lua_pushstring(L, result.error ? result.error : "recognize failed");
        iot_stt_result_free(&result);
        return 2;
    }
    lua_pushstring(L, result.text ? result.text : "");
    lua_pushnumber(L, result.confidence);
    iot_stt_result_free(&result);
    return 2;
}

static const luaL_Reg stt_funcs[] = {
    {"create",         lua_iot_stt_create},
    {"recognize_file", lua_iot_stt_recognize_file},
    {NULL, NULL}
};

int luaopen_stt_register(lua_State* L)
{
    luaL_newlib(L, stt_funcs);
    return 1;
}

/*===========================================================
 * tts 模块 Lua API
 *===========================================================*/

static iot_tts_t* g_tts = NULL;

static int lua_iot_tts_create(lua_State* L)
{
    luaL_checktype(L, 1, LUA_TTABLE);

    iot_tts_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    lua_getfield(L, 1, "provider");
    cfg.provider = (iot_tts_provider_t)lua_tointeger(L, -1);
    lua_getfield(L, 1, "api_key");
    strncpy(cfg.api_key, lua_tostring(L, -1) ? lua_tostring(L, -1) : "", sizeof(cfg.api_key) - 1);
    lua_getfield(L, 1, "voice");
    cfg.voice = (iot_tts_voice_t)luaL_optinteger(L, -1, IOT_TTS_VOICE_DEFAULT);
    lua_getfield(L, 1, "speed");
    cfg.speed = lua_isnumber(L, -1) ? (float)lua_tonumber(L, -1) : 1.0f;
    lua_getfield(L, 1, "volume");
    cfg.volume = lua_isnumber(L, -1) ? (float)lua_tonumber(L, -1) : 1.0f;
    lua_getfield(L, 1, "language");
    strncpy(cfg.language, lua_tostring(L, -1) ? lua_tostring(L, -1) : "zh-CN", sizeof(cfg.language) - 1);
    lua_pop(L, 6);

    if (g_tts) iot_tts_free(g_tts);
    g_tts = iot_tts_create(&cfg);

    lua_pushboolean(L, g_tts != NULL);
    return 1;
}

static int lua_iot_tts_speak(lua_State* L)
{
    if (!g_tts) {
        lua_pushnil(L); lua_pushstring(L, "TTS not created"); return 2;
    }
    const char* text = luaL_checkstring(L, 1);
    int ret = iot_tts_speak(g_tts, text);
    lua_pushboolean(L, ret == 0);
    return 1;
}

static int lua_tts_to_file(lua_State* L)
{
    if (!g_tts) {
        lua_pushnil(L); lua_pushstring(L, "TTS not created"); return 2;
    }
    const char* text = luaL_checkstring(L, 1);
    const char* path = luaL_checkstring(L, 2);
    int ret = iot_tts_synthesize_to_file(g_tts, text, path);
    lua_pushboolean(L, ret == 0);
    return 1;
}

static const luaL_Reg tts_funcs[] = {
    {"create",  lua_iot_tts_create},
    {"speak",   lua_iot_tts_speak},
    {"to_file", lua_tts_to_file},
    {NULL, NULL}
};

int luaopen_tts_register(lua_State* L)
{
    luaL_newlib(L, tts_funcs);
    return 1;
}

/*===========================================================
 * 子模块注册表 (供 iot_modules.c 调用)
 *===========================================================*/

/**
 * 注册全部 AI 子模块
 */
void ai_modules_register(lua_State* L)
{
    luaL_requiref(L, "model", luaopen_model_register, 1);
    lua_pop(L, 1);

    luaL_requiref(L, "llm",   luaopen_llm_register, 1);
    lua_pop(L, 1);

    luaL_requiref(L, "stt",   luaopen_stt_register, 1);
    lua_pop(L, 1);

    luaL_requiref(L, "tts",   luaopen_tts_register, 1);
    lua_pop(L, 1);

    LOG_INFO("ai", "AI modules registered: model, llm, stt, tts");
}
