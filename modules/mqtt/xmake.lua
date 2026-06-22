-- mqtt 模块 xmake 配置

target("iot_mqtt")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..", "../net")
    add_deps("iot", "iot_net")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")