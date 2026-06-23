-- iot-os xmake 构建脚本
-- 物联网操作系统（基于FreeRTOS和4G模组）

set_project("iot-os")
set_version("1.0.0")

-- 设置 C 语言标准
set_languages("c99")

-- 添加编译规则
add_rules("mode.debug", "mode.release")

-- 默认使用 gcc 工具链
set_toolchains("gcc")

--===========================================================
-- 平台配置
--===========================================================
option("platform")
    set_default("windows")
    set_showmenu(true)
    set_description("选择目标平台")
    set_values("linux", "windows", "esp32", "ml307n", "yopen")
option_end()

--===========================================================
-- 模块配置选项
--===========================================================

-- Modules 模块
option("module_crypto")
    set_default(true)
    set_showmenu(true)
    set_description("Enable crypto module")
option_end()

option("module_fs")
    set_default(true)
    set_showmenu(true)
    set_description("Enable fs module")
option_end()

option("module_http")
    set_default(true)
    set_showmenu(true)
    set_description("Enable http module")
option_end()

option("module_mqtt")
    set_default(true)
    set_showmenu(true)
    set_description("Enable mqtt module")
option_end()

option("module_net")
    set_default(true)
    set_showmenu(true)
    set_description("Enable net module")
option_end()

option("module_zlib")
    set_default(true)
    set_showmenu(true)
    set_description("Enable zlib module")
option_end()

option("module_wdt")
    set_default(true)
    set_showmenu(true)
    set_description("Enable wdt module")
option_end()

-- Vendor 模块
option("vendor_cjson")
    set_default(false)
    set_showmenu(true)
    set_description("Enable cjson vendor module")
option_end()

option("vendor_gmssl")
    set_default(true)
    set_showmenu(true)
    set_description("Enable gmssl vendor module")
option_end()

option("vendor_sqlite3")
    set_default(false)
    set_showmenu(true)
    set_description("Enable sqlite3 vendor module")
option_end()

-- 公共头文件目录
add_includedirs("platforms/"..(get_config("platform") or "windows"))
add_includedirs("platforms")
add_includedirs("vendor/lua")
add_includedirs("modules/zlib")
add_includedirs("core")
add_includedirs("script")

--===========================================================
-- 包含子目录
--===========================================================
includes("core", "modules", "drivers", "platforms", "vendor")

--===========================================================
-- 默认构建目标（非桌面平台使用）
--===========================================================
local plat = get_config("platform") or "windows"
if plat ~= "windows" and plat ~= "linux" then
    target("all")
        set_kind("phony")
        add_deps("iot_core", "drivers", "iot")
        add_deps("iot_crypto", "iot_fs", "iot_http", "iot_mqtt", "iot_net", "iot_zlib", "iot_lvgl", "iot_fonts")
        add_deps("lua", "cjson", "lua-cjson", "gmssl", "libjpeg-turbo")
        set_default(true)
end
