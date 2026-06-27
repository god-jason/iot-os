-- platforms xmake 配置

-- 获取当前平台配置
local plat = get_config("platform") or "windows"

-- 判断是否为桌面平台（需要编译成可执行文件）
local is_desktop_platform = plat == "windows" or plat == "linux" or plat == "wasm"

-- 根据 platform 参数创建目标
target("iot")
if is_desktop_platform then
    set_kind("binary")
    -- 使用相对于项目根目录的路径
    set_targetdir("$(projectdir)/build/" .. plat .. "/x64/$(mode)")
    -- 运行时工作目录设为项目根目录
    set_rundir("$(projectdir)")
else
    set_kind("static")
end
add_files(plat .. "/*.c")
add_files(plat .. "/modules/*.c")
add_headerfiles(plat .. "/*.h")
add_headerfiles(plat .. "/modules/*.h")
add_includedirs(plat)
add_includedirs("../vendor/lua", "../core")
add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")

-- WASM 平台特殊配置
if plat == "wasm" then
    set_kind("shared")
    add_rules("wasm")
    add_ldflags("-s USE_SDL=2", "-s USE_LVGL=1", "-s ALLOW_MEMORY_GROWTH=1")
    add_ldflags("-s EXPORTED_FUNCTIONS=['_main','_iot_wasm_stop']", "-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']")
    add_deps("lua", "cjson", "lua-cjson", "miniz", "iot_zlib", "iot_fs", "iot_lvgl", "iot_fonts")
    add_deps("iot_core")
    set_default(true)
    return
end

-- 桌面平台添加依赖和链接库
if is_desktop_platform then
    add_deps("lua", "gmssl", "libjpeg-turbo", "miniz", "iot_zlib", "cjson", "sqlite3", "lua-cjson")
    add_deps("iot_crypto", "iot_fs", "iot_http", "iot_mqtt", "iot_modbus", "iot_net", "iot_lvgl", "iot_fonts")
    add_deps("iot_core", "drivers")

    if plat == "windows" then
        add_links("ws2_32", "winmm", "bcrypt")
    end

    set_policy("check.auto_ignore_flags", false)
    add_ldflags("-Wl,--start-group")
    add_ldflags("-llua", "-lgmssl", "-llibjpeg-turbo", "-llua-cjson",
        "-lminiz", "-liot_zlib", "-lcjson", "-lsqlite3", "-liot_crypto",
        "-liot_fs", "-liot_http", "-liot_mqtt", "-liot_modbus", "-liot_net", "-liot_lvgl", "-llvgl",
        "-liot_fonts", "-ldrivers", "-liot_core")
    if plat == "windows" then
        add_ldflags("-lws2_32", "-lwinmm", "-lbcrypt")
    end
    add_ldflags("-Wl,--end-group")

    set_default(true)
end
