/**
 * @file iot_uart.h
 * @brief 串口
 * @version 1.0
 * @date 2026.06.10
 */

#ifndef IOT_UART_H
#define IOT_UART_H

#include "lua.h"

/* Lua模块入口 */
LUAMOD_API int luaopen_uart(lua_State* L);

#endif /* IOT_UART_H */
