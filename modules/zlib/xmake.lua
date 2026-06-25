-- zlib 模块（基于 vendor/miniz）

target("iot_zlib")
    set_kind("static")
    add_files("deflate.c", "gzip.c", "zip.c", "tar.c", "zlib_module.c")
    add_headerfiles("*.h")
    add_includedirs(".", "..", "../../vendor/miniz")
    add_deps("miniz")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
