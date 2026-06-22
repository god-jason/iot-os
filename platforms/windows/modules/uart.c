/*
@module  uart
@summary UART串口通信
@version 1.0
@date    2026.06.19
@tag     UART
*/

#include "module.h"
#include "iot_callback.h"
#include "iot_params.h"
#include <windows.h>

#define IOT_UART_MAX   4

typedef struct {
    int inited;
    HANDLE hCom;
    char device[32];
    void* callback_ud;
} iot_uart_ctx_t;

static iot_uart_ctx_t g_uart_ctx[IOT_UART_MAX] = {0};
static iot_mutex_t g_uart_mutex = NULL;
static int g_uart_thread_running = 0;
static iot_task_t g_uart_thread = NULL;

static void uart_event_cb(int id) {
    if (id < 0 || id >= IOT_UART_MAX) return;
    
    if (g_uart_ctx[id].callback_ud) {
        iot_callback_call(g_uart_ctx[id].callback_ud, NULL);
    }
}

static DWORD WINAPI uart_read_thread(LPVOID arg) {
    HANDLE events[IOT_UART_MAX + 1];
    int event_count;
    
    while (g_uart_thread_running) {
        iot_mutex_lock(g_uart_mutex, -1);
        
        event_count = 0;
        for (int i = 0; i < IOT_UART_MAX; i++) {
            if (g_uart_ctx[i].inited && g_uart_ctx[i].hCom != INVALID_HANDLE_VALUE) {
                events[event_count++] = g_uart_ctx[i].hCom;
            }
        }
        
        iot_mutex_unlock(g_uart_mutex);
        
        if (event_count == 0) {
            iot_task_delay(10);
            continue;
        }
        
        DWORD ret = WaitForMultipleObjects(event_count, events, FALSE, 10);
        if (ret == WAIT_TIMEOUT) continue;
        if (ret == WAIT_FAILED) continue;
        
        int idx = ret - WAIT_OBJECT_0;
        if (idx < 0 || idx >= event_count) continue;
        
        for (int i = 0; i < IOT_UART_MAX; i++) {
            if (g_uart_ctx[i].inited && g_uart_ctx[i].hCom == events[idx]) {
                uart_event_cb(i);
                break;
            }
        }
    }
    
    return 0;
}

static void uart_start_read_thread(void) {
    if (g_uart_thread_running) return;
    
    if (!g_uart_mutex) {
        g_uart_mutex = iot_mutex_create();
    }
    
    g_uart_thread_running = 1;
    g_uart_thread = iot_task_create("uart_read", uart_read_thread, NULL, 4096, IOT_OS_PRIO_NORMAL);
}

static int luaopen_uart_setup(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int baudrate = luaL_checkinteger(L, 2);
    int databits = luaL_optinteger(L, 3, 8);
    int parity = luaL_optinteger(L, 4, 0);
    int stopbits = luaL_optinteger(L, 5, 0);
    int flowctl = luaL_optinteger(L, 6, 0);
    
    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    if (g_uart_ctx[id].inited) {
        CloseHandle(g_uart_ctx[id].hCom);
        g_uart_ctx[id].inited = 0;
    }
    
    snprintf(g_uart_ctx[id].device, sizeof(g_uart_ctx[id].device), "COM%d", id + 1);
    
    HANDLE hCom = CreateFile(g_uart_ctx[id].device,
                             GENERIC_READ | GENERIC_WRITE,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                             NULL);
    
    if (hCom == INVALID_HANDLE_VALUE) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    DCB dcb;
    memset(&dcb, 0, sizeof(dcb));
    dcb.DCBlength = sizeof(dcb);
    
    if (!GetCommState(hCom, &dcb)) {
        CloseHandle(hCom);
        lua_pushboolean(L, 0);
        return 1;
    }
    
    dcb.BaudRate = baudrate;
    dcb.ByteSize = (BYTE)databits;
    
    switch (parity) {
        case 1: dcb.Parity = ODDPARITY; break;
        case 2: dcb.Parity = EVENPARITY; break;
        default: dcb.Parity = NOPARITY; break;
    }
    
    switch (stopbits) {
        case 2: dcb.StopBits = TWOSTOPBITS; break;
        default: dcb.StopBits = ONESTOPBIT; break;
    }
    
    dcb.fOutxCtsFlow = (flowctl == 1) ? TRUE : FALSE;
    dcb.fRtsControl = (flowctl == 1) ? RTS_CONTROL_HANDSHAKE : RTS_CONTROL_DISABLE;
    dcb.fOutX = FALSE;
    dcb.fInX = FALSE;
    
    if (!SetCommState(hCom, &dcb)) {
        CloseHandle(hCom);
        lua_pushboolean(L, 0);
        return 1;
    }
    
    COMMTIMEOUTS timeouts;
    memset(&timeouts, 0, sizeof(timeouts));
    timeouts.ReadIntervalTimeout = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    
    SetCommTimeouts(hCom, &timeouts);
    
    PurgeComm(hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
    
    g_uart_ctx[id].hCom = hCom;
    g_uart_ctx[id].inited = 1;
    
    uart_start_read_thread();
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_uart_close(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    
    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    if (!g_uart_ctx[id].inited) {
        lua_pushboolean(L, 1);
        return 1;
    }
    
    if (g_uart_ctx[id].callback_ud) {
        iot_callback_free(g_uart_ctx[id].callback_ud);
        g_uart_ctx[id].callback_ud = NULL;
    }
    
    CloseHandle(g_uart_ctx[id].hCom);
    g_uart_ctx[id].inited = 0;
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_uart_write(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    size_t len = 0;
    const char* data = luaL_checklstring(L, 2, &len);
    
    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushnil(L);
        return 1;
    }
    
    if (!g_uart_ctx[id].inited) {
        lua_pushnil(L);
        return 1;
    }
    
    DWORD written;
    if (!WriteFile(g_uart_ctx[id].hCom, data, (DWORD)len, &written, NULL)) {
        lua_pushnil(L);
    } else {
        lua_pushinteger(L, written);
    }
    return 1;
}

static int luaopen_uart_read(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    int len = luaL_checkinteger(L, 2);
    
    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushnil(L);
        return 1;
    }
    
    if (!g_uart_ctx[id].inited) {
        lua_pushnil(L);
        return 1;
    }
    
    if (len <= 0) {
        lua_pushnil(L);
        return 1;
    }
    
    char* buff = (char*)iot_malloc(len);
    if (!buff) {
        lua_pushnil(L);
        return 1;
    }
    
    DWORD read;
    if (!ReadFile(g_uart_ctx[id].hCom, buff, (DWORD)len, &read, NULL)) {
        iot_free(buff);
        lua_pushnil(L);
        return 1;
    }
    
    if (read == 0) {
        iot_free(buff);
        lua_pushnil(L);
        return 1;
    }
    
    lua_pushlstring(L, buff, read);
    iot_free(buff);
    return 1;
}

static int luaopen_uart_get_rxrb_data_len(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    
    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    if (!g_uart_ctx[id].inited) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    COMSTAT stat;
    DWORD errors;
    if (!ClearCommError(g_uart_ctx[id].hCom, &errors, &stat)) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    lua_pushinteger(L, stat.cbInQue);
    return 1;
}

static int iot_uart_clean(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    
    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    if (!g_uart_ctx[id].inited) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    PurgeComm(g_uart_ctx[id].hCom, PURGE_TXCLEAR | PURGE_RXCLEAR);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int luaopen_uart_on(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    luaL_checktype(L, 2, LUA_TFUNCTION);
    
    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    if (g_uart_ctx[id].callback_ud) {
        iot_callback_free(g_uart_ctx[id].callback_ud);
    }
    
    void* cb_ud = iot_callback_save(L, 2);
    if (!cb_ud) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    g_uart_ctx[id].callback_ud = cb_ud;
    
    lua_pushboolean(L, 1);
    return 1;
}

static const luaL_Reg luaopen_uart_lib[] = {
    { "setup",   luaopen_uart_setup },
    { "close",   luaopen_uart_close },
    { "write",   luaopen_uart_write },
    { "read",    luaopen_uart_read },
    { "on",      luaopen_uart_on },
    { "get_rxrb_data_len", luaopen_uart_get_rxrb_data_len },
    { "clean",   luaopen_uart_clean },
    {NULL, NULL}
};

LUAMOD_API int luaopen_uart_register(lua_State* L) {
    luaL_newlibtable(L, luaopen_uart_lib);
    luaL_setfuncs(L, luaopen_uart_lib, 0);
    return 1;
}
