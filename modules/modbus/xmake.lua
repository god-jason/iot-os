-- modbus 模块 xmake 配置

target("iot_modbus")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..")
    add_deps("iot_core")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
