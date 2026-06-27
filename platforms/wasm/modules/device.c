/*
@module  device
@summary 设备信息模块
@version 1.0
@date    2026.06.27
@desc    获取浏览器和设备相关信息

### Lua 示例
```lua
-- 获取设备信息
local info = device.info()

print("浏览器:", info.browser)      -- Chrome, Firefox, Safari, etc.
print("版本:", info.version)         -- 90.0.1234.5678
print("用户代理:", info.userAgent)   -- 完整的 User-Agent 字符串
print("语言:", info.language)       -- zh-CN, en-US, etc.
print("平台:", info.platform)        -- Win32, MacIntel, Linux x86_64
print("在线:", info.online)          -- true/false
print("屏幕:", info.screenWidth, "x", info.screenHeight)
print("窗口:", info.windowWidth, "x", info.windowHeight)
print("像素比:", info.pixelRatio)   -- 1, 2 (Retina)
print("触控:", info.touchSupport)   -- true/false
print("时间:", info.timezone)        -- Asia/Shanghai
print("类型:", info.deviceType)      -- desktop, mobile, tablet
```
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lua.h"
#include "lauxlib.h"

#include "emscripten/emscripten.h"
#include "emscripten/dom_pk_code.h"

static int device_info(lua_State* L) {
    lua_newtable(L);
    
    /* 浏览器信息 */
    EM_ASM({
        var ua = navigator.userAgent;
        var browser = "Unknown";
        var version = "0.0.0";
        
        if (ua.indexOf("Firefox") > -1) {
            browser = "Firefox";
            version = ua.match(/Firefox\/([\d.]+)/)[1];
        } else if (ua.indexOf("Chrome") > -1) {
            browser = "Chrome";
            version = ua.match(/Chrome\/([\d.]+)/)[1];
        } else if (ua.indexOf("Safari") > -1 && ua.indexOf("Chrome") == -1) {
            browser = "Safari";
            version = ua.match(/Version\/([\d.]+)/)[1];
        } else if (ua.indexOf("Edge") > -1) {
            browser = "Edge";
            version = ua.match(/Edge\/([\d.]+)/)[1];
        } else if (ua.indexOf("Opera") > -1 || ua.indexOf("OPR") > -1) {
            browser = "Opera";
            version = ua.match(/(?:Opera|OPR)\/([\d.]+)/)[1];
        }
        
        lua_pushstring(L, "browser");
        lua_pushstring(L, browser);
        lua_settable(L, -3);
        
        lua_pushstring(L, "version");
        lua_pushstring(L, version);
        lua_settable(L, -3);
        
        lua_pushstring(L, "userAgent");
        lua_pushstring(L, ua);
        lua_settable(L, -3);
    }, 0);
    
    /* 语言 */
    EM_ASM({
        lua_pushstring(L, "language");
        lua_pushstring(L, navigator.language || navigator.userLanguage || "");
        lua_settable(L, -3);
    }, 0);
    
    /* 平台 */
    EM_ASM({
        lua_pushstring(L, "platform");
        lua_pushstring(L, navigator.platform || "");
        lua_settable(L, -3);
    }, 0);
    
    /* 在线状态 */
    EM_ASM({
        lua_pushstring(L, "online");
        lua_pushboolean(L, navigator.onLine ? 1 : 0);
        lua_settable(L, -3);
    }, 0);
    
    /* 屏幕尺寸 */
    EM_ASM({
        lua_pushstring(L, "screenWidth");
        lua_pushinteger(L, screen.width);
        lua_settable(L, -3);
        
        lua_pushstring(L, "screenHeight");
        lua_pushinteger(L, screen.height);
        lua_settable(L, -3);
    }, 0);
    
    /* 窗口尺寸 */
    EM_ASM({
        lua_pushstring(L, "windowWidth");
        lua_pushinteger(L, window.innerWidth);
        lua_settable(L, -3);
        
        lua_pushstring(L, "windowHeight");
        lua_pushinteger(L, window.innerHeight);
        lua_settable(L, -3);
    }, 0);
    
    /* 像素比 */
    EM_ASM({
        lua_pushstring(L, "pixelRatio");
        lua_pushnumber(L, window.devicePixelRatio || 1);
        lua_settable(L, -3);
    }, 0);
    
    /* 触控支持 */
    EM_ASM({
        lua_pushstring(L, "touchSupport");
        lua_pushboolean(L, ('ontouchstart' in window) || (navigator.maxTouchPoints > 0) ? 1 : 0);
        lua_settable(L, -3);
    }, 0);
    
    /* 时区 */
    EM_ASM({
        lua_pushstring(L, "timezone");
        lua_pushstring(L, Intl.DateTimeFormat().resolvedOptions().timeZone || "");
        lua_settable(L, -3);
    }, 0);
    
    /* 设备类型 */
    EM_ASM({
        var type = "desktop";
        var ua = navigator.userAgent.toLowerCase();
        
        if (/(tablet|ipad|playbook|silk)|(android(?!.*mobi))/i.test(ua)) {
            type = "tablet";
        } else if (/Mobile|Android|iP(hone|od)|IEMobile|BlackBerry|Kindle|Silk-Accelerated|(hpw|web)OS|Opera M(obi|ini)/.test(ua)) {
            type = "mobile";
        }
        
        lua_pushstring(L, "deviceType");
        lua_pushstring(L, type);
        lua_settable(L, -3);
    }, 0);
    
    return 1;
}

static int device_online(lua_State* L) {
    EM_ASM({
        lua_pushboolean(L, navigator.onLine ? 1 : 0);
    }, 1);
    return 1;
}

static int device_vibrate(lua_State* L) {
    int pattern_len = 0;
    
    if (lua_gettop(L) >= 1 && lua_istable(L, 1)) {
        pattern_len = lua_rawlen(L, 1);
    }
    
    if (pattern_len == 0) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    int* pattern = (int*)malloc(sizeof(int) * pattern_len);
    if (!pattern) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    for (int i = 0; i < pattern_len; i++) {
        lua_rawgeti(L, 1, i + 1);
        pattern[i] = (int)lua_tointeger(L, -1);
        lua_pop(L, 1);
    }
    
    EM_ASM({
        var pattern = new Array();
        for (var i = 0; i < $1; i++) {
            pattern.push(HEAP32[($0 + i * 4) >> 2]);
        }
        navigator.vibrate(pattern);
    }, pattern_len, (int)pattern);
    
    free(pattern);
    lua_pushboolean(L, 1);
    return 1;
}

static int device_wake_lock(lua_State* L) {
    const char* action = luaL_optstring(L, 1, "request");
    
    if (strcmp(action, "request") == 0) {
        EM_ASM({
            if ('wakeLock' in navigator) {
                navigator.wakeLock.request('screen').then(function(wakeLock) {
                    window._wakeLock = wakeLock;
                }).catch(function(err) {
                    console.error('Wake Lock error:', err);
                });
            }
        }, 0);
    } else if (strcmp(action, "release") == 0) {
        EM_ASM({
            if (window._wakeLock) {
                window._wakeLock.release();
                window._wakeLock = null;
            }
        }, 0);
    }
    
    return 0;
}

static const luaL_Reg device_methods[] = {
    {"info", device_info},
    {"online", device_online},
    {"vibrate", device_vibrate},
    {"wake_lock", device_wake_lock},
    {NULL, NULL}
};

LUAMOD_API int luaopen_device_register(lua_State* L) {
    lua_newtable(L);
    luaL_setfuncs(L, device_methods, 0);
    return 1;
}