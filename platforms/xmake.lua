-- platforms xmake 配置

-- 获取当前平台配置
local plat = get_config("platform") or "windows"

-- 根据 platform 参数创建目标
-- 创建平台目标
target("platform")
set_kind("static")
add_files(plat .. "/modules/*.c")
add_headerfiles(plat .. "/*.h")
add_includedirs(plat)
add_includedirs("../vendor/lua", "../iot")
add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
