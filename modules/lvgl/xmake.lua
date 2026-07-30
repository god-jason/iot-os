-- lvgl 模块 xmake 配置

target("iot_lvgl")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..", "../../vendor/lvgl")
    add_deps("iot_core", "lvgl")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")

    -- 桌面平台启用 SDL 窗口驱动
    if is_plat("windows", "linux", "macosx") then
        add_files("lv_sdl_drv.c")
        add_defines("LV_USE_SDL_DRV")
        -- 使用预编译 SDL2
        if is_plat("windows") then
            add_includedirs("../../vendor/libsdl/SDL2-2.32.10/x86_64-w64-mingw32/include/SDL2")
        end
    end
