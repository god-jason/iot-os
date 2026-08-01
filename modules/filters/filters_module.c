/**
 * @file filters_module.c
 * @brief 数字滤波器 Lua 模块封装
 *
 * 将 C 实现的 7 种滤波算法封装为统一的 Lua 对象接口：
 *   local filterObj = filter.create("name", opts)
 *   filterObj:update(num)      -- 输入新样本，返回滤波后的值
 *   filterObj:reset(num)       -- 重置滤波器，可选传入初始值
 *   filterObj:get()            -- 获取当前滤波值
 *
 * 支持的滤波器名称:
 *   "ema"            - 指数移动平均
 *   "ma"             - 滑动窗口平均
 *   "median"         - 中值滤波
 *   "kalman"         - 一维卡尔曼
 *   "iir"            - IIR 数字滤波器
 *   "lowpass"        - 一阶低通
 *   "complementary"  - 互补滤波
 *
 * @author  IoT-OS Team
 * @date    2026.07.30
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "iot_log.h"
#include "iot.h"
#include "filters.h"

/*================================================================
 *                 滤波器类型定义
 *================================================================*/

typedef enum {
    IOT_FILTER_LUA_EMA = 0,
    IOT_FILTER_LUA_MA,
    IOT_FILTER_LUA_MEDIAN,
    IOT_FILTER_LUA_KALMAN,
    IOT_FILTER_LUA_IIR,
    IOT_FILTER_LUA_LOWPASS,
    IOT_FILTER_LUA_COMPLEMENTARY
} iot_filter_lua_type_t;

/**
 * @brief Lua 滤波器对象 - 所有滤波器类型的联合封装
 */
typedef struct {
    iot_filter_lua_type_t type;
    int  (*update)(void* impl, float input, float input2, float* output);
    int  (*get)(void* impl, float* output);
    void (*reset)(void* impl, float initial);
    void (*deinit)(void* impl);
    union {
        iot_filter_ema_t ema;
        iot_filter_ma_t ma;
        iot_filter_median_t median;
        iot_filter_kalman_t kalman;
        iot_filter_iir_t iir;
        iot_filter_lowpass_t lowpass;
        iot_filter_complementary_t complementary;
    } impl;
} iot_filter_lua_obj_t;

#define IOT_FILTER_LUA_MT "filter"

/*================================================================
 *                 通用辅助函数
 *================================================================*/

static float get_float_field(lua_State* L, int table_idx, const char* key, float default_value)
{
    float result = default_value;
    lua_getfield(L, table_idx, key);
    if (lua_isnumber(L, -1)) {
        result = (float)lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return result;
}

static int get_int_field(lua_State* L, int table_idx, const char* key, int default_value)
{
    int result = default_value;
    lua_getfield(L, table_idx, key);
    if (lua_isinteger(L, -1)) {
        result = (int)lua_tointeger(L, -1);
    } else if (lua_isnumber(L, -1)) {
        result = (int)lua_tonumber(L, -1);
    }
    lua_pop(L, 1);
    return result;
}

static int push_filter_error(lua_State* L, const char* msg)
{
    lua_pushnil(L);
    lua_pushstring(L, msg);
    return 2;
}

/*================================================================
 *              各类型滤波器的虚函数实现
 *================================================================*/

/* EMA */
static int ema_update(void* impl, float input, float unused, float* output)
{
    (void)unused;
    return iot_filter_ema_update((iot_filter_ema_t*)impl, input, output);
}
static int ema_get(void* impl, float* output)
{
    return iot_filter_ema_get((iot_filter_ema_t*)impl, output);
}
static void ema_reset(void* impl, float initial)
{
    iot_filter_ema_t* f = (iot_filter_ema_t*)impl;
    iot_filter_ema_reset(f);
    if (initial != 0.0f) {
        f->value = initial;
        f->initialized = true;
    }
}
static void ema_deinit(void* impl) { (void)impl; }

/* MA */
static int ma_update(void* impl, float input, float unused, float* output)
{
    (void)unused;
    return iot_filter_ma_update((iot_filter_ma_t*)impl, input, output);
}
static int ma_get(void* impl, float* output)
{
    return iot_filter_ma_get((iot_filter_ma_t*)impl, output);
}
static void ma_reset(void* impl, float initial)
{
    (void)initial;
    iot_filter_ma_reset((iot_filter_ma_t*)impl);
}
static void ma_deinit(void* impl)
{
    iot_filter_ma_deinit((iot_filter_ma_t*)impl);
}

/* Median */
static int median_update(void* impl, float input, float unused, float* output)
{
    (void)unused;
    return iot_filter_median_update((iot_filter_median_t*)impl, input, output);
}
static int median_get(void* impl, float* output)
{
    return iot_filter_median_get((iot_filter_median_t*)impl, output);
}
static void median_reset(void* impl, float initial)
{
    (void)initial;
    iot_filter_median_reset((iot_filter_median_t*)impl);
}
static void median_deinit(void* impl)
{
    iot_filter_median_deinit((iot_filter_median_t*)impl);
}

/* Kalman */
static int kalman_update(void* impl, float input, float unused, float* output)
{
    (void)unused;
    return iot_filter_kalman_update((iot_filter_kalman_t*)impl, input, output);
}
static int kalman_get(void* impl, float* output)
{
    return iot_filter_kalman_get((iot_filter_kalman_t*)impl, output);
}
static void kalman_reset(void* impl, float initial)
{
    iot_filter_kalman_t* f = (iot_filter_kalman_t*)impl;
    iot_filter_kalman_reset(f);
    if (initial != 0.0f) {
        f->x = initial;
        f->initialized = true;
    }
}
static void kalman_deinit(void* impl) { (void)impl; }

/* IIR */
static int iir_update(void* impl, float input, float unused, float* output)
{
    (void)unused;
    return iot_filter_iir_update((iot_filter_iir_t*)impl, input, output);
}
static int iir_get(void* impl, float* output)
{
    return iot_filter_iir_get((iot_filter_iir_t*)impl, output);
}
static void iir_reset(void* impl, float initial)
{
    (void)initial;
    iot_filter_iir_reset((iot_filter_iir_t*)impl);
}
static void iir_deinit(void* impl) { (void)impl; }

/* Lowpass */
static int lowpass_update(void* impl, float input, float unused, float* output)
{
    (void)unused;
    return iot_filter_lowpass_update((iot_filter_lowpass_t*)impl, input, output);
}
static int lowpass_get(void* impl, float* output)
{
    return iot_filter_lowpass_get((iot_filter_lowpass_t*)impl, output);
}
static void lowpass_reset(void* impl, float initial)
{
    iot_filter_lowpass_t* f = (iot_filter_lowpass_t*)impl;
    iot_filter_lowpass_reset(f);
    if (initial != 0.0f) {
        f->value = initial;
        f->initialized = true;
    }
}
static void lowpass_deinit(void* impl) { (void)impl; }

/* Complementary (update 需要两个输入) */
static int complementary_update(void* impl, float input, float input2, float* output)
{
    return iot_filter_complementary_update((iot_filter_complementary_t*)impl, input, input2, output);
}
static int complementary_get(void* impl, float* output)
{
    return iot_filter_complementary_get((iot_filter_complementary_t*)impl, output);
}
static void complementary_reset(void* impl, float initial)
{
    iot_filter_complementary_t* f = (iot_filter_complementary_t*)impl;
    iot_filter_complementary_reset(f);
    if (initial != 0.0f) {
        f->value = initial;
        f->initialized = true;
    }
}
static void complementary_deinit(void* impl) { (void)impl; }

/*================================================================
 *                 Lua 方法实现
 *================================================================*/

/**
 * @brief filterObj:update(value [, value2])
 * @note complementary 类型需要两个参数: update(low_pass, high_pass_delta)
 */
static int iot_filter_lua_update(lua_State* L)
{
    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)luaL_checkudata(L, 1, IOT_FILTER_LUA_MT);
    float input = (float)luaL_checknumber(L, 2);
    float input2 = 0.0f;
    
    /* complementary 类型接收两个参数 */
    if (obj->type == IOT_FILTER_LUA_COMPLEMENTARY) {
        input2 = (float)luaL_checknumber(L, 3);
    }
    
    float output;
    int ret = obj->update(&obj->impl, input, input2, &output);
    if (ret != IOT_FILTER_OK) {
        return push_filter_error(L, "filter:update failed");
    }
    lua_pushnumber(L, output);
    return 1;
}

/**
 * @brief filterObj:get()
 */
static int filter_lua_get(lua_State* L)
{
    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)luaL_checkudata(L, 1, IOT_FILTER_LUA_MT);
    float output;
    if (obj->get(&obj->impl, &output) != IOT_FILTER_OK) {
        return push_filter_error(L, "filter not initialized or no data");
    }
    lua_pushnumber(L, output);
    return 1;
}

/**
 * @brief filterObj:reset([initial_value])
 */
static int filter_lua_reset(lua_State* L)
{
    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)luaL_checkudata(L, 1, IOT_FILTER_LUA_MT);
    float initial = 0.0f;
    if (lua_gettop(L) >= 2 && lua_isnumber(L, 2)) {
        initial = (float)lua_tonumber(L, 2);
    }
    obj->reset(&obj->impl, initial);
    return 0;
}

/**
 * @brief GC 回调
 */
static int filter_lua_gc(lua_State* L)
{
    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)luaL_checkudata(L, 1, IOT_FILTER_LUA_MT);
    obj->deinit(&obj->impl);
    return 0;
}

static const luaL_Reg iot_filter_methods[] = {
    { "update", iot_filter_lua_update },
    { "get",    filter_lua_get },
    { "reset",  filter_lua_reset },
    { "__gc",   filter_lua_gc },
    { NULL,     NULL }
};

/*================================================================
 *                 工厂函数: filter.create(name, opts)
 *================================================================*/

/**
 * @brief 创建 EMA 滤波器
 */
static int create_ema(lua_State* L, int opts_idx)
{
    float alpha = get_float_field(L, opts_idx, "alpha", -1.0f);
    if (alpha < 0.0f) {
        float cutoff = get_float_field(L, opts_idx, "cutoff", 0.0f);
        float sr     = get_float_field(L, opts_idx, "sr", 0.0f);
        if (cutoff > 0.0f && sr > 0.0f) {
            alpha = iot_filter_calc_alpha_from_freq(cutoff, sr);
        }
    }
    if (alpha < 0.0f) {
        float tau = get_float_field(L, opts_idx, "tau", 0.0f);
        float sr  = get_float_field(L, opts_idx, "sr", 0.0f);
        if (tau > 0.0f && sr > 0.0f) {
            alpha = iot_filter_calc_alpha_from_tau(tau, sr);
        }
    }
    if (alpha < 0.0f || alpha > 1.0f) {
        return push_filter_error(L, "ema: invalid or missing 'alpha' (0~1)");
    }

    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)lua_newuserdata(L, sizeof(iot_filter_lua_obj_t));
    memset(obj, 0, sizeof(*obj));
    obj->type = IOT_FILTER_LUA_EMA;
    obj->update = ema_update;
    obj->get = ema_get;
    obj->reset = ema_reset;
    obj->deinit = ema_deinit;

    iot_filter_ema_init(&obj->impl.ema, alpha);

    /* 可选初始值 */
    lua_getfield(L, opts_idx, "initial");
    if (!lua_isnil(L, -1)) {
        obj->impl.ema.value = (float)lua_tonumber(L, -1);
        obj->impl.ema.initialized = true;
    }
    lua_pop(L, 1);

    luaL_getmetatable(L, IOT_FILTER_LUA_MT);
    lua_setmetatable(L, -2);
    return 1;
}

/**
 * @brief 创建 MA 滤波器
 */
static int create_ma(lua_State* L, int opts_idx)
{
    int window = get_int_field(L, opts_idx, "window", 0);
    if (window < 2) {
        return push_filter_error(L, "ma: 'window' must be >= 2");
    }

    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)lua_newuserdata(L, sizeof(iot_filter_lua_obj_t));
    memset(obj, 0, sizeof(*obj));
    obj->type = IOT_FILTER_LUA_MA;
    obj->update = ma_update;
    obj->get = ma_get;
    obj->reset = ma_reset;
    obj->deinit = ma_deinit;

    if (iot_filter_ma_init(&obj->impl.ma, window) != IOT_FILTER_OK) {
        return push_filter_error(L, "ma: memory allocation failed");
    }

    luaL_getmetatable(L, IOT_FILTER_LUA_MT);
    lua_setmetatable(L, -2);
    return 1;
}

/**
 * @brief 创建 Median 滤波器
 */
static int create_median(lua_State* L, int opts_idx)
{
    int window = get_int_field(L, opts_idx, "window", 0);
    if (window < 3) {
        return push_filter_error(L, "median: 'window' must be >= 3");
    }

    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)lua_newuserdata(L, sizeof(iot_filter_lua_obj_t));
    memset(obj, 0, sizeof(*obj));
    obj->type = IOT_FILTER_LUA_MEDIAN;
    obj->update = median_update;
    obj->get = median_get;
    obj->reset = median_reset;
    obj->deinit = median_deinit;

    if (iot_filter_median_init(&obj->impl.median, window) != IOT_FILTER_OK) {
        return push_filter_error(L, "median: memory allocation failed");
    }

    luaL_getmetatable(L, IOT_FILTER_LUA_MT);
    lua_setmetatable(L, -2);
    return 1;
}

/**
 * @brief 创建 Kalman 滤波器
 */
static int create_kalman(lua_State* L, int opts_idx)
{
    float q = get_float_field(L, opts_idx, "q", 0.001f);
    float r = get_float_field(L, opts_idx, "r", 0.1f);

    if (r <= 0.0f) {
        return push_filter_error(L, "kalman: 'r' (measurement noise) must be > 0");
    }

    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)lua_newuserdata(L, sizeof(iot_filter_lua_obj_t));
    memset(obj, 0, sizeof(*obj));
    obj->type = IOT_FILTER_LUA_KALMAN;
    obj->update = kalman_update;
    obj->get = kalman_get;
    obj->reset = kalman_reset;
    obj->deinit = kalman_deinit;

    iot_filter_kalman_init(&obj->impl.kalman, q, r);

    /* 可选初始估计值 */
    lua_getfield(L, opts_idx, "initial");
    if (!lua_isnil(L, -1)) {
        obj->impl.kalman.x = (float)lua_tonumber(L, -1);
        obj->impl.kalman.initialized = true;
    }
    lua_pop(L, 1);

    luaL_getmetatable(L, IOT_FILTER_LUA_MT);
    lua_setmetatable(L, -2);
    return 1;
}

/**
 * @brief 创建 IIR 滤波器
 */
static int create_iir(lua_State* L, int opts_idx)
{
    float b[3] = {1.0f, 0.0f, 0.0f};
    float a[3] = {1.0f, 0.0f, 0.0f};
    int b_count = 0, a_count = 0;

    /* 读取 b 系数 */
    lua_getfield(L, opts_idx, "b");
    if (lua_istable(L, -1)) {
        for (int i = 1; i <= 3; i++) {
            lua_rawgeti(L, -1, i);
            if (lua_isnumber(L, -1)) {
                b[i - 1] = (float)lua_tonumber(L, -1);
                b_count++;
                lua_pop(L, 1);
            } else {
                lua_pop(L, 1);
                break;
            }
        }
    }
    lua_pop(L, 1);

    /* 读取 a 系数 */
    lua_getfield(L, opts_idx, "a");
    if (lua_istable(L, -1)) {
        for (int i = 1; i <= 3; i++) {
            lua_rawgeti(L, -1, i);
            if (lua_isnumber(L, -1)) {
                a[i - 1] = (float)lua_tonumber(L, -1);
                a_count++;
                lua_pop(L, 1);
            } else {
                lua_pop(L, 1);
                break;
            }
        }
    }
    lua_pop(L, 1);

    if (b_count < 2 || a_count < 2) {
        return push_filter_error(L, "iir: need at least 2 'b' and 2 'a' coefficients");
    }
    if (a[0] == 0.0f) {
        return push_filter_error(L, "iir: a[0] must not be zero");
    }

    int order = (b_count >= 3 && a_count >= 3) ? 2 : 1;

    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)lua_newuserdata(L, sizeof(iot_filter_lua_obj_t));
    memset(obj, 0, sizeof(*obj));
    obj->type = IOT_FILTER_LUA_IIR;
    obj->update = iir_update;
    obj->get = iir_get;
    obj->reset = iir_reset;
    obj->deinit = iir_deinit;

    if (iot_filter_iir_init(&obj->impl.iir, b, a, order) != IOT_FILTER_OK) {
        return push_filter_error(L, "iir: init failed");
    }

    luaL_getmetatable(L, IOT_FILTER_LUA_MT);
    lua_setmetatable(L, -2);
    return 1;
}

/**
 * @brief 创建 Lowpass 滤波器
 */
static int create_lowpass(lua_State* L, int opts_idx)
{
    float alpha = get_float_field(L, opts_idx, "alpha", -1.0f);
    if (alpha < 0.0f) {
        float cutoff = get_float_field(L, opts_idx, "cutoff", 0.0f);
        float sr     = get_float_field(L, opts_idx, "sr", 0.0f);
        if (cutoff > 0.0f && sr > 0.0f) {
            alpha = iot_filter_calc_alpha_from_freq(cutoff, sr);
        }
    }
    if (alpha < 0.0f) {
        float tau = get_float_field(L, opts_idx, "tau", 0.0f);
        float sr  = get_float_field(L, opts_idx, "sr", 0.0f);
        if (tau > 0.0f && sr > 0.0f) {
            alpha = iot_filter_calc_alpha_from_tau(tau, sr);
        }
    }
    if (alpha < 0.0f || alpha > 1.0f) {
        return push_filter_error(L, "lowpass: invalid or missing 'alpha' (0~1)");
    }

    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)lua_newuserdata(L, sizeof(iot_filter_lua_obj_t));
    memset(obj, 0, sizeof(*obj));
    obj->type = IOT_FILTER_LUA_LOWPASS;
    obj->update = lowpass_update;
    obj->get = lowpass_get;
    obj->reset = lowpass_reset;
    obj->deinit = lowpass_deinit;

    iot_filter_lowpass_init(&obj->impl.lowpass, alpha);

    lua_getfield(L, opts_idx, "initial");
    if (!lua_isnil(L, -1)) {
        obj->impl.lowpass.value = (float)lua_tonumber(L, -1);
        obj->impl.lowpass.initialized = true;
    }
    lua_pop(L, 1);

    luaL_getmetatable(L, IOT_FILTER_LUA_MT);
    lua_setmetatable(L, -2);
    return 1;
}

/**
 * @brief 创建 Complementary 滤波器
 */
static int create_complementary(lua_State* L, int opts_idx)
{
    float alpha = get_float_field(L, opts_idx, "alpha", 0.98f);

    if (alpha < 0.0f || alpha > 1.0f) {
        return push_filter_error(L, "complementary: 'alpha' must be 0~1");
    }

    iot_filter_lua_obj_t* obj = (iot_filter_lua_obj_t*)lua_newuserdata(L, sizeof(iot_filter_lua_obj_t));
    memset(obj, 0, sizeof(*obj));
    obj->type = IOT_FILTER_LUA_COMPLEMENTARY;
    obj->update = complementary_update;
    obj->get = complementary_get;
    obj->reset = complementary_reset;
    obj->deinit = complementary_deinit;

    iot_filter_complementary_init(&obj->impl.complementary, alpha);

    lua_getfield(L, opts_idx, "initial");
    if (!lua_isnil(L, -1)) {
        obj->impl.complementary.value = (float)lua_tonumber(L, -1);
        obj->impl.complementary.initialized = true;
    }
    lua_pop(L, 1);

    luaL_getmetatable(L, IOT_FILTER_LUA_MT);
    lua_setmetatable(L, -2);
    return 1;
}

/**
 * @brief filter.create(name, opts)
 */
static int luaopen_iot_filter_create(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);
    int opts_idx = 2;

    if (!lua_istable(L, opts_idx)) {
        return push_filter_error(L, "create: second arg must be options table");
    }

    if (strcmp(name, "ema") == 0)           return create_ema(L, opts_idx);
    if (strcmp(name, "ma") == 0)            return create_ma(L, opts_idx);
    if (strcmp(name, "median") == 0)        return create_median(L, opts_idx);
    if (strcmp(name, "kalman") == 0)        return create_kalman(L, opts_idx);
    if (strcmp(name, "iir") == 0)           return create_iir(L, opts_idx);
    if (strcmp(name, "lowpass") == 0)       return create_lowpass(L, opts_idx);
    if (strcmp(name, "complementary") == 0) return create_complementary(L, opts_idx);

    return push_filter_error(L, "create: unknown filter name, valid: ema/ma/median/kalman/iir/lowpass/complementary");
}

/*================================================================
 *                 工具函数 Lua 绑定
 *================================================================*/

static int luaopen_iot_filter_alpha_from_freq(lua_State* L)
{
    float cutoff      = (float)luaL_checknumber(L, 1);
    float sample_rate = (float)luaL_checknumber(L, 2);
    lua_pushnumber(L, iot_filter_calc_alpha_from_freq(cutoff, sample_rate));
    return 1;
}

static int luaopen_iot_filter_alpha_from_tau(lua_State* L)
{
    float tau         = (float)luaL_checknumber(L, 1);
    float sample_rate = (float)luaL_checknumber(L, 2);
    lua_pushnumber(L, iot_filter_calc_alpha_from_tau(tau, sample_rate));
    return 1;
}

/*================================================================
 *                 模块注册入口
 *================================================================*/

static const luaL_Reg iot_filter_module_funcs[] = {
    { "create",            luaopen_iot_filter_create },
    { "alpha_from_freq",   luaopen_iot_filter_alpha_from_freq },
    { "alpha_from_tau",    luaopen_iot_filter_alpha_from_tau },
    { NULL,                NULL }
};

/**
 * @brief filters 模块注册函数
 */
LUAMOD_API int luaopen_filters_register(lua_State* L)
{
    luaL_newlib(L, iot_filter_module_funcs);

    /* 注册统一的滤波器元表 */
    luaL_newmetatable(L, IOT_FILTER_LUA_MT);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, iot_filter_methods, 0);
    lua_pop(L, 1);

    return 1;
}
