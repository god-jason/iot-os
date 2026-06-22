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
-- 包含子目录
--===========================================================
includes("iot", "modules", "drivers", "platforms", "vendor")

--===========================================================
-- 平台配置
--===========================================================
option("platform")
    set_default("windows")
    set_showmenu(true)
    set_description("选择目标平台")
    set_values("linux", "windows", "esp32", "ml307n", "yopen")
option_end()


-- 公共头文件目录
add_includedirs("platforms/"..(get_config("platform") or "windows"))
add_includedirs("vendor/lua")
add_includedirs("modules/zlib")
add_includedirs("iot")
add_includedirs("script")

-- 添加平台相关配置
if is_config("platform", "linux") then
    add_defines("IOT_PLATFORM_LINUX")
elseif is_config("platform", "windows") then
    add_defines("IOT_PLATFORM_WINDOWS")
elseif is_config("platform", "esp32") then
    add_defines("IOT_PLATFORM_ESP32")
elseif is_config("platform", "ml307n") then
    add_defines("IOT_PLATFORM_ML307N")
elseif is_config("platform", "yopen") then
    add_defines("IOT_PLATFORM_YOPEN")
end

if is_config("freertos") then
    add_defines("IOT_FREERTOS")
end

--===========================================================
-- 默认目标
--===========================================================
set_default("iot", "iot_crypto", "iot_fs", "iot_http", "iot_mqtt", "iot_net", "iot_zlib", "iot_lvgl", "drivers", "cjson", "lua", "lua-cjson")
