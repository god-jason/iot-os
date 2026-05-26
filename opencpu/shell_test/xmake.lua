-----------------------------------------------------------------------
-- Xmake script file
-- Copyright (C) 2024, Xinsheng Tech.
-- All rights reserved.
--
-- @author      wyl
-- @file        xmake.lua
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Configuration
-----------------------------------------------------------------------
local TARGET_NAME = "ocshelltest"

-----------------------------------------------------------------------
-- Export global macro
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Export global include directory
-----------------------------------------------------------------------
    add_global_includedirs("inc")
-----------------------------------------------------------------------
-- Target
-----------------------------------------------------------------------
target(TARGET_NAME)
-- [don't edit] ---
    set_kind("static")
    add_deps("base")
    add_global_deps(TARGET_NAME)
-- [   end    ] ---
if os.getenv("CLOSE_TEST") ~= "y" then

    add_defines("CM_DEMO_MEM_SUPPORT")
    add_defines("CM_DEMO_FS_SUPPORT")
    add_defines("CM_DEMO_RTC_SUPPORT")
    add_defines("CM_DEMO_SYS_SUPPORT")
    add_defines("CM_DEMO_VIRT_AT_SUPPORT")
    add_defines("CM_DEMO_ADC_SUPPORT")
    add_defines("CM_DEMO_GPIO_SUPPORT")
    add_defines("CM_DEMO_UART_SUPPORT")
    add_defines("CM_DEMO_I2C_SUPPORT")
    add_defines("CM_DEMO_SPI_SUPPORT")
    add_defines("CM_DEMO_PWM_SUPPORT")
    add_defines("CM_DEMO_AUDIO_SUPPORT")
    add_defines("CM_DEMO_NTP_SUPPORT")
    add_defines("CM_DEMO_WIFISCAN_SUPPORT")
    add_defines("CM_DEMO_PM_SUPPORT")
    add_defines("CM_DEMO_FOTA_SUPPORT")
    add_defines("CM_DEMO_LBS_SUPPORT")
    add_defines("CM_DEMO_HTTP_SUPPORT")
    add_defines("CM_DEMO_ASOCKET_SUPPORT")
    add_defines("CM_DEMO_MODEM_SUPPORT")
    add_defines("CM_DEMO_PING_SUPPORT")
    add_defines("CM_DEMO_LWM2M_SUPPORT")
    add_defines("CM_DEMO_FTP_SUPPORT")
    add_defines("CM_DEMO_SMS_SUPPORT")
    add_defines("CM_DEMO_MQTT_SUPPORT")
    add_defines("CM_DEMO_SSL_SUPPORT")
    add_defines("CM_DEMO_LCD_SUPPORT")
    add_defines("CM_DEMO_KEYPAD_SUPPORT")
    add_defines("CM_DEMO_DMP_SUPPORT")
    add_defines("CM_DEMO_ATLBS_SUPPORT")
    add_defines("CM_DEMO_DNS_SUPPORT")

    if has_config("ml307n_gc_config") then
        add_defines("CM_DEMO_GNSS_SUPPORT")
    end

    add_includedirs("inc")

    add_srcfile("src/cmo_demo_fs.c")
    add_srcfile("src/cmo_demo_sys.c")
    add_srcfile("src/cmo_demo_mem.c")
    add_srcfile("src/cmo_demo_rtc.c")
    add_srcfile("src/cmo_demo_virt_at.c")
    add_srcfile("src/cmo_demo_adc.c")
    add_srcfile("src/cmo_demo_gpio.c")
    add_srcfile("src/cmo_demo_uart.c")
    add_srcfile("src/cmo_demo_i2c.c")
    add_srcfile("src/cmo_demo_pwm.c")
    if has_config("CM_SERVICE_AUDIO_ENABLE") then
        add_srcfile("src/cmo_demo_audio.c")
    end
    add_srcfile("src/cmo_demo_spi.c")
    add_srcfile("src/cmo_demo_ntp.c")
    add_srcfile("src/cmo_demo_wifiscan.c")
    add_srcfile("src/cmo_demo_pm.c")
    add_srcfile("src/cmo_demo_fota.c")
    if has_config("CM_SERVICE_LBS_ENABLE") then
    add_srcfile("src/cmo_demo_lbs.c")
    end
    add_srcfile("src/cmo_demo_http.c")
    add_srcfile("src/cmo_demo_asocket.c")
    if has_config("CM_SERVICE_LWM2M_ENABLE") then
    add_srcfile("src/cmo_demo_lwm2m.c")
    end
    if has_config("CM_SERVICE_FTP_ENABLE") then
    add_srcfile("src/cmo_demo_ftp.c")
    end
    add_srcfile("src/cmo_demo_ping.c")
    add_srcfile("src/cmo_demo_sms.c")
    if has_config("CM_SERVICE_MQTT_ENABLE") then
        add_srcfile("src/cmo_demo_mqtt.c")
    end
    add_srcfile("src/cmo_demo_ssl.c")
    add_srcfile("src/cmo_demo_lcd.c")
    add_srcfile("src/cmo_demo_keypad.c")

    if not has_config("volte") and has_config("CM_SERVICE_AUDIO_ENABLE") then
    if has_config("CM_SERVICE_TTS_ENABLE") then
        add_srcfile("src/cmo_demo_tts.c")
    end
    end

    -- AT TEST
    if has_config("CM_SERVICE_FTP_ENABLE") then
    add_srcfile("src/at_test_ftp.c")
    end
    if has_config("CM_SERVICE_MQTT_ENABLE") then
        add_srcfile("src/at_test_mqtt.c")
    end
    if has_config("CM_SERVICE_LWM2M_ENABLE") then
    add_srcfile("src/at_test_lwm2m.c")
    end
    add_srcfile("src/at_test_ping.c")
    if has_config("CM_SERVICE_DM_ENABLE") then
        add_srcfile("src/at_test_dmp.c")
    end
    if has_config("CM_SERVICE_LBS_ENABLE") then
    add_srcfile("src/at_test_lbs.c")
    end
    add_srcfile("src/at_test_tcpip.c")
    add_srcfile("src/at_test_fota.c")
    add_srcfile("src/at_test_dns.c")
    add_srcfile("src/at_test_pm.c")
    add_srcfile("src/at_test_virt_at.c")
    add_srcfile("src/at_test_ssl.c")
    add_srcfile("src/at_test_http.c")
    if has_config("ml307n_gc_config") then
        add_srcfile("src/cmo_demo_gnss.c")
    end

end
target_end()
