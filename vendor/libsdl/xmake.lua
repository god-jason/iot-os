-- libsdl 预编译库配置（MinGW x86_64）
-- 提供 SDL2 头文件和库路径给其他目标使用

-- SDL2 预编译库根路径
local sdl2_root = path.join(os.scriptdir(), "SDL2-2.32.10", "x86_64-w64-mingw32")

target("libsdl")
    set_kind("phony")
    -- 仅暴露头文件和库路径，不编译任何文件
    add_includedirs(path.join(sdl2_root, "include", "SDL2"))
    add_linkdirs(path.join(sdl2_root, "lib"))
    -- SDL2.dll 复制到输出目录
    after_build(function (target)
        local dll_src = path.join(sdl2_root, "bin", "SDL2.dll")
        local dll_dst = path.join(target:targetdir(), "SDL2.dll")
        if os.isfile(dll_src) and not os.isfile(dll_dst) then
            os.cp(dll_src, dll_dst)
        end
    end)
