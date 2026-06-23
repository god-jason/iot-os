-- net 模块 xmake 配置

target("iot_net")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..")
    add_includedirs("../../vendor/lwip")
    add_includedirs("../../vendor/lwip/src/include")
    add_includedirs("../../vendor/gmssl/include")
    add_deps("iot")
    add_deps("gmssl")

    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")