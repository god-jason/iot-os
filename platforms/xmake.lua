-- platforms xmake 配置

-- 获取当前平台配置
local current_platform = get_config("platform") or "windows"

-- 平台配置表
local platform_configs = {
    windows = {
        name = "platform_windows",
        dir = "windows"
    },
    linux = {
        name = "platform_linux",
        dir = "linux"
    },
    esp32 = {
        name = "platform_esp32",
        dir = "esp32"
    },
    ml307n = {
        name = "platform_ml307n",
        dir = "ml307n"
    },
    yopen = {
        name = "platform_yopen",
        dir = "yopen"
    }
}

-- 根据 platform 参数创建目标
local cfg = platform_configs[current_platform]
if cfg then
    -- 创建平台目标
    target(cfg.name)
        set_kind("static")
        add_files(cfg.dir .. "/modules/*.c")
        add_headerfiles(cfg.dir .. "/*.h")
        add_includedirs(cfg.dir)
        add_deps("iot_core")
        add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
    
    -- 将平台头文件目录添加到全局
    add_includedirs(cfg.dir)
end