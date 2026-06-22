-- lua-cjson xmake 配置

target("lua-cjson")
    set_kind("static")
    
    -- 添加源文件
    add_files("lua_cjson.c", "dtoa.c", "g_fmt.c", "strbuf.c", "fpconv.c")
    
    -- 添加头文件
    add_headerfiles("fpconv.h", "strbuf.h", "dtoa_config.h")
    
    -- 包含目录
    add_includedirs(".")
    
    -- 编译选项
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")