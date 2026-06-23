-- net 模块 xmake 配置

target("iot_net")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..")
    add_includedirs("../../vendor/gmssl/include")
    add_deps("iot_core")

    -- Windows 平台链接 Winsock 库，所有平台都依赖 gmssl（TLS/TLCP 功能）
    if is_plat("windows") then
        add_links("ws2_32", "winmm")
    end
    add_deps("gmssl")

    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")