-- iot 核心库 xmake 配置

-- Modules 模块
if is_config("module_crypto", true) then
    add_defines("IOT_ENABLE_MODULE_CRYPTO")
    set_config("vendor_gmssl", true)
end

if is_config("module_fs", true) then
    add_defines("IOT_ENABLE_MODULE_FS")
end

if is_config("module_http", true) then
    add_defines("IOT_ENABLE_MODULE_HTTP")
    set_config("module_net", true)
end

if is_config("module_mqtt", true) then
    add_defines("IOT_ENABLE_MODULE_MQTT")
    set_config("module_net", true)
end

if is_config("module_net", true) then
    add_defines("IOT_ENABLE_MODULE_NET")
end

if is_config("module_zlib", true) then
    add_defines("IOT_ENABLE_MODULE_ZLIB")
end

-- Vendor 模块
if is_config("vendor_cjson", false) then
    add_defines("IOT_ENABLE_VENDOR_CJSON")
end

if is_config("vendor_gmssl", true) then
    add_defines("IOT_ENABLE_VENDOR_GMSSL")
end

if is_config("vendor_sqlite3", false) then
    add_defines("IOT_ENABLE_VENDOR_SQLITE3")
end

target("iot_core")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".")
    add_deps("iot_zlib")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
