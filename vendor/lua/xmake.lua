-- lua xmake 配置

target("lua")
    set_kind("static")
    
    -- 添加源文件
    add_files("lapi.c", "lauxlib.c", "lbaselib.c", "lbitlib.c")
    add_files("lcode.c", "lcorolib.c", "lctype.c", "ldblib.c")
    add_files("ldebug.c", "ldo.c", "ldump.c", "lfunc.c")
    add_files("lgc.c", "linit.c", "liolib.c", "llex.c")
    add_files("lmathlib.c", "lmem.c", "loadlib.c", "lobject.c")
    add_files("lopcodes.c", "lparser.c", "lstate.c", "lstring.c")
    add_files("lstrlib.c", "ltablib.c", "ltm.c", "lutf8lib.c", "loslib.c")
    add_files("lundump.c", "lvm.c", "lzio.c")
    
    -- 添加头文件
    add_headerfiles("lua.h", "luaconf.h", "lualib.h", "lauxlib.h")
    add_headerfiles("lapi.h", "lcode.h", "lctype.h", "ldebug.h")
    add_headerfiles("ldo.h", "lfunc.h", "lgc.h", "llex.h")
    add_headerfiles("llimits.h", "lmem.h", "lobject.h", "lopcodes.h")
    add_headerfiles("lparser.h", "lstate.h", "lstring.h", "ltable.h")
    add_headerfiles("ltm.h", "lundump.h", "lvm.h", "lzio.h")
    
    -- 包含目录
    add_deps("platform")
    add_includedirs("../../platforms/"..(get_config("platform") or "windows"))
    
    -- 编译选项
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")