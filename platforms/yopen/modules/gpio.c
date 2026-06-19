/*
@module  gpio
@summary GPIO控制
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     GPIO
*/

/*
GPIO参考示例
-- 初始化GPIO
local gpio = gpio.open(0, {dir=gpio.OUTPUT, pull=gpio.PULL_NONE})

-- 设置GPIO电平
gpio.write(gpio.HIGH)

-- 读取GPIO电平
local val = gpio.read()
print("gpio value:", val)

-- 关闭GPIO
gpio.close()
*/

#include "module.h"
#include "yopen_gpio.h"

/* GPIO 方向 */
#define GPIO_INPUT     0
#define GPIO_OUTPUT    1

/* GPIO 电平 */
#define GPIO_LOW       0
#define GPIO_HIGH      1

/* 上下拉模式 */
#define GPIO_PULL_NONE     0
#define GPIO_PULL_UP       1
#define GPIO_PULL_DOWN     2

/**
 * @brief 打开GPIO
 * @api gpio.open(id, config)
 * @int id       GPIO编号 (0-GPIO_MAX)
 * @table config 配置参数 {dir=gpio.INPUT/OUTPUT, pull=gpio.PULL_NONE/UP/DOWN, voltage=Vol_1_80V/Vol_3_30V}
 * @return userdata GPIO操作句柄
 */
static int luaopen_gpio_open(lua_State* L) {
    int id = (int)luaL_checkinteger(L, 1);
    yopen_GpioNum gpio_num = (yopen_GpioNum)id;

    /* 解析配置参数 */
    int dir = GPIO_OUTPUT;
    int pull = GPIO_PULL_NONE;
    int voltage = 0;  // default

    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "dir");
        if (lua_isnumber(L, -1)) dir = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "pull");
        if (lua_isnumber(L, -1)) pull = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, 2, "voltage");
        if (lua_isnumber(L, -1)) voltage = (int)lua_tonumber(L, -1);
        lua_pop(L, 1);
    }

    /* 配置GPIO */
    yopen_pin_set_func(gpio_num, 0);

    yopen_GpioDir gpio_dir = (yopen_GpioDir)dir;
    yopen_PullMode gpio_pull = PULL_DEFAULT;
    yopen_LvlMode gpio_lvl = LVL_LOW;

    if (dir == GPIO_OUTPUT) {
        gpio_lvl = LVL_HIGH;
    } else {
        if (pull == GPIO_PULL_UP) gpio_pull = FORCE_PULL_UP;
        else if (pull == GPIO_PULL_DOWN) gpio_pull = FORCE_PULL_DOWN;
        else if (pull == GPIO_PULL_NONE) gpio_pull = FORCE_PULL_NONE;
    }

    yopen_gpio_init(gpio_num, gpio_dir, gpio_pull, gpio_lvl);

    lua_pushinteger(L, id);
    return 1;
}

/**
 * @brief 设置GPIO电平
 * @api gpio.write(val)
 * @int val 电平值 (0=LOW, 1=HIGH)
 * @return nil
 */
static int luaopen_gpio_write(lua_State* L) {
    int gpio_id = (int)luaL_checkinteger(L, 1);
    int val = (int)luaL_checkinteger(L, 2);
    yopen_gpio_set_level((yopen_GpioNum)gpio_id, val ? LVL_HIGH : LVL_LOW);
    return 0;
}

/**
 * @brief 读取GPIO电平
 * @api gpio.read()
 * @return int 电平值 (0=LOW, 1=HIGH)
 */
static int luaopen_gpio_read(lua_State* L) {
    int gpio_id = (int)luaL_checkinteger(L, 1);
    yopen_LvlMode val = LVL_LOW;
    yopen_gpio_get_level((yopen_GpioNum)gpio_id, &val);
    lua_pushinteger(L, val == LVL_HIGH ? 1 : 0);
    return 1;
}

/**
 * @brief 关闭GPIO
 * @api gpio.close()
 * @return nil
 */
static int luaopen_gpio_close(lua_State* L) {
    int gpio_id = (int)luaL_checkinteger(L, 1);
    yopen_gpio_deinit((yopen_GpioNum)gpio_id);
    return 0;
}

static const luaL_Reg luaopen_gpio_funcs[] = {
    {"open",   luaopen_gpio_open},
    {"write",  luaopen_gpio_write},
    {"read",   luaopen_gpio_read},
    {"close",  luaopen_gpio_close},
    {NULL, NULL}
};

static const luaL_Reg luaopen_gpio_consts[] = {
    {"INPUT",     GPIO_INPUT},
    {"OUTPUT",    GPIO_OUTPUT},
    {"LOW",       GPIO_LOW},
    {"HIGH",      GPIO_HIGH},
    {"PULL_NONE", GPIO_PULL_NONE},
    {"PULL_UP",   GPIO_PULL_UP},
    {"PULL_DOWN", GPIO_PULL_DOWN},
    {NULL, NULL}
};

int luaopen_gpio(lua_State* L) {
    luaL_newlib(L, luaopen_gpio_funcs);
    luaL_setfuncs(L, luaopen_gpio_consts, 0);
    return 1;
}