-- miniz 第三方库

target("miniz")
    set_kind("static")
    add_files("miniz.c", "miniz_tdef.c", "miniz_tinfl.c", "miniz_zip.c")
    add_headerfiles("*.h")
    add_includedirs(".")
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter")
