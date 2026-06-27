/**
 * @file iot.c
 * @brief WebAssembly 平台初始化实现
 */

#include "iot.h"
#include "iot_wdt.h"

static int g_main_loop_id = 0;

int net_port_set_nonblocking(iot_socket_t fd)
{
    return 0;
}

void iot_event_init(void) {
}

void iot_event_deinit(void) {
}

static void wasm_main_loop(void) {
    lv_task_handler();
}

extern void iot_start(void);

int main(void) {
    iot_start();
    
    g_main_loop_id = emscripten_set_main_loop(wasm_main_loop, 0, 1);
    
    return 0;
}

EMSCRIPTEN_KEEPALIVE
void iot_wasm_stop(void) {
    if (g_main_loop_id != 0) {
        emscripten_cancel_main_loop();
        g_main_loop_id = 0;
    }
}