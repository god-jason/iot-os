-- cjson xmake 配置

target("cjson")
    set_kind("static")
    add_files("cJSON.c", "cJSON_Utils.c")
    add_headerfiles("cJSON.h", "cJSON_Utils.h")
    add_includedirs(".", "../lua")
    add_deps("lua")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")