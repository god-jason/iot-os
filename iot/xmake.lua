-- iot 核心库 xmake 配置

target("iot_core")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".")
    add_defines("IOT_CORE")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")