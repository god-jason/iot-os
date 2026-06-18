/*
@module  uart
@summary 串口通信
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     UART
*/

/*
UART参考示例
-- 打开串口
local uart = uart.open(0, {
    baud = 115200,
    databits = 8,
    stopbits = 1,
    parity = uart.PARITY_NONE
})

-- 发送数据
uart.write("hello")

-- 读取数据
local data = uart.read(100)
print("received:", data)

-- 关闭串口
uart.close()
*/

#include "module.h"
#include "yopen_uart.h"

/* 串口参数结构 */
typedef struct {
    yopen_uart_port_number_e port;
    yopen_uart_baud_e baud;
    yopen_uart_databit_e databits;
    yopen_uart_stopbit_e stopbits;
    yopen_uart_paritybit_e parity;
    yopen_uart_flowctrl_e flowctrl;
    uint8_t tx_pin;
    uint8_t rx_pin;
} uart_config_t;

static yopen_uart_port_number_e current_port = YOPEN_PORT_NONE;

/**
 * @brief 打开串口
 * @api uart.open(port, config)
 * @int port      串口端口号 (0=UART0, 1=UART1, 2=UART2, 3=UART3, -1=USB)
 * @table config  配置参数
 * @return userdata 串口操作句柄
 */
static int luaopen_uart_open(lua_State* L) {
    int port_num = (int)luaL_checkinteger(L, 1);
    yopen_uart_port_number_e port = (yopen_uart_port_number_e)port_num;

    uart_config_t config = {
        .port = port,
        .baud = YOPEN_UART_BAUD_115200,
        .databits = YOPEN_UART_DATABIT_8,
        .stopbits = YOPEN_UART_STOP_1,
        .parity = YOPEN_UART_PARITY_NONE,
        .flowctrl = YOPEN_FC_NONE,
        .tx_pin = 0,
        .rx_pin = 0
    };

    /* 解析配置参数 */
    if (lua_istable(L, 2)) {
        lua_getfield(L, 2, "baud");
        if (lua_isnumber(L, -1)) {
            int baud = (int)lua_tonumber(L, -1);
            switch(baud) {
                case 9600:   config.baud = YOPEN_UART_BAUD_9600; break;
                case 19200:  config.baud = YOPEN_UART_BAUD_19200; break;
                case 57600:  config.baud = YOPEN_UART_BAUD_57600; break;
                case 115200: config.baud = YOPEN_UART_BAUD_115200; break;
                case 230400: config.baud = YOPEN_UART_BAUD_230400; break;
                case 460800: config.baud = YOPEN_UART_BAUD_460800; break;
                case 921600: config.baud = YOPEN_UART_BAUD_921600; break;
                default:     config.baud = YOPEN_UART_BAUD_115200; break;
            }
        }
        lua_pop(L, 1);

        lua_getfield(L, 2, "databits");
        if (lua_isnumber(L, -1)) {
            int db = (int)lua_tonumber(L, -1);
            config.databits = (db == 7) ? YOPEN_UART_DATABIT_7 : YOPEN_UART_DATABIT_8;
        }
        lua_pop(L, 1);

        lua_getfield(L, 2, "stopbits");
        if (lua_isnumber(L, -1)) {
            int sb = (int)lua_tonumber(L, -1);
            config.stopbits = (sb == 2) ? YOPEN_UART_STOP_2 : YOPEN_UART_STOP_1;
        }
        lua_pop(L, 1);

        lua_getfield(L, 2, "parity");
        if (lua_isstring(L, -1)) {
            const char* p = lua_tostring(L, -1);
            if (strcmp(p, "odd") == 0) config.parity = YOPEN_UART_PARITY_ODD;
            else if (strcmp(p, "even") == 0) config.parity = YOPEN_UART_PARITY_EVEN;
            else config.parity = YOPEN_UART_PARITY_NONE;
        }
        lua_pop(L, 1);
    }

    /* 打开串口 */
    yopen_uart_open_t uart_cfg = {0};
    uart_cfg.baud = config.baud;
    uart_cfg.databits = config.databits;
    uart_cfg.stopbits = config.stopbits;
    uart_cfg.parity = config.parity;
    uart_cfg.flowctrl = config.flowctrl;

    if (yopen_uart_open(port, &uart_cfg) == YOPEN_UART_SUCCESS) {
        current_port = port;
        lua_pushinteger(L, port);
        return 1;
    }

    lua_pushnil(L);
    lua_pushstring(L, "uart open failed");
    return 2;
}

/**
 * @brief 发送数据
 * @api uart.write(data)
 * @string data 要发送的数据
 * @return int 发送的字节数
 */
static int luaopen_uart_write(lua_State* L) {
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);

    if (current_port == YOPEN_PORT_NONE) {
        lua_pushinteger(L, 0);
        return 1;
    }

    int written = yopen_uart_write(current_port, (uint8_t*)data, len);
    lua_pushinteger(L, written);
    return 1;
}

/**
 * @brief 读取数据
 * @api uart.read(maxlen)
 * @int maxlen 最大读取字节数
 * @return string 读取的数据
 */
static int luaopen_uart_read(lua_State* L) {
    int maxlen = (int)luaL_checkinteger(L, 2);

    if (current_port == YOPEN_PORT_NONE) {
        lua_pushstring(L, "");
        return 1;
    }

    uint8_t* buf = (uint8_t*)iot_malloc(maxlen);
    if (!buf) {
        lua_pushstring(L, "");
        return 1;
    }

    int read = yopen_uart_read(current_port, buf, maxlen, 1000);  // 1s timeout

    if (read > 0) {
        lua_pushlstring(L, (char*)buf, read);
    } else {
        lua_pushstring(L, "");
    }

    iot_free(buf);
    return 1;
}

/**
 * @brief 关闭串口
 * @api uart.close()
 * @return nil
 */
static int luaopen_uart_close(lua_State* L) {
    if (current_port != YOPEN_PORT_NONE) {
        yopen_uart_close(current_port);
        current_port = YOPEN_PORT_NONE;
    }
    return 0;
}

static const luaL_Reg luaopen_uart_funcs[] = {
    {"open",   luaopen_uart_open},
    {"write",  luaopen_uart_write},
    {"read",   luaopen_uart_read},
    {"close",  luaopen_uart_close},
    {NULL, NULL}
};

static const luaL_Reg luaopen_uart_consts[] = {
    {"PARITY_NONE", 0},
    {"PARITY_ODD",  1},
    {"PARITY_EVEN", 2},
    {NULL, NULL}
};

int luaopen_uart(lua_State* L) {
    luaL_newlib(L, luaopen_uart_funcs);
    luaL_setfuncs(L, luaopen_uart_consts, 0);
    return 1;
}