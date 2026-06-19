/*
@module  mobile
@summary 移动网络
@version 1.0
@date    2026.06.19
@author  TRAE
@tag     MOBILE
*/

#include "module.h"
#include "yopen_nw.h"

static int luaopen_mobile_connect(lua_State* L) {
    const char* apn = luaL_optstring(L, 1, "cmnet");
    (void)apn;
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_mobile_disconnect(lua_State* L) {
    return 0;
}

static int luaopen_mobile_status(lua_State* L) {
    yopen_nw_reg_status_info_s reg_info = {0};
    yopen_nw_get_reg_status(0, &reg_info);

    yopen_nw_operator_info_s oper_info = {0};
    yopen_nw_get_operator_name(0, &oper_info);

    unsigned char csq = 0;
    yopen_nw_get_csq(0, &csq);

    lua_createtable(L, 0, 4);

    switch (reg_info.data_reg.state) {
        case YOPEN_NW_REG_STATUS_REGISTERED:
            lua_pushstring(L, "connected");
            break;
        case YOPEN_NW_REG_STATUS_SEARCHING:
            lua_pushstring(L, "searching");
            break;
        case YOPEN_NW_REG_STATUS_DENIED:
            lua_pushstring(L, "denied");
            break;
        default:
            lua_pushstring(L, "disconnected");
            break;
    }
    lua_setfield(L, -2, "status");

    lua_pushinteger(L, csq);
    lua_setfield(L, -2, "csq");
    lua_pushinteger(L, 0);
    lua_setfield(L, -2, "rssi");
    lua_pushstring(L, oper_info.oper_name);
    lua_setfield(L, -2, "operator");

    return 1;
}

static int luaopen_mobile_signal(lua_State* L) {
    unsigned char csq = 0;
    yopen_nw_get_csq(0, &csq);
    lua_pushinteger(L, csq);
    return 1;
}

static const luaL_Reg luaopen_mobile_funcs[] = {
    {"connect",    luaopen_mobile_connect},
    {"disconnect", luaopen_mobile_disconnect},
    {"status",     luaopen_mobile_status},
    {"signal",     luaopen_mobile_signal},
    {NULL, NULL}
};

int luaopen_mobile(lua_State* L) {
    luaL_newlib(L, luaopen_mobile_funcs);
    return 1;
}