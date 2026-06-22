-- net 模块 xmake 配置

target("iot_net")
    set_kind("static")
    add_files("*.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..")
    add_deps("iot_core")

    -- 平台相关定义
    if is_config("freertos") then
        add_defines("IOT_NET_FREERTOS=1")
    elseif is_config("platform", "linux") then
        add_defines("IOT_NET_LINUX=1")
    end

    -- 链接第三方库（需要根据实际情况配置）
    -- add_syslinks("lwip")
    -- add_deps("gmssl")

    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")