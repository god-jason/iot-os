-- lvgl xmake 配置

target("lvgl")
    set_kind("static")
    
    -- 添加所有源文件
    add_files("src/core/*.c")
    add_files("src/draw/*.c")
    add_files("src/draw/sw/*.c")
    add_files("src/extra/*.c")
    add_files("src/extra/layouts/flex/*.c")
    add_files("src/extra/layouts/grid/*.c")
    add_files("src/extra/themes/basic/*.c")
    add_files("src/extra/themes/default/*.c")
    add_files("src/extra/themes/mono/*.c")
    add_files("src/extra/widgets/analogclock/*.c")
    add_files("src/extra/widgets/animimg/*.c")
    add_files("src/extra/widgets/calendar/*.c")
    add_files("src/extra/widgets/carousel/*.c")
    add_files("src/extra/widgets/chart/*.c")
    add_files("src/extra/widgets/colorwheel/*.c")
    add_files("src/extra/widgets/dclock/*.c")
    add_files("src/extra/widgets/imgbtn/*.c")
    add_files("src/extra/widgets/keyboard/*.c")
    add_files("src/extra/widgets/led/*.c")
    add_files("src/extra/widgets/list/*.c")
    add_files("src/extra/widgets/menu/*.c")
    add_files("src/extra/widgets/meter/*.c")
    add_files("src/extra/widgets/msgbox/*.c")
    add_files("src/extra/widgets/radiobtn/*.c")
    add_files("src/extra/widgets/span/*.c")
    add_files("src/extra/widgets/spinbox/*.c")
    add_files("src/extra/widgets/spinner/*.c")
    add_files("src/extra/widgets/tabview/*.c")
    add_files("src/extra/widgets/textprogress/*.c")
    add_files("src/extra/widgets/tileview/*.c")
    -- add_files("src/extra/widgets/video/*.c")  -- 需要FreeRTOS，暂不编译
    add_files("src/extra/widgets/win/*.c")
    add_files("src/font/*.c")
    add_files("src/hal/*.c")
    add_files("src/misc/*.c")
    add_files("src/widgets/*.c")
    
    -- 添加头文件
    --add_headerdirs("src")
    add_includedirs("src")
    
    -- 编译选项
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
    
    -- 定义 LVGL 配置
    add_defines("LV_CONF_INCLUDE_SIMPLE")