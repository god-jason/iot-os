/*
@module  gpio
@summary GPIO通用输入输出
@version 1.1
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     GPIO
*/

/*
GPIO参考示例
-- 初始化GPIO7为输出模式
gpio.setup(7, gpio.OUTPUT, gpio.PULLUP)

-- 设置GPIO7输出高电平
gpio.set(7, gpio.HIGH)

-- 读取GPIO7电平
local level = gpio.get(7)
print("GPIO7 level:", level)

-- GPIO7输出低电平
gpio.set(7, gpio.LOW)

-- 配置GPIO7为输入，带中断回调
gpio.setup(7, gpio.INPUT, gpio.PULLUP, function(level)
    print("GPIO7 interrupt, level:", level)
end)

-- 设置中断触发方式为双边沿
gpio.int_enable(7, gpio.INT_BOTH)

-- 去初始化GPIO7
gpio.close(7)
*/


#include "lua.h"
#include "module.h"
#include "cm_gpio.h"
#include "iot_rtos.h"
#include "iot_params.h"

/* GPIO方向 */
#define IOT_GPIO_INPUT       0
#define IOT_GPIO_OUTPUT      1

/* GPIO电平 */
#define IOT_GPIO_LOW         0
#define IOT_GPIO_HIGH        1

/* GPIO上拉下拉 */
#define IOT_GPIO_PULL_NONE   0
#define IOT_GPIO_PULL_DOWN   1
#define IOT_GPIO_PULL_UP     2

/* GPIO中断触发方式 */
#define IOT_GPIO_INT_RISING  0  /* 上升沿触发 */
#define IOT_GPIO_INT_FALLING 1  /* 下降沿触发 */
#define IOT_GPIO_INT_BOTH    2  /* 双边沿触发 */
#define IOT_GPIO_INT_HIGH    3  /* 高电平触发 */
#define IOT_GPIO_INT_LOW     4  /* 低电平触发 */

/* GPIO引脚数量 */
#define IOT_GPIO_MAX         30

/* 全局GPIO配置表 */
static cm_gpio_cfg_t g_gpio_cfg[IOT_GPIO_MAX] = {0};
static int g_gpio_inited[IOT_GPIO_MAX] = {0};
static cm_gpio_interrupt_e g_gpio_int_mode[IOT_GPIO_MAX] = {0};
static void* g_gpio_callback_ud[IOT_GPIO_MAX] = {NULL};

/**
 * @brief GPIO中断回调函数（中断上下文） */
static void iot_gpio_interrupt_callback(cm_gpio_num_e pin, cm_gpio_level_e level) {
    LOG("GPIO%d interrupt, level=%d", pin, level);

    /* 创建参数列表 */
    params_t* params = params_create(1);
    if (!params) {
        LOG("ERR params_create failed");
        return;
    }

    params_push_int(params, level);

    /* 通过RTOS消息队列安全调用回调 */
    iot_rtos_call(g_gpio_callback_ud[pin], params);
}

/**
 * @brief 初始化GPIO
 * @api gpio.setup(pin, direction, pull, callback)
 * @int pin GPIO引脚号 0-29
 * @int direction 方向,gpio.INPUT(0)输入,gpio.OUTPUT(1)输出
 * @int pull 上拉下拉,gpio.NOPULL(0),gpio.PULLDOWN(1),gpio.PULLUP(2)
 * @function callback 中断回调函数,当direction为输入时可设置
 * @return int|function 成功返回true,失败返回false
 * @usage
-- 配置GPIO7为输出
gpio.setup(7, gpio.OUTPUT, gpio.NOPULL)

-- 配置GPIO7为输入，带中断回调
gpio.setup(7, gpio.INPUT, gpio.PULLUP, function(pin, level)
    print("GPIO7中断,电平:", level)
end)
 */
static int iot_gpio_setup(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    int direction = luaL_checkinteger(L, 2);
    int pull = luaL_optinteger(L, 3, IOT_GPIO_PULL_NONE);
    int cb_idx = 4;

    LOG("setup pin=%d dir=%d pull=%d", pin, direction, pull);

    if (pin < 0 || pin >= IOT_GPIO_MAX) {
        LOG("ERR invalid pin %d", pin);
        lua_pushnil(L);
        return 1;
    }

    if (direction != IOT_GPIO_INPUT && direction != IOT_GPIO_OUTPUT) {
        LOG("ERR invalid direction %d", direction);
        lua_pushnil(L);
        return 1;
    }

    if (pull < IOT_GPIO_PULL_NONE || pull > IOT_GPIO_PULL_UP) {
        LOG("ERR invalid pull %d", pull);
        lua_pushnil(L);
        return 1;
    }

    /* 如果已初始化，先关闭 */
    if (g_gpio_inited[pin]) {
        cm_gpio_interrupt_disable((cm_gpio_num_e)pin);
        if (g_gpio_callback_ud[pin]) {
            lua_pushlightuserdata(L, g_gpio_callback_ud[pin]);
            lua_pushnil(L);
            lua_settable(L, LUA_REGISTRYINDEX);
            g_gpio_callback_ud[pin] = NULL;
        }
        cm_gpio_deinit((cm_gpio_num_e)pin);
        g_gpio_inited[pin] = 0;
    }

    /* 配置GPIO */
    cm_gpio_cfg_t cfg;
    memset(&cfg, 0, sizeof(cfg));
    cfg.mode = CM_GPIO_MODE_NUM;
    cfg.direction = (direction == IOT_GPIO_INPUT) ? CM_GPIO_DIRECTION_INPUT : CM_GPIO_DIRECTION_OUTPUT;
    cfg.pull = (cm_gpio_pull_e)pull;

    int ret = cm_gpio_init((cm_gpio_num_e)pin, &cfg);
    if (ret != 0) {
        LOG("ERR init failed ret=%d", ret);
        lua_pushnil(L);
        return 1;
    }

    g_gpio_inited[pin] = 1;
    g_gpio_cfg[pin] = cfg;

    if (direction == IOT_GPIO_INPUT && lua_isfunction(L, cb_idx)) {
        /* 保存回调函数 */
        void* ud = lua_newuserdata(L, 1);
        lua_pushvalue(L, cb_idx);
        lua_setuservalue(L, -2);
        g_gpio_callback_ud[pin] = ud;

        lua_pushlightuserdata(L, ud);
        lua_pushvalue(L, -2);
        lua_settable(L, LUA_REGISTRYINDEX);

        lua_pop(L, 1);

        /* 默认使用下降沿触发 */
        g_gpio_int_mode[pin] = CM_GPIO_IT_EDGE_FALLING;
        ret = cm_gpio_interrupt_register((cm_gpio_num_e)pin, iot_gpio_interrupt_callback);
        if (ret != 0) {
            LOG("ERR register interrupt failed ret=%d", ret);
            lua_pushlightuserdata(L, ud);
            lua_pushnil(L);
            lua_settable(L, LUA_REGISTRYINDEX);
            g_gpio_callback_ud[pin] = NULL;
            cm_gpio_deinit((cm_gpio_num_e)pin);
            g_gpio_inited[pin] = 0;
            lua_pushnil(L);
            return 1;
        }

        cm_gpio_interrupt_enable((cm_gpio_num_e)pin, CM_GPIO_IT_EDGE_FALLING);
        LOG("GPIO%d interrupt enabled", pin);
    }

    lua_pushboolean(L, 1);
    return 1;
}

/**
 * @brief 设置GPIO电平
 * @api gpio.set(pin, level)
 * @int pin GPIO引脚号 0-29
 * @int level 电平,gpio.LOW(0) 或 gpio.HIGH(1)
 * @return bool 成功返回true,失败返回false
 * @usage
gpio.set(7, gpio.HIGH) -- 设置高电平
gpio.set(7, gpio.LOW) -- 设置低电平
 */
static int iot_gpio_set(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    int level = luaL_checkinteger(L, 2);

    if (pin < 0 || pin >= IOT_GPIO_MAX) {
        LOG("ERR invalid pin %d", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_gpio_inited[pin]) {
        LOG("ERR pin %d not inited", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (level != IOT_GPIO_LOW && level != IOT_GPIO_HIGH) {
        LOG("ERR invalid level %d", level);
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = cm_gpio_set_level((cm_gpio_num_e)pin, (cm_gpio_level_e)level);
    LOG("set pin=%d level=%d ret=%d", pin, level, ret);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 读取GPIO电平
 * @api gpio.get(pin)
 * @int pin GPIO引脚号 0-29
 * @return int 电平值 gpio.LOW(0) 或 gpio.HIGH(1)
 * @usage
local level = gpio.get(7)
print("GPIO7电平:", level)
 */
static int iot_gpio_get(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    cm_gpio_level_e level = CM_GPIO_LEVEL_LOW;

    if (pin < 0 || pin >= IOT_GPIO_MAX) {
        LOG("ERR invalid pin %d", pin);
        lua_pushinteger(L, 0);
        return 1;
    }

    if (!g_gpio_inited[pin]) {
        LOG("ERR pin %d not inited", pin);
        lua_pushinteger(L, 0);
        return 1;
    }

    int ret = cm_gpio_get_level((cm_gpio_num_e)pin, &level);
    if (ret != 0) {
        LOG("ERR get level failed ret=%d", ret);
        lua_pushinteger(L, 0);
    } else {
        LOG("get pin=%d level=%d", pin, level);
        lua_pushinteger(L, level);
    }
    return 1;
}

/**
 * @brief 关闭GPIO
 * @api gpio.close(pin)
 * @int pin GPIO引脚号 0-29
 * @return bool 成功返回true,失败返回false
 * @usage
gpio.close(7)
 */
static int iot_gpio_close(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);

    if (pin < 0 || pin >= IOT_GPIO_MAX) {
        LOG("ERR invalid pin %d", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_gpio_inited[pin]) {
        lua_pushboolean(L, 1);
        return 1;
    }

    /* 失能中断 */
    cm_gpio_interrupt_disable((cm_gpio_num_e)pin);

    /* 取消注册回调 */
    if (g_gpio_callback_ud[pin]) {
        lua_pushlightuserdata(L, g_gpio_callback_ud[pin]);
        lua_pushnil(L);
        lua_settable(L, LUA_REGISTRYINDEX);
        g_gpio_callback_ud[pin] = NULL;
    }

    /* 去初始化 */
    int ret = cm_gpio_deinit((cm_gpio_num_e)pin);
    g_gpio_inited[pin] = 0;

    LOG("close pin=%d ret=%d", pin, ret);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 设置GPIO上拉下拉
 * @api gpio.pull(pin, pull)
 * @int pin GPIO引脚号 0-29
 * @int pull 上下拉模式 gpio.NOPULL(0),gpio.PULLDOWN(1),gpio.PULLUP(2)
 * @return bool 成功返回true,失败返回false
 * @usage
gpio.pull(7, gpio.PULLUP) -- 设置上拉
 */
static int iot_gpio_pull(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    int pull = luaL_checkinteger(L, 2);

    if (pin < 0 || pin >= IOT_GPIO_MAX) {
        LOG("ERR invalid pin %d", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (pull < IOT_GPIO_PULL_NONE || pull > IOT_GPIO_PULL_UP) {
        LOG("ERR invalid pull %d", pull);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_gpio_inited[pin]) {
        LOG("ERR pin %d not inited", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = cm_gpio_set_pull((cm_gpio_num_e)pin, (cm_gpio_pull_e)pull);
    if (ret == 0) {
        g_gpio_cfg[pin].pull = (cm_gpio_pull_e)pull;
    }

    LOG("pull pin=%d pull=%d ret=%d", pin, pull, ret);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 设置GPIO方向
 * @api gpio.direction(pin, dir)
 * @int pin GPIO引脚号 0-29
 * @int dir 方向,gpio.INPUT(0)输入,gpio.OUTPUT(1)输出
 * @return bool 成功返回true,失败返回false
 * @usage
gpio.direction(7, gpio.OUTPUT) -- 设置为输出 */
static int iot_gpio_direction(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    int dir = luaL_checkinteger(L, 2);

    if (pin < 0 || pin >= IOT_GPIO_MAX) {
        LOG("ERR invalid pin %d", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (dir != IOT_GPIO_INPUT && dir != IOT_GPIO_OUTPUT) {
        LOG("ERR invalid direction %d", dir);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_gpio_inited[pin]) {
        LOG("ERR pin %d not inited", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = cm_gpio_set_direction((cm_gpio_num_e)pin, (cm_gpio_direction_e)dir);
    if (ret == 0) {
        g_gpio_cfg[pin].direction = (cm_gpio_direction_e)dir;
    }

    LOG("direction pin=%d dir=%d ret=%d", pin, dir, ret);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 使能GPIO中断
 * @api gpio.int_enable(pin, mode)
 * @int pin GPIO引脚号 0-29
 * @int mode 触发方式,gpio.INT_RISING(0)上升沿,gpio.INT_FALLING(1)下降沿,gpio.INT_BOTH(2)双边沿,gpio.INT_HIGH(3)高电平,gpio.INT_LOW(4)低电平
 * @return bool 成功返回true,失败返回false
 * @usage
gpio.int_enable(7, gpio.INT_RISING) -- 上升沿触发
 */
static int iot_gpio_int_enable(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);
    int mode = luaL_optinteger(L, 2, IOT_GPIO_INT_FALLING);

    if (pin < 0 || pin >= IOT_GPIO_MAX) {
        LOG("ERR invalid pin %d", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_gpio_inited[pin]) {
        LOG("ERR pin %d not inited", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    cm_gpio_interrupt_e intr_mode;
    switch (mode) {
        case IOT_GPIO_INT_RISING:  intr_mode = CM_GPIO_IT_EDGE_RISING; break;
        case IOT_GPIO_INT_FALLING: intr_mode = CM_GPIO_IT_EDGE_FALLING; break;
        case IOT_GPIO_INT_BOTH:    intr_mode = CM_GPIO_IT_EDGE_BOTH; break;
        case IOT_GPIO_INT_HIGH:    intr_mode = CM_GPIO_IT_LEVEL_HIGH; break;
        case IOT_GPIO_INT_LOW:     intr_mode = CM_GPIO_IT_LEVEL_LOW; break;
        default:
            LOG("ERR invalid mode %d, using default", mode);
            intr_mode = CM_GPIO_IT_EDGE_FALLING;
            break;
    }

    g_gpio_int_mode[pin] = intr_mode;
    int ret = cm_gpio_interrupt_enable((cm_gpio_num_e)pin, intr_mode);
    LOG("int_enable pin=%d mode=%d ret=%d", pin, mode, ret);
    lua_pushboolean(L, ret == 0);
    return 1;
}

/**
 * @brief 失能GPIO中断
 * @api gpio.int_disable(pin)
 * @int pin GPIO引脚号 0-29
 * @return bool 成功返回true,失败返回false
 * @usage
gpio.int_disable(7)
 */
static int iot_gpio_int_disable(lua_State* L) {
    int pin = luaL_checkinteger(L, 1);

    if (pin < 0 || pin >= IOT_GPIO_MAX) {
        LOG("ERR invalid pin %d", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    if (!g_gpio_inited[pin]) {
        LOG("ERR pin %d not inited", pin);
        lua_pushboolean(L, 0);
        return 1;
    }

    int ret = cm_gpio_interrupt_disable((cm_gpio_num_e)pin);
    LOG("int_disable pin=%d ret=%d", pin, ret);
    lua_pushboolean(L, ret == 0);
    return 1;
}

static const luaL_Reg gpio_lib[] = {
    /* 函数定义 */
    { "setup",        iot_gpio_setup },
    { "set",          iot_gpio_set },
    { "get",          iot_gpio_get },
    { "close",        iot_gpio_close },
    { "pull",         iot_gpio_pull },
    { "direction",    iot_gpio_direction },
    { "int_enable",   iot_gpio_int_enable },
    { "int_disable",  iot_gpio_int_disable },
    { NULL, NULL }
};

LUAMOD_API int luaopen_gpio(lua_State* L) {
    LOG("luaopen gpio");
    luaL_newlibtable(L, gpio_lib);
    luaL_setfuncs(L, gpio_lib, 0);

    /* 常量定义 */
    lua_pushinteger(L, IOT_GPIO_INPUT);
    lua_setfield(L, -2, "INPUT");
    lua_pushinteger(L, IOT_GPIO_OUTPUT);
    lua_setfield(L, -2, "OUTPUT");
    lua_pushinteger(L, IOT_GPIO_LOW);
    lua_setfield(L, -2, "LOW");
    lua_pushinteger(L, IOT_GPIO_HIGH);
    lua_setfield(L, -2, "HIGH");
    lua_pushinteger(L, IOT_GPIO_PULL_NONE);
    lua_setfield(L, -2, "NOPULL");
    lua_pushinteger(L, IOT_GPIO_PULL_DOWN);
    lua_setfield(L, -2, "PULLDOWN");
    lua_pushinteger(L, IOT_GPIO_PULL_UP);
    lua_setfield(L, -2, "PULLUP");
    lua_pushinteger(L, IOT_GPIO_INT_RISING);
    lua_setfield(L, -2, "INT_RISING");
    lua_pushinteger(L, IOT_GPIO_INT_FALLING);
    lua_setfield(L, -2, "INT_FALLING");
    lua_pushinteger(L, IOT_GPIO_INT_BOTH);
    lua_setfield(L, -2, "INT_BOTH");
    lua_pushinteger(L, IOT_GPIO_INT_HIGH);
    lua_setfield(L, -2, "INT_HIGH");
    lua_pushinteger(L, IOT_GPIO_INT_LOW);
    lua_setfield(L, -2, "INT_LOW");

    return 1;
}