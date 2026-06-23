-- iot 核心库 xmake 配置

-- Modules 模块
if has_config("module_crypto") then
    add_defines("IOT_ENABLE_MODULE_CRYPTO")
    set_config("vendor_gmssl", true)
end

if has_config("module_fs") then
    add_defines("IOT_ENABLE_MODULE_FS")
end

if has_config("module_http") then
    add_defines("IOT_ENABLE_MODULE_HTTP")
end

if has_config("module_mqtt") then
    add_defines("IOT_ENABLE_MODULE_MQTT")
end

if has_config("module_net") then
    add_defines("IOT_ENABLE_MODULE_NET")
end

if has_config("module_zlib") then
    add_defines("IOT_ENABLE_MODULE_ZLIB")
end

if has_config("module_wdt") then
    add_defines("IOT_ENABLE_LUA_WDT")
end

-- Vendor 模块
if has_config("vendor_cjson") then
    add_defines("IOT_ENABLE_VENDOR_CJSON")
end

if has_config("vendor_gmssl") then
    add_defines("IOT_ENABLE_VENDOR_GMSSL")
end

if has_config("vendor_sqlite3") then
    add_defines("IOT_ENABLE_VENDOR_SQLITE3")
end

target("iot_core")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".")
    add_deps("iot_zlib")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
