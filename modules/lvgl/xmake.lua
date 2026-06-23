-- lvgl 模块 xmake 配置

target("iot_lvgl")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..", "../../vendor/lvgl")
    add_deps("iot_core")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")