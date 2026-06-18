/*
@module  platform_modules
@summary 平台模块注册入口
@version 1.0
@date    2026.06.19
@author  TRAE
*/

/* luaopen 函数声明 */
extern int luaopen_os(lua_State* L);
extern int luaopen_pm(lua_State* L);
extern int luaopen_at(lua_State* L);
extern int luaopen_log(lua_State* L);
extern int luaopen_mem(lua_State* L);
extern int luaopen_cpu(lua_State* L);
extern int luaopen_fs(lua_State* L);
extern int luaopen_flash(lua_State* L);
extern int luaopen_gpio(lua_State* L);
extern int luaopen_uart(lua_State* L);
extern int luaopen_i2c(lua_State* L);
extern int luaopen_spi(lua_State* L);
extern int luaopen_pwm(lua_State* L);
extern int luaopen_adc(lua_State* L);
extern int luaopen_wdt(lua_State* L);
extern int luaopen_rtc(lua_State* L);
extern int luaopen_wifi(lua_State* L);
extern int luaopen_mobile(lua_State* L);
extern int luaopen_sim(lua_State* L);
extern int luaopen_socket(lua_State* L);
extern int luaopen_http(lua_State* L);
extern int luaopen_mqtt(lua_State* L);
extern int luaopen_ntp(lua_State* L);
extern int luaopen_sms(lua_State* L);
extern int luaopen_fota(lua_State* L);
extern int luaopen_audio(lua_State* L);
extern int luaopen_tts(lua_State* L);
extern int luaopen_camera(lua_State* L);
extern int luaopen_lcd(lua_State* L);
extern int luaopen_gnss(lua_State* L);

/* 平台模块列表 */
static const luaL_Reg platform_modules[] = {
    /* ================ 系统基础模块 ================ */
    {"os",     luaopen_os},           /* 操作系统接口 */
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

    /* ================ 固件升级模块 ================ */
    {"fota",   luaopen_fota},         /* 固件空中升级 */

    /* ================ 音频模块 ================ */
    {"audio",  luaopen_audio},        /* 音频播放 */
    {"tts",    luaopen_tts},          /* 语音合成 */

    /* ================ 可选模块 ================ */
    {"camera", luaopen_camera},        /* 相机模块 */
    {"lcd",    luaopen_lcd},          /* LCD显示 */
    {"gnss",   luaopen_gnss},         /* GNSS定位 */

    {NULL, NULL}
};

LUALIB_API int luaopen_platform_modules(lua_State* L) {
    luaL_newlib(L, platform_modules);
    return 1;
}