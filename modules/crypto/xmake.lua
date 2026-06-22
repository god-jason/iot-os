-- crypto 模块 xmake 配置

target("iot_crypto")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..", "../../vendor/gmssl/include")
    add_deps("iot", "gmssl")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")