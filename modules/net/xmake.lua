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

    -- Windows 平台链接 Winsock 库，不依赖 gmssl（TLS 功能暂不可用）
    if is_plat("windows") then
        add_links("ws2_32", "winmm")
    else
        add_deps("gmssl")
    end

    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")