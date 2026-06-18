-----------------------------------------------------------------------
-- Xmake script file
-- Copyright (C) 2024, Xinsheng Tech.
-- All rights reserved.
--
-- @author      weirunrun
-- @file        xmake.lua
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Configuration
-----------------------------------------------------------------------
local TARGET_NAME = "oc"

-----------------------------------------------------------------------
-- Include subdirs
-----------------------------------------------------------------------
if get_config("cpu_type") == "cpu-ap" then
    -- Below subsystem source files are open to OC customers
    includes("shell_test")
    if has_config("CM_SERVICE_AUDIO_ENABLE") then
        includes("cmo_audio_player")
        includes("cmo_audio_recorder")
        includes("cmo_codec_driver")
        includes("cmo_audio_decoders")
    end
    if has_config("CM_SERVICE_LCD_ENABLE") then
        includes("cmo_lcd")
    end
    includes("cmo_spi")
    -- End of open sourced subsystems
end
-----------------------------------------------------------------------
-- Export global macro
-----------------------------------------------------------------------

-----------------------------------------------------------------------
-- Export global include directory
-----------------------------------------------------------------------
    add_global_includedirs("include")
    add_global_includedirs("shell_test/inc")

    add_includedirs("../net/lwip/lwip-2.1.2/src/include/openapi")
    add_includedirs("../net/lwip/lwip-2.1.2/src/include/")
    add_includedirs("../include/mdl/modem")
    add_includedirs("../include/mdl/service")
    add_includedirs("../driver/include")
    add_includedirs("../kernel/include")

    --lwm2m
    add_global_includedirs("cmo_lwm2m/inc")

    -----------------------------------------------------------------------
    -- Private function
    -----------------------------------------------------------------------
function add_core_files()
    add_srcfile("cmo_mem/src/cmo_mem.c")
    add_srcfile("cmo_adc/src/cmo_adc.c")
    add_srcfile("cmo_keypad/src/cmo_keypad.c")
    add_srcfile("cmo_gpio/src/cmo_gpio.c")
    add_srcfile("cmo_gpio/src/cmo_gpio_v2.c")
    add_srcfile("cmo_iomux/src/cmo_iomux.c")
    add_srcfile("cmo_iomux/src/cmo_iomux_v2.c")
    add_srcfile("cmo_uart/src/cmo_uart.c")
    add_srcfile("cmo_i2c/src/cmo_i2c.c")
    add_srcfile("cmo_pwm/src/cmo_pwm.c")
    add_srcfile("cmo_fs/src/cmo_fs.c")
    add_srcfile("cmo_rtc/src/cmo_rtc.c")
    add_srcfile("cmo_sys/src/cmo_sys.c")
    add_srcfile("cmo_virt_at/src/cmo_virt_at.c")
    add_srcfile("cmo_ntp/src/cmo_ntp.c")
    add_srcfile("cmo_pm/src/cmo_pm.c")
    add_srcfile("cmo_wifiscan/src/cmo_wifiscan.c")
    add_srcfile("cmo_modem/src/cmo_modem_info.c")
    add_srcfile("cmo_sim/src/cmo_sim.c")
    add_srcfile("cmo_sms/src/cmo_sms.c")
    add_srcfile("cmo_dm/src/cmo_dm.c")
end

function add_ota_files()
    add_srcfile("cmo_ota/src/cmo_ota.c")
    add_srcfile("cmo_ota/src/cmo_fota.c")
end

function add_lwm2m_files()
    add_srcfile("cmo_lwm2m/src/cmo_lwm2m.c")
end

function add_ftp_files()
    add_srcfile("cmo_ftp/src/cmo_ftp.c")
end

function add_ping_files()
    add_srcfile("cmo_ping/src/cmo_ping.c")
end

function add_gnss_files()
    add_includedirs("../onemo/cm_gnss/cm_gnss_core")
    add_includedirs("../onemo/cm_gnss/cm_gnss_manager")
    add_includedirs("../onemo/cm_gnss/cm_gnss_nmea")
    add_includedirs("../onemo/cm_gnss/cm_gnss_drives")
    add_includedirs("../onemo/cm_gnss/cm_gnss_platform/inc")
    add_srcfile("cmo_gnss/src/cmo_gnss.c")
end

function add_tts_files()
    add_includedirs("../onemo/cm_tts/tts")
    add_srcfile("cmo_tts/src/cmo_local_tts.c")
end

function add_camera_files()
    add_includedirs("cmo_camera/src")
    --add_srcdirall("cmo_camera/src")
    add_srcfile("cmo_camera/src/cmo_camera.c")
    add_srcfile("cmo_camera/src/cmo_camera_sensor.c")
end

-----------------------------------------------------------------------
-- Target
-----------------------------------------------------------------------
-- SDK mode uses prebuilt liboc_frozen.a; do not compile duplicate "oc" here
if os.getenv("USE_SDK") ~= "y" then
target(TARGET_NAME)
-- [donot edit] ---
    set_kind("static")
    add_deps("base")
    add_global_deps(TARGET_NAME)
-- [   end    ] ---
    add_includedirs("cmo_mem")
    add_includedirs("cmo_fs")
    add_includedirs("cmo_rtc")
    add_includedirs("cmo_sys")
    add_includedirs("cmo_virt_at")
    add_includedirs("cmo_adc")
    add_includedirs("cmo_keypad")
    add_includedirs("cmo_gpio")
    add_includedirs("cmo_iomux")
    add_includedirs("cmo_uart")
    add_includedirs("cmo_i2c")
    add_includedirs("cmo_pwm")
    add_includedirs("cmo_audio_player")
    add_includedirs("cmo_ntp")
    add_includedirs("cmo_wifiscan")
    add_includedirs("cmo_pm")
    add_includedirs("cmo_modem")
    add_includedirs("cmo_sim")
    add_includedirs("shell_test")
    add_includedirs("cmo_ota")
    add_includedirs("cmo_sms")
    add_includedirs("cmo_dm")

    add_core_files()
    add_lwm2m_files()
    add_ftp_files()
    add_ota_files()
    add_ping_files()

    if has_config("ml307n_gc_config") then
        add_gnss_files()
    end

    if not has_config("volte") and has_config("CM_SERVICE_TTS_ENABLE") then
        add_tts_files()
    end

    if has_config("CM_SERVICE_CAMERA_ENABLE") then
    add_camera_files()
    end
target_end()
end
