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
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

#define IOT_UART_MAX   4

typedef struct {
    int inited;
    int fd;
    char device[32];
    struct termios old_tio;
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

static void* uart_read_thread(void* arg) {
    fd_set read_set;
    int max_fd;
    
    while (g_uart_thread_running) {
        iot_mutex_lock(g_uart_mutex, -1);
        
        FD_ZERO(&read_set);
        max_fd = 0;
        
        for (int i = 0; i < IOT_UART_MAX; i++) {
            if (g_uart_ctx[i].inited && g_uart_ctx[i].fd >= 0) {
                FD_SET(g_uart_ctx[i].fd, &read_set);
                if (g_uart_ctx[i].fd > max_fd) {
                    max_fd = g_uart_ctx[i].fd;
                }
            }
        }
        
        iot_mutex_unlock(g_uart_mutex);
        
        if (max_fd == 0) {
            iot_task_delay(10);
            continue;
        }
        
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 10000;
        
        int ret = select(max_fd + 1, &read_set, NULL, NULL, &timeout);
        if (ret < 0) {
            continue;
        }
        
        if (ret == 0) continue;
        
        for (int i = 0; i < IOT_UART_MAX; i++) {
            if (g_uart_ctx[i].inited && g_uart_ctx[i].fd >= 0 && FD_ISSET(g_uart_ctx[i].fd, &read_set)) {
                uart_event_cb(i);
            }
        }
    }
    
    return NULL;
}

static void uart_start_read_thread(void) {
    if (g_uart_thread_running) return;
    
    if (!g_uart_mutex) {
        g_uart_mutex = iot_mutex_create();
    }
    
    g_uart_thread_running = 1;
    g_uart_thread = iot_task_create("uart_read", uart_read_thread, NULL, 4096, IOT_OS_PRIO_NORMAL);
}

static int iot_uart_setup(lua_State* L) {
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
        tcsetattr(g_uart_ctx[id].fd, TCSANOW, &g_uart_ctx[id].old_tio);
        close(g_uart_ctx[id].fd);
        g_uart_ctx[id].inited = 0;
    }
    
    snprintf(g_uart_ctx[id].device, sizeof(g_uart_ctx[id].device), "/dev/ttyS%d", id);
    
    int fd = open(g_uart_ctx[id].device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    struct termios tio;
    tcgetattr(fd, &g_uart_ctx[id].old_tio);
    memset(&tio, 0, sizeof(tio));
    
    tio.c_cflag |= CLOCAL | CREAD;
    
    switch (databits) {
        case 5: tio.c_cflag |= CS5; break;
        case 6: tio.c_cflag |= CS6; break;
        case 7: tio.c_cflag |= CS7; break;
        default: tio.c_cflag |= CS8; break;
    }
    
    switch (parity) {
        case 1: tio.c_cflag |= PARENB | PARODD; break;
        case 2: tio.c_cflag |= PARENB; break;
        default: tio.c_cflag &= ~PARENB; break;
    }
    
    switch (stopbits) {
        case 2: tio.c_cflag |= CSTOPB; break;
        default: tio.c_cflag &= ~CSTOPB; break;
    }
    
    switch (flowctl) {
        case 1: tio.c_cflag |= CRTSCTS; break;
        default: tio.c_cflag &= ~CRTSCTS; break;
    }
    
    tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tio.c_oflag &= ~OPOST;
    tio.c_cc[VMIN] = 1;
    tio.c_cc[VTIME] = 0;
    
    speed_t speed;
    switch (baudrate) {
        case 2400: speed = B2400; break;
        case 4800: speed = B4800; break;
        case 9600: speed = B9600; break;
        case 19200: speed = B19200; break;
        case 38400: speed = B38400; break;
        case 57600: speed = B57600; break;
        case 115200: speed = B115200; break;
        case 230400: speed = B230400; break;
        case 460800: speed = B460800; break;
        case 921600: speed = B921600; break;
        default: speed = B9600; break;
    }
    
    cfsetispeed(&tio, speed);
    cfsetospeed(&tio, speed);
    
    tcsetattr(fd, TCSANOW, &tio);
    
    g_uart_ctx[id].fd = fd;
    g_uart_ctx[id].inited = 1;
    
    uart_start_read_thread();
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_uart_close(lua_State* L) {
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
    
    tcsetattr(g_uart_ctx[id].fd, TCSANOW, &g_uart_ctx[id].old_tio);
    close(g_uart_ctx[id].fd);
    g_uart_ctx[id].inited = 0;
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_uart_write(lua_State* L) {
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
    
    int ret = write(g_uart_ctx[id].fd, data, len);
    if (ret < 0) {
        lua_pushnil(L);
    } else {
        lua_pushinteger(L, ret);
    }
    return 1;
}

static int iot_uart_read(lua_State* L) {
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
    
    int ret = read(g_uart_ctx[id].fd, buff, len);
    if (ret <= 0) {
        iot_free(buff);
        lua_pushnil(L);
        return 1;
    }
    
    lua_pushlstring(L, buff, ret);
    iot_free(buff);
    return 1;
}

static int iot_uart_get_rxrb_data_len(lua_State* L) {
    int id = luaL_checkinteger(L, 1);
    
    if (id < 0 || id >= IOT_UART_MAX) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    if (!g_uart_ctx[id].inited) {
        lua_pushinteger(L, 0);
        return 1;
    }
    
    int len = 0;
    ioctl(g_uart_ctx[id].fd, FIONREAD, &len);
    
    lua_pushinteger(L, len);
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
    
    tcflush(g_uart_ctx[id].fd, TCIOFLUSH);
    
    lua_pushboolean(L, 1);
    return 1;
}

static int iot_uart_on(lua_State* L) {
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

static const luaL_Reg uart_lib[] = {
    { "setup",   iot_uart_setup },
    { "close",   iot_uart_close },
    { "write",   iot_uart_write },
    { "read",    iot_uart_read },
    { "on",      iot_uart_on },
    { "get_rxrb_data_len", iot_uart_get_rxrb_data_len },
    { "clean",   iot_uart_clean },
    {NULL, NULL}
};

LUAMOD_API int luaopen_uart(lua_State* L) {
    luaL_newlibtable(L, uart_lib);
    luaL_setfuncs(L, uart_lib, 0);
    return 1;
}
