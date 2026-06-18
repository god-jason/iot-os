/*
@module  lcd
@summary LCD显示模块
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
@tag     LCD
*/

/*
LCD参考示例
local lcd = require("lcd")

-- 初始化LCD
local ok = lcd.init({
    spi_dev = 2,      -- SPI设备号
    dcx_gpio = 23,    -- DCX引脚
    rst_gpio = 24     -- RST引脚
})

if ok then
    print("LCD init OK")
    
    -- 发送命令
    lcd.write_cmd(0x2C)  -- 写入GRAM命令
    
    -- 发送数据（刷屏数据）
    local data = string.rep("\xFF\xFF", 128 * 160)  -- 全屏白色
    lcd.write_data(data)
    
    -- 读取ID
    local id = lcd.read_id(0x04)
    print("LCD ID:", id)
end

-- 去初始化
lcd.deinit()
*/

#include "lua.h"
#include "lauxlib.h"
#include "iot_base.h"
#include "cm_spi_lcd.h"

static int iot_lcd_init(lua_State* L);
static int iot_lcd_deinit(lua_State* L);
static int iot_lcd_write_cmd(lua_State* L);
static int iot_lcd_write_data(lua_State* L);
static int iot_lcd_read_id(lua_State* L);

static const luaL_Reg lcd_lib[] = {
    {"init",        iot_lcd_init},
    {"deinit",      iot_lcd_deinit},
    {"write_cmd",   iot_lcd_write_cmd},
    {"write_data",  iot_lcd_write_data},
    {"read_id",     iot_lcd_read_id},
    {NULL, NULL}
};

/**
 * @brief 初始化LCD
 * @api lcd.init(config)
 * @param config table 配置参数
 * @return bool 成功返回true
 * @usage
 * local ok = lcd.init({
 *     spi_dev = 2,
 *     dcx_gpio = 23,
 *     rst_gpio = 24
 * })
 */
static int iot_lcd_init(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    
    cm_spi_lcd_cfg_t cfg;
    memset(&cfg, 0, sizeof(cm_spi_lcd_cfg_t));
    
    lua_getfield(L, 1, "spi_dev");
    cfg.spi_dev = lua_isnumber(L, -1) ? (cm_spi_dev_e)lua_tointeger(L, -1) : CM_SPI_DEV_2;
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "dcx_gpio");
    cfg.dcx_gpio = lua_isnumber(L, -1) ? (cm_gpio_num_e)lua_tointeger(L, -1) : CM_GPIO_NUM_MAX;
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "rst_gpio");
    cfg.rst_gpio = lua_isnumber(L, -1) ? (cm_gpio_num_e)lua_tointeger(L, -1) : CM_GPIO_NUM_MAX;
    lua_pop(L, 1);
    
    int32_t ret = cm_spi_lcd_init(&cfg);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 去初始化LCD
 * @api lcd.deinit()
 * @return bool 成功返回true
 */
static int iot_lcd_deinit(lua_State* L) {
    int32_t ret = cm_spi_lcd_deinit();
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 发送命令
 * @api lcd.write_cmd(cmd)
 * @param cmd int 命令值
 * @return bool 成功返回true
 */
static int iot_lcd_write_cmd(lua_State* L) {
    uint8_t cmd = (uint8_t)luaL_checkinteger(L, 1);
    
    int32_t ret = cm_spi_lcd_write_cmd(&cmd, 1);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 发送数据
 * @api lcd.write_data(data)
 * @param data string 数据
 * @return bool 成功返回true
 */
static int iot_lcd_write_data(lua_State* L) {
    size_t len;
    const char* data = luaL_checklstring(L, 1, &len);
    
    int32_t ret = cm_spi_lcd_write_data((uint8_t*)data, len);
    lua_pushboolean(L, (ret == 0));
    return 1;
}

/**
 * @brief 读取LCD ID
 * @api lcd.read_id(cmd)
 * @param cmd int 读取ID的命令
 * @return string ID字符串
 */
static int iot_lcd_read_id(lua_State* L) {
    uint8_t cmd = (uint8_t)luaL_checkinteger(L, 1);
    uint8_t id_buf[4] = {0};
    
    int32_t ret = cm_spi_lcd_read_id(&cmd, 1, id_buf, sizeof(id_buf));
    if (ret == 0) {
        lua_pushlstring(L, (const char*)id_buf, sizeof(id_buf));
    } else {
        lua_pushnil(L);
    }
    return 1;
}

LUAMOD_API int luaopen_lcd(lua_State* L) {
    luaL_newlibtable(L, lcd_lib);
    luaL_setfuncs(L, lcd_lib, 0);
    
    /* SPI设备号常量 */
    lua_pushinteger(L, CM_SPI_DEV_1);
    lua_setfield(L, -2, "SPI_DEV_1");
    
    lua_pushinteger(L, CM_SPI_DEV_2);
    lua_setfield(L, -2, "SPI_DEV_2");
    
    lua_pushinteger(L, CM_SPI_DEV_3);
    lua_setfield(L, -2, "SPI_DEV_3");
    
    return 1;
}