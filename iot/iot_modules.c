/*
@module  modules
@summary Lua模块注册入口
@version 1.0
@date    2026.06.10
@author  杰神 & TRAE & ChatGPT
*/

#include "iot_modules.h"

/* 系统基础模块 */
#include "iot_os.h"
#include "iot_rtos.h"
#include "iot_pm.h"
#include "iot_at.h"
#include "iot_log.h"
#include "iot_mem.h"
#include "iot_cpu.h"

/* 文件系统模块 */
#include "iot_fs.h"
#include "iot_flash.h"

/* 外设控制模块 */
#include "iot_gpio.h"
#include "iot_uart.h"
#include "iot_i2c.h"
#include "iot_spi.h"
#include "iot_pwm.h"
#include "iot_adc.h"
#include "iot_wdt.h"
#include "iot_rtc.h"

/* 可选外设模块 */
#ifdef ENABLE_CAMERA
#include "iot_camera.h"  /* 相机模块 */
#endif

#ifdef ENABLE_GNSS
#include "iot_gnss.h"  /* GNSS定位 */
#endif

#ifdef ENABLE_LCD
#include "iot_lcd.h"   /* LCD显示模块 */
#endif

/* 网络通信模块 */
#include "iot_wifi.h"
#include "iot_mobile.h"
#include "iot_sim.h"
#include "iot_socket.h"
#include "iot_http.h"
#include "iot_mqtt.h"
#include "iot_ntp.h"
#include "iot_sms.h"  /* 短信模块 */

#ifdef ENABLE_LWM2M
#include "iot_lwm2m.h" /* LwM2M协议 */
#endif

/* 数据处理模块 */
#include "iot_pack.h"
#include "iot_json.h"
#include "iot_yaml.h"
#include "iot_base64.h"
#include "iot_zlib.h"

#ifdef ENABLE_SQLITE3
#include "iot_sqlite3.h"
#endif

#ifdef ENABLE_CJSON
extern int luaopen_cjson(lua_State* L);
extern int luaopen_cjson_safe(lua_State* L);
#endif

#ifdef ENABLE_FONTS
#include "iot_font.h"
#endif

#ifdef ENABLE_GMSSL
#include "iot_gmssl.h"
#endif

#ifdef ENABLE_JPEG
#include "iot_jpeg.h"
#endif

#ifdef ENABLE_QRENCODE
#include "iot_qrcode.h"
#endif

#ifdef ENABLE_QUIRC
#include "iot_qrcode.h"
#endif

#ifdef ENABLE_LVGL
#include "lua-lvgl/lvgl.h"
#endif

/* 固件升级模块 */
#include "iot_fota.h"

/* 音频模块（实装或 stub 均导出 luaopen_audio/tts） */
#include "iot_audio.h"
#include "iot_tts.h"

/* 基础模块列表 */
static const luaL_Reg base_modules[] = {
    /* ================ 系统基础模块 ================ */
    {"os",     luaopen_os},           /* 操作系统接口 */
    {"rtos",   luaopen_rtos},         /* 实时操作系统接口 */
    {"pm",     luaopen_pm},           /* 电源管理 */
    {"at",     luaopen_at},           /* AT命令接口 */
    {"log",    luaopen_log},          /* 日志系统 */
    {"mem",    luaopen_mem},          /* 内存管理 */
    {"cpu",    luaopen_cpu},          /* CPU信息 */

    /* ================ 文件系统模块 ================ */
    {"fs",     luaopen_fs},           /* 文件系统 */
    {"flash",  luaopen_flash},        /* Flash操作 */

    /* ================ 外设控制模块 ================ */
    {"gpio",   luaopen_gpio},         /* 通用IO */
    {"uart",   luaopen_uart},         /* 串口 */
    {"i2c",    luaopen_i2c},          /* I2C总线 */
    {"spi",    luaopen_spi},          /* SPI总线 */
    {"pwm",    luaopen_pwm},          /* PWM输出 */
    {"adc",    luaopen_adc},          /* 模数转换 */
    {"wdt",    luaopen_wdt},          /* 看门狗 */
    {"rtc",    luaopen_rtc},          /* 实时时钟 */

    /* ================ 网络通信模块 ================ */
    {"wifi",   luaopen_wifi},         /* WiFi功能 */
    {"mobile", luaopen_mobile},       /* 移动网络 */
    {"sim",    luaopen_sim},          /* SIM卡管理 */
    {"socket", luaopen_socket},       /* Socket网络 */
    {"http",   luaopen_http},         /* HTTP协议 */
    {"mqtt",   luaopen_mqtt},         /* MQTT协议 */
    {"ntp",    luaopen_ntp},          /* NTP时间同步 */
    {"sms",    luaopen_sms},          /* 短信功能 */

    /* ================ 数据处理模块 ================ */
    {"pack",   luaopen_pack},         /* 数据打包/解包 */
    {"json",   luaopen_json},         /* JSON解析 */
    {"yaml",   luaopen_yaml},         /* YAML解析 */
    {"base64", luaopen_base64},       /* Base64编解码 */
    {"zlib",   luaopen_zlib},         /* 压缩解压 */

    /* ================ 固件升级模块 ================ */
    {"fota",   luaopen_fota},         /* 固件空中升级 */

    /* ================ 音频模块（dc 板为 stub） ================ */
    {"audio",  luaopen_audio},        /* 音频播放 */
    {"tts",    luaopen_tts},          /* 语音合成 */

    {NULL, NULL}
};

/**
 * @brief 注册单个模块
 * @param L Lua状态机
 * @param name 模块名称
 * @param func 模块初始化函数
 * @return 是否注册成功
 */
static int register_module(lua_State* L, const char* name, lua_CFunction func)
{
    luaL_requiref(L, name, func, 1);
    lua_pop(L, 1);
    return 1;
}

/**
 * @brief 注册所有Lua模块
 * @param L Lua状态机
 */
void modules_register(lua_State* L)
{
    const luaL_Reg *lib;
    int count = 0;
    
    LOG("register modules...");
    
    /* 注册基础模块 */
    for (lib = base_modules; lib->func; lib++) {
        register_module(L, lib->name, lib->func);
        count++;
    }
    
    /* 注册可选模块 */
    
#ifdef ENABLE_CAMERA
    register_module(L, "camera", luaopen_camera);
    count++;
    LOG("  camera module enabled");
#endif
    
#ifdef ENABLE_GNSS
    register_module(L, "gnss", luaopen_gnss);
    count++;
    LOG("  gnss module enabled");
#endif

#ifdef ENABLE_LCD
    register_module(L, "lcd", luaopen_lcd);
    count++;
    LOG("  lcd module enabled");
#endif

#ifdef ENABLE_LWM2M
    register_module(L, "lwm2m", luaopen_lwm2m);
    count++;
    LOG("  lwm2m module enabled");
#endif

#ifdef ENABLE_SQLITE3
    register_module(L, "sqlite3", luaopen_sqlite3);
    count++;
    LOG("  sqlite3 module enabled");
#endif

#ifdef ENABLE_CJSON
    register_module(L, "cjson", luaopen_cjson);
    count++;
    LOG("  cjson module enabled");
#endif

#ifdef ENABLE_FONTS
    register_module(L, "font", luaopen_font);
    count++;
    LOG("  font module enabled");
#endif

#ifdef ENABLE_GMSSL
    register_module(L, "gmssl", luaopen_gmssl);
    count++;
    LOG("  gmssl module enabled");
#endif

#ifdef ENABLE_JPEG
    register_module(L, "jpeg", luaopen_jpeg);
    count++;
    LOG("  jpeg module enabled");
#endif

#ifdef ENABLE_QRENCODE
    register_module(L, "qrcode", luaopen_qrcode);
    count++;
    LOG("  qrcode (encode) module enabled");
#endif

#ifdef ENABLE_QUIRC
#ifndef ENABLE_QRENCODE
    register_module(L, "qrcode", luaopen_qrcode);
    count++;
    LOG("  qrcode (decode) module enabled");
#endif
#endif

#ifdef ENABLE_LVGL
    register_module(L, "lvgl", luaopen_lvgl);
    count++;
    LOG("  lvgl module enabled");
#endif

    LOG("total %d modules", count);
}