-- sqlite3 xmake 配置

target("sqlite3")
    set_kind("static")
    add_files("sqlite3.c", "sqlite3_module.c")
    add_headerfiles("sqlite3.h", "sqlite3ext.h")
    add_includedirs(".")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter", "-DSQLITE_THREADSAFE=1", "-DSQLITE_OMIT_LOAD_EXTENSION")