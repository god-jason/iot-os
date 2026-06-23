-- iot 核心库 xmake 配置

target("iot")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".")
    add_deps("iot_zlib")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")