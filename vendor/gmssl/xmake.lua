-- gmssl xmake 配置

target("gmssl")
    set_kind("static")
    
    -- 添加源文件
    add_files("src/*.c")
    add_files("src/sdf/*.c")
    add_files("src/skf/*.c")
    
    -- 添加头文件
    add_headerfiles("include/gmssl/*.h")
    add_headerfiles("src/sdf/*.h")
    add_headerfiles("src/skf/*.h")
    
    -- 包含目录
    add_includedirs("include", "src")
    
    -- 编译选项
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
    
    -- 排除工具文件（tools 目录下的是命令行工具）
    add_files("tools/*.c", {remove = true})