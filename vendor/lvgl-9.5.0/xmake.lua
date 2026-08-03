-- lvgl 9.5.0 xmake 配置

target("lvgl")
    set_kind("static")

    -- 核心源文件
    add_files("src/lv_init.c")
    add_files("src/core/*.c")
    add_files("src/display/*.c")
    add_files("src/indev/*.c")
    add_files("src/tick/*.c")
    add_files("src/osal/*.c")

    -- stdlib (使用 CLIB 实现：标准C库包装)
    add_files("src/stdlib/*.c")
    add_files("src/stdlib/clib/*.c")

    -- misc
    add_files("src/misc/*.c")
    add_files("src/misc/cache/*.c")
    add_files("src/misc/cache/class/*.c")
    add_files("src/misc/cache/instance/*.c")

    -- draw (只编译软件渲染器)
    add_files("src/draw/*.c")
    add_files("src/draw/sw/*.c")
    add_files("src/draw/sw/blend/*.c")
    add_files("src/draw/snapshot/*.c")
    add_files("src/draw/convert/*.c")

    -- font
    add_files("src/font/*.c")
    add_files("src/font/fmt_txt/*.c")

    -- layouts
    add_files("src/layouts/*.c")
    add_files("src/layouts/flex/*.c")
    add_files("src/layouts/grid/*.c")

    -- themes (含 default/mono/simple 子目录)
    add_files("src/themes/*.c")
    add_files("src/themes/default/*.c")
    add_files("src/themes/mono/*.c")
    add_files("src/themes/simple/*.c")

    -- others
    add_files("src/others/fragment/*.c")
    add_files("src/others/file_explorer/*.c")
    add_files("src/others/translation/*.c")

    -- widgets
    add_files("src/widgets/animimage/*.c")
    add_files("src/widgets/arclabel/*.c")
    add_files("src/widgets/arc/*.c")
    add_files("src/widgets/bar/*.c")
    add_files("src/widgets/button/*.c")
    add_files("src/widgets/buttonmatrix/*.c")
    add_files("src/widgets/calendar/*.c")
    add_files("src/widgets/canvas/*.c")
    add_files("src/widgets/chart/*.c")
    add_files("src/widgets/checkbox/*.c")
    add_files("src/widgets/dropdown/*.c")
    add_files("src/widgets/gif/*.c")
    add_files("src/widgets/image/*.c")
    add_files("src/widgets/imagebutton/*.c")
    add_files("src/widgets/ime/*.c")
    add_files("src/widgets/keyboard/*.c")
    add_files("src/widgets/label/*.c")
    add_files("src/widgets/led/*.c")
    add_files("src/widgets/line/*.c")
    add_files("src/widgets/list/*.c")
    add_files("src/widgets/menu/*.c")
    add_files("src/widgets/msgbox/*.c")
    add_files("src/widgets/roller/*.c")
    add_files("src/widgets/scale/*.c")
    add_files("src/widgets/slider/*.c")
    add_files("src/widgets/span/*.c")
    add_files("src/widgets/spinbox/*.c")
    add_files("src/widgets/spinner/*.c")
    add_files("src/widgets/switch/*.c")
    add_files("src/widgets/table/*.c")
    add_files("src/widgets/tabview/*.c")
    add_files("src/widgets/textarea/*.c")
    add_files("src/widgets/tileview/*.c")
    add_files("src/widgets/win/*.c")

    -- libs (文件系统驱动和图片解码)
    add_files("src/libs/fsdrv/*.c")
    add_files("src/libs/bin_decoder/*.c")
    -- tiny_ttf 内置矢量字体（无需外部依赖）
    add_files("src/libs/tiny_ttf/*.c")
    -- GIF 解码器
    add_files("src/libs/gif/*.c")

    -- SDL 驱动 (桌面平台)
    if is_plat("windows", "linux", "macosx") then
        add_files("src/drivers/sdl/*.c")
    end

    -- 头文件目录 (包含根目录以便找到 lv_conf.h)
    add_includedirs(".", "src")

    -- 编译选项
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")

    -- 使用 lv_conf.h 配置文件
    add_defines("LV_CONF_INCLUDE_SIMPLE")

    -- 桌面平台引入 SDL2 (手动指定路径)
    if is_plat("windows") then
        add_includedirs("../libsdl/SDL2-2.32.10/x86_64-w64-mingw32/include/SDL2")
    end
