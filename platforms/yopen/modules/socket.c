/*
@module  socket
@summary Socket网络
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     SOCKET
*/

#include "module.h"
#include "yopen_sockets.h"

static int luaopen_socket_tcp(lua_State* L) {
    lua_pushnil(L);
    return 1;
}

static int luaopen_socket_udp(lua_State* L) {
    lua_pushnil(L);
    return 1;
}

static int luaopen_socket_close(lua_State* L) {
    return 0;
}

static const luaL_Reg luaopen_socket_funcs[] = {
    {"tcp",  luaopen_socket_tcp},
    {"udp",  luaopen_socket_udp},
    {"close", luaopen_socket_close},
    {NULL, NULL}
};

int luaopen_socket(lua_State* L) {
    luaL_newlib(L, luaopen_socket_funcs);
    return 1;
}