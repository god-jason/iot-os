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

-- 公共头文件目录
add_includedirs("platforms/"..(get_config("platform") or "windows"))
add_includedirs("platforms")
add_includedirs("vendor/lua")
add_includedirs("modules/zlib")
add_includedirs("iot")
add_includedirs("script")

--===========================================================
-- 包含子目录
--===========================================================
includes("iot", "modules", "drivers", "platforms", "vendor")

--===========================================================
-- 默认构建目标（依赖所有子目标）
--===========================================================
target("all")
    set_kind("phony")
    add_deps("iot", "platform", "drivers")
    add_deps("iot_crypto", "iot_fs", "iot_http", "iot_mqtt", "iot_net", "iot_zlib", "iot_lvgl", "iot_fonts")
    add_deps("lua", "cjson", "lua-cjson", "gmssl", "libjpeg-turbo")
    set_default(true)
