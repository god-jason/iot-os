-- gmssl xmake 配置

target("gmssl")
    set_kind("static")

    -- 添加源文件（排除 tools 目录）
    add_files("src/*.c", {exclude = "tools/*.c"})
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