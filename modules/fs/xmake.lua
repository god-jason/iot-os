-- fs 模块 xmake 配置

target("iot_fs")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..")
    add_deps("iot")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")