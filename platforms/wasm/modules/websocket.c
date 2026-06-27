/*
@module  websocket
@summary WebAssembly WebSocket模块
@version 1.0
@date    2026.06.27
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "lua.h"
#include "lauxlib.h"

#include "emscripten/emscripten.h"
#include "emscripten/websocket.h"

#define MAX_WS_INSTANCES 16

typedef struct {
    EMSCRIPTEN_WEBSOCKET_T ws;
    int lua_ref;
    int connected;
} ws_instance_t;

static ws_instance_t ws_instances[MAX_WS_INSTANCES];
static int ws_instance_count = 0;

static int ws_find_free_slot(void) {
    for (int i = 0; i < MAX_WS_INSTANCES; i++) {
        if (ws_instances[i].ws == 0) {
            return i;
        }
    }
    return -1;
}

static int ws_find_by_handle(EMSCRIPTEN_WEBSOCKET_T ws) {
    for (int i = 0; i < MAX_WS_INSTANCES; i++) {
        if (ws_instances[i].ws == ws) {
            return i;
        }
    }
    return -1;
}

static void ws_callback(int eventType, const EmscriptenWebSocketEvent* e, void* userData) {
    int idx = (int)(intptr_t)userData;
    if (idx < 0 || idx >= MAX_WS_INSTANCES) return;
    
    lua_State* L = NULL;
    extern lua_State* iot_get_lua_state(void);
    L = iot_get_lua_state();
    if (!L) return;
    
    ws_instance_t* inst = &ws_instances[idx];
    if (inst->lua_ref == LUA_NOREF) return;
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, inst->lua_ref);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    switch (eventType) {
        case EMSCRIPTEN_WEBSOCKET_OPEN:
            inst->connected = 1;
            lua_getfield(L, -1, "on_open");
            if (lua_isfunction(L, -1)) {
                lua_pcall(L, 0, 0, 0);
            }
            lua_pop(L, 1);
            break;
            
        case EMSCRIPTEN_WEBSOCKET_MESSAGE:
            lua_getfield(L, -1, "on_message");
            if (lua_isfunction(L, -1)) {
                lua_pushlstring(L, e->data, e->numBytes);
                lua_pcall(L, 1, 0, 0);
            }
            lua_pop(L, 1);
            break;
            
        case EMSCRIPTEN_WEBSOCKET_ERROR:
            inst->connected = 0;
            lua_getfield(L, -1, "on_error");
            if (lua_isfunction(L, -1)) {
                lua_pushstring(L, e->data);
                lua_pcall(L, 1, 0, 0);
            }
            lua_pop(L, 1);
            break;
            
        case EMSCRIPTEN_WEBSOCKET_CLOSE:
            inst->connected = 0;
            lua_getfield(L, -1, "on_close");
            if (lua_isfunction(L, -1)) {
                lua_pushinteger(L, e->closeCode);
                lua_pushstring(L, e->reason);
                lua_pcall(L, 2, 0, 0);
            }
            lua_pop(L, 1);
            break;
    }
    
    lua_pop(L, 1);
}

static int lvgl_websocket_create(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    
    int idx = ws_find_free_slot();
    if (idx < 0) {
        return luaL_error(L, "websocket: max instances exceeded");
    }
    
    EMSCRIPTEN_WEBSOCKET_T ws = emscripten_websocket_new(url, NULL, NULL, ws_callback, (void*)(intptr_t)idx);
    if (ws == 0) {
        return luaL_error(L, "websocket: failed to create");
    }
    
    ws_instances[idx].ws = ws;
    ws_instances[idx].connected = 0;
    
    lua_newtable(L);
    
    lua_pushstring(L, url);
    lua_setfield(L, -2, "url");
    
    lua_pushinteger(L, idx);
    lua_setfield(L, -2, "__index");
    
    lua_pushvalue(L, -1);
    ws_instances[idx].lua_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    
    return 1;
}

static int lvgl_websocket_send(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    size_t len;
    const char* data = luaL_checklstring(L, 2, &len);
    
    if (idx < 0 || idx >= MAX_WS_INSTANCES) {
        return luaL_error(L, "websocket: invalid instance");
    }
    
    ws_instance_t* inst = &ws_instances[idx];
    if (!inst->ws || !inst->connected) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    int ret = emscripten_websocket_send_binary(inst->ws, data, len);
    lua_pushboolean(L, ret == 0);
    return 1;
}

static int lvgl_websocket_send_text(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    const char* data = luaL_checkstring(L, 2);
    
    if (idx < 0 || idx >= MAX_WS_INSTANCES) {
        return luaL_error(L, "websocket: invalid instance");
    }
    
    ws_instance_t* inst = &ws_instances[idx];
    if (!inst->ws || !inst->connected) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    int ret = emscripten_websocket_send_text(inst->ws, data);
    lua_pushboolean(L, ret == 0);
    return 1;
}

static int lvgl_websocket_close(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    
    if (idx < 0 || idx >= MAX_WS_INSTANCES) {
        return luaL_error(L, "websocket: invalid instance");
    }
    
    ws_instance_t* inst = &ws_instances[idx];
    if (inst->ws) {
        emscripten_websocket_close(inst->ws, 1000, "normal close");
        if (inst->lua_ref != LUA_NOREF) {
            luaL_unref(lua_gettop(L) > 0 ? L : NULL, LUA_REGISTRYINDEX, inst->lua_ref);
            inst->lua_ref = LUA_NOREF;
        }
        inst->ws = 0;
        inst->connected = 0;
    }
    
    return 0;
}

static int lvgl_websocket_is_connected(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    
    if (idx < 0 || idx >= MAX_WS_INSTANCES) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    lua_pushboolean(L, ws_instances[idx].connected);
    return 1;
}

static const luaL_Reg websocket_methods[] = {
    {"create", lvgl_websocket_create},
    {"send", lvgl_websocket_send},
    {"send_text", lvgl_websocket_send_text},
    {"close", lvgl_websocket_close},
    {"is_connected", lvgl_websocket_is_connected},
    {NULL, NULL}
};

LUAMOD_API int luaopen_websocket_register(lua_State* L) {
    lua_newtable(L);
    luaL_setfuncs(L, websocket_methods, 0);
    return 1;
}