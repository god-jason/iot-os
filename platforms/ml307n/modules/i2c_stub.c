/*
 * I2C module stub when BSP_I2C_ENABLE is off (e.g. ml307n-dc 数传板).
 */
#include "lua.h"
#include "module.h"
#include "iot_i2c.h"

static int i2c_unavailable(lua_State *L)
{
    (void)L;
    return luaL_error(L, "i2c not enabled on this board build");
}

static const luaL_Reg i2c_lib[] = {
    { "setup", i2c_unavailable },
    { "close", i2c_unavailable },
    { "write", i2c_unavailable },
    { "read",  i2c_unavailable },
    { NULL, NULL }
};

LUAMOD_API int luaopen_i2c(lua_State *L)
{
    luaL_newlibtable(L, i2c_lib);
    luaL_setfuncs(L, i2c_lib, 0);
    return 1;
}
