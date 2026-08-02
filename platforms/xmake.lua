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
add_includedirs("../vendor/lua", "../core", "../modules")
add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")

-- WASM 平台特殊配置
if plat == "wasm" then
    set_kind("shared")
    add_rules("wasm")
    add_ldflags("-s USE_SDL=2", "-s USE_LVGL=1", "-s ALLOW_MEMORY_GROWTH=1")
    add_ldflags("-s EXPORTED_FUNCTIONS=['_main','_iot_wasm_stop']")
    add_ldflags("-s EXPORTED_RUNTIME_METHODS=['ccall','cwrap']")
    -- 启用 fetch API
    add_ldflags("-s FETCH=1")
    -- 启用 WebSocket
    add_ldflags("-s WEBSOCKET_SUBPROTOCOL='binary'")
    -- 启用文件系统
    add_ldflags("-s FORCE_FILESYSTEM=1")
    add_deps("lua", "lua-cjson", "miniz", "iot_zlib", "iot_fs", "iot_lvgl", "iot_filters", "iot_pack", "iot_wdt")
    add_deps("iot_core")
    set_default(true)
    return
end

-- 桌面平台添加依赖和链接库
if is_desktop_platform then
    add_deps("lua", "gmssl", "miniz", "iot_zlib", "lua-cjson")
    add_deps("iot_crypto", "iot_fs", "iot_http", "iot_mqtt", "iot_modbus", "iot_net", "iot_lvgl", "iot_filters", "iot_pack", "iot_wdt")
    add_deps("iot_core", "drivers", "ai")

    -- 桌面平台引入 SDL2（供 LVGL 窗口驱动使用）
    if plat ~= "wasm" then
        if plat == "windows" then
            add_includedirs("../vendor/libsdl/SDL2-2.32.10/x86_64-w64-mingw32/include/SDL2")
            add_linkdirs("../vendor/libsdl/SDL2-2.32.10/x86_64-w64-mingw32/lib")
            -- 静态链接 SDL2，禁用 main 宏重定向
            add_defines("SDL_MAIN_HANDLED")
        end
    end

    if plat == "windows" then
        -- 静态链接 SDL2，禁用 main 宏重定向
        add_defines("SDL_MAIN_HANDLED")
    end

    set_policy("check.auto_ignore_flags", false)
    add_ldflags("-Wl,--start-group")
    add_ldflags("-llua", "-lgmssl", "-llua-cjson",
        "-lminiz", "-liot_zlib", "-liot_crypto",
        "-liot_fs", "-liot_http", "-liot_mqtt", "-liot_modbus", "-liot_net", "-liot_lvgl", "-llvgl",
        "-liot_filters", "-liot_pack", "-liot_wdt", "-ldrivers", "-liot_core", "-lai")
    if plat == "windows" then
        -- SDL2 静态链接，系统库必须在 SDL2 之后
        add_ldflags("-Wl,-Bstatic", "-lSDL2", "-lSDL2main", "-Wl,-Bdynamic")
        add_ldflags("-lws2_32", "-lwinmm", "-lgdi32", "-limm32", "-lole32", "-loleaut32",
            "-lshell32", "-lsetupapi", "-lversion", "-ladvapi32", "-luser32",
            "-luuid", "-ldinput8", "-ldxguid", "-lbcrypt", "-lmingw32")
    end
    add_ldflags("-Wl,--end-group")

    set_default(true)
end