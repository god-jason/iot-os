/*
@module  fetch
@summary WebAssembly Fetch API模块
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
#include "emscripten/fetch.h"

#define MAX_FETCH_INSTANCES 32

typedef struct {
    emscripten_fetch_t* fetch;
    int lua_ref;
    int completed;
    char* response_data;
    size_t response_size;
} fetch_instance_t;

static fetch_instance_t fetch_instances[MAX_FETCH_INSTANCES];

static int fetch_find_free_slot(void) {
    for (int i = 0; i < MAX_FETCH_INSTANCES; i++) {
        if (fetch_instances[i].fetch == NULL) {
            return i;
        }
    }
    return -1;
}

static void fetch_on_success(emscripten_fetch_t* fetch) {
    int idx = (int)(intptr_t)fetch->userData;
    if (idx < 0 || idx >= MAX_FETCH_INSTANCES) return;
    
    fetch_instance_t* inst = &fetch_instances[idx];
    inst->completed = 1;
    
    if (fetch->numBytes > 0 && fetch->data != NULL) {
        inst->response_size = fetch->numBytes;
        inst->response_data = (char*)malloc(fetch->numBytes + 1);
        if (inst->response_data) {
            memcpy(inst->response_data, fetch->data, fetch->numBytes);
            inst->response_data[fetch->numBytes] = '\0';
        }
    }
    
    lua_State* L = NULL;
    extern lua_State* iot_get_lua_state(void);
    L = iot_get_lua_state();
    if (!L || inst->lua_ref == LUA_NOREF) {
        emscripten_fetch_close(fetch);
        inst->fetch = NULL;
        return;
    }
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, inst->lua_ref);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        emscripten_fetch_close(fetch);
        inst->fetch = NULL;
        return;
    }
    
    lua_getfield(L, -1, "on_success");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, fetch->status);
        lua_pushlstring(L, fetch->data, fetch->numBytes);
        lua_pcall(L, 2, 0, 0);
    }
    lua_pop(L, 1);
    
    lua_pop(L, 1);
    emscripten_fetch_close(fetch);
    inst->fetch = NULL;
}

static void fetch_on_error(emscripten_fetch_t* fetch) {
    int idx = (int)(intptr_t)fetch->userData;
    if (idx < 0 || idx >= MAX_FETCH_INSTANCES) return;
    
    fetch_instance_t* inst = &fetch_instances[idx];
    inst->completed = 1;
    
    lua_State* L = NULL;
    extern lua_State* iot_get_lua_state(void);
    L = iot_get_lua_state();
    if (!L || inst->lua_ref == LUA_NOREF) {
        emscripten_fetch_close(fetch);
        inst->fetch = NULL;
        return;
    }
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, inst->lua_ref);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        emscripten_fetch_close(fetch);
        inst->fetch = NULL;
        return;
    }
    
    lua_getfield(L, -1, "on_error");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, fetch->status);
        lua_pushstring(L, fetch->data);
        lua_pcall(L, 2, 0, 0);
    }
    lua_pop(L, 1);
    
    lua_pop(L, 1);
    emscripten_fetch_close(fetch);
    inst->fetch = NULL;
}

static void fetch_on_progress(emscripten_fetch_t* fetch) {
    int idx = (int)(intptr_t)fetch->userData;
    if (idx < 0 || idx >= MAX_FETCH_INSTANCES) return;
    
    fetch_instance_t* inst = &fetch_instances[idx];
    
    lua_State* L = NULL;
    extern lua_State* iot_get_lua_state(void);
    L = iot_get_lua_state();
    if (!L || inst->lua_ref == LUA_NOREF) return;
    
    lua_rawgeti(L, LUA_REGISTRYINDEX, inst->lua_ref);
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    
    lua_getfield(L, -1, "on_progress");
    if (lua_isfunction(L, -1)) {
        lua_pushinteger(L, fetch->dataOffset);
        lua_pushinteger(L, fetch->numBytes);
        lua_pushinteger(L, fetch->totalBytes);
        lua_pcall(L, 3, 0, 0);
    }
    lua_pop(L, 1);
    
    lua_pop(L, 1);
}

static int wasm_fetch_get(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    
    int idx = fetch_find_free_slot();
    if (idx < 0) {
        return luaL_error(L, "fetch: max instances exceeded");
    }
    
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
    attr.onsuccess = fetch_on_success;
    attr.onerror = fetch_on_error;
    attr.onprogress = fetch_on_progress;
    attr.userData = (void*)(intptr_t)idx;
    
    emscripten_fetch_t* fetch = emscripten_fetch(&attr, url);
    if (!fetch) {
        return luaL_error(L, "fetch: failed to create request");
    }
    
    fetch_instances[idx].fetch = fetch;
    fetch_instances[idx].completed = 0;
    fetch_instances[idx].response_data = NULL;
    fetch_instances[idx].response_size = 0;
    
    if (lua_istable(L, 2)) {
        lua_pushvalue(L, 2);
        fetch_instances[idx].lua_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    } else {
        fetch_instances[idx].lua_ref = LUA_NOREF;
    }
    
    lua_newtable(L);
    lua_pushinteger(L, fetch->status);
    lua_setfield(L, -2, "status");
    
    if (fetch_instances[idx].response_data) {
        lua_pushlstring(L, fetch_instances[idx].response_data, fetch_instances[idx].response_size);
        lua_setfield(L, -2, "body");
        free(fetch_instances[idx].response_data);
        fetch_instances[idx].response_data = NULL;
    }
    
    lua_pushstring(L, url);
    lua_setfield(L, -2, "url");
    
    emscripten_fetch_close(fetch);
    fetch_instances[idx].fetch = NULL;
    
    return 1;
}

static int wasm_fetch_post(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    size_t body_len;
    const char* body = luaL_optlstring(L, 2, "", &body_len);
    const char* content_type = luaL_optstring(L, 3, "application/json");
    
    int idx = fetch_find_free_slot();
    if (idx < 0) {
        return luaL_error(L, "fetch: max instances exceeded");
    }
    
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "POST");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY | EMSCRIPTEN_FETCH_SYNCHRONOUS;
    attr.requestData = body;
    attr.requestDataSize = body_len;
    
    char headers[256];
    snprintf(headers, sizeof(headers), "Content-Type: %s", content_type);
    attr.requestHeaders = headers;
    
    attr.onsuccess = fetch_on_success;
    attr.onerror = fetch_on_error;
    attr.userData = (void*)(intptr_t)idx;
    
    emscripten_fetch_t* fetch = emscripten_fetch(&attr, url);
    if (!fetch) {
        return luaL_error(L, "fetch: failed to create request");
    }
    
    fetch_instances[idx].fetch = fetch;
    fetch_instances[idx].completed = 0;
    fetch_instances[idx].response_data = NULL;
    fetch_instances[idx].response_size = 0;
    
    if (lua_istable(L, 4)) {
        lua_pushvalue(L, 4);
        fetch_instances[idx].lua_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    } else {
        fetch_instances[idx].lua_ref = LUA_NOREF;
    }
    
    lua_newtable(L);
    lua_pushinteger(L, fetch->status);
    lua_setfield(L, -2, "status");
    
    if (fetch_instances[idx].response_data) {
        lua_pushlstring(L, fetch_instances[idx].response_data, fetch_instances[idx].response_size);
        lua_setfield(L, -2, "body");
        free(fetch_instances[idx].response_data);
        fetch_instances[idx].response_data = NULL;
    }
    
    emscripten_fetch_close(fetch);
    fetch_instances[idx].fetch = NULL;
    
    return 1;
}

static int wasm_fetch_async(lua_State* L) {
    const char* url = luaL_checkstring(L, 1);
    const char* method = luaL_optstring(L, 2, "GET");
    size_t body_len;
    const char* body = luaL_optlstring(L, 3, "", &body_len);
    
    if (!lua_istable(L, 4)) {
        return luaL_error(L, "fetch_async: callback table required");
    }
    
    int idx = fetch_find_free_slot();
    if (idx < 0) {
        return luaL_error(L, "fetch: max instances exceeded");
    }
    
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, method);
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = fetch_on_success;
    attr.onerror = fetch_on_error;
    attr.onprogress = fetch_on_progress;
    attr.userData = (void*)(intptr_t)idx;
    
    if (body_len > 0) {
        attr.requestData = body;
        attr.requestDataSize = body_len;
    }
    
    emscripten_fetch_t* fetch = emscripten_fetch(&attr, url);
    if (!fetch) {
        return luaL_error(L, "fetch: failed to create request");
    }
    
    fetch_instances[idx].fetch = fetch;
    fetch_instances[idx].completed = 0;
    fetch_instances[idx].response_data = NULL;
    fetch_instances[idx].response_size = 0;
    
    lua_pushvalue(L, 4);
    fetch_instances[idx].lua_ref = luaL_ref(L, LUA_REGISTRYINDEX);
    
    lua_pushinteger(L, idx);
    return 1;
}

static int wasm_fetch_is_completed(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    
    if (idx < 0 || idx >= MAX_FETCH_INSTANCES) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    lua_pushboolean(L, fetch_instances[idx].completed);
    return 1;
}

static int wasm_fetch_get_response(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    
    if (idx < 0 || idx >= MAX_FETCH_INSTANCES) {
        lua_pushnil(L);
        return 1;
    }
    
    fetch_instance_t* inst = &fetch_instances[idx];
    
    lua_newtable(L);
    lua_pushinteger(L, inst->fetch ? inst->fetch->status : 0);
    lua_setfield(L, -2, "status");
    
    if (inst->response_data) {
        lua_pushlstring(L, inst->response_data, inst->response_size);
        lua_setfield(L, -2, "body");
    }
    
    return 1;
}

static int wasm_fetch_cancel(lua_State* L) {
    int idx = luaL_checkinteger(L, 1);
    
    if (idx < 0 || idx >= MAX_FETCH_INSTANCES) {
        return 0;
    }
    
    fetch_instance_t* inst = &fetch_instances[idx];
    if (inst->fetch) {
        emscripten_fetch_close(inst->fetch);
        inst->fetch = NULL;
    }
    if (inst->lua_ref != LUA_NOREF) {
        luaL_unref(L, LUA_REGISTRYINDEX, inst->lua_ref);
        inst->lua_ref = LUA_NOREF;
    }
    if (inst->response_data) {
        free(inst->response_data);
        inst->response_data = NULL;
    }
    inst->completed = 0;
    
    return 0;
}

static const luaL_Reg fetch_methods[] = {
    {"get", wasm_fetch_get},
    {"post", wasm_fetch_post},
    {"async", wasm_fetch_async},
    {"is_completed", wasm_fetch_is_completed},
    {"get_response", wasm_fetch_get_response},
    {"cancel", wasm_fetch_cancel},
    {NULL, NULL}
};

LUAMOD_API int luaopen_fetch_register(lua_State* L) {
    lua_newtable(L);
    luaL_setfuncs(L, fetch_methods, 0);
    return 1;
}