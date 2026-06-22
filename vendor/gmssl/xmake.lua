-- gmssl xmake 配置

target("gmssl")
    set_kind("static")

    -- 添加源文件（排除 tools 目录和 ARM 特定文件）
    add_files("src/*.c|tools/**|gf128_arm64.c|sm2_z256_arm64.c|sm3_arm64.c|sm4_arm64.c|sm4_ce.c|sm9_z256_arm64.c|rand_apple.c|rand_unix.c")
    add_files("src/sdf/*.c")
    add_files("src/skf/*.c")
    add_files("gmssl_module.c")

    -- 添加头文件
    add_headerfiles("include/gmssl/*.h")
    add_headerfiles("src/sdf/*.h")
    add_headerfiles("src/skf/*.h")

    -- 包含目录
    add_includedirs("include", "src", "../lua", "../../platforms/"..(get_config("platform") or "windows"))

    -- 依赖
    add_deps("lua", "platform")

    -- 编译选项
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")