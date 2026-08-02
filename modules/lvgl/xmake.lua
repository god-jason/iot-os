-- lvgl 模块 xmake 配置

target("iot_lvgl")
    set_kind("static")
    add_files("*.c|lv_sdl_drv.c|lvgl_meter.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..", "../../vendor/lvgl-9.5.0", "../../vendor/lvgl-9.5.0/src")
    add_deps("iot_core", "lvgl")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")

    -- 使用 LVGL 9 内置 SDL 驱动，不再需要自定义 lv_sdl_drv.c
    -- lv_conf.h 中已配置 LV_USE_SDL=1
