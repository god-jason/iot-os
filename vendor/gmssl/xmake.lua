-- gmssl xmake 配置

target("gmssl")
    set_kind("static")

    -- 添加源文件（排除 tools 目录和平台特定文件）
    -- Windows 平台排除：ARM64 文件、Unix/Apple 特定文件、网络/TLS 相关文件（需要 Unix socket）、OpenCL 文件
    add_files("src/*.c|tools/**|gf128_arm64.c|sm2_z256_arm64.c|sm3_arm64.c|sm4_arm64.c|sm4_ce.c|sm9_z256_arm64.c|rand_apple.c|rand_unix.c|socket.c|http.c|http_win.c|ocsp.c|sct.c|tls.c|tls12.c|tls13.c|tls_alpn.c|tls_cert.c|tls_cookie.c|tls_ext.c|tls_ocsp.c|tls_psk.c|tls_sct.c|tls_sni.c|tls_trace.c|tlcp.c|sm4_cl.c")
    -- SDF/SKF 目录在 Windows 上无法编译（需要 dlfcn.h）
    -- add_files("src/sdf/*.c|sdf_ext.c")
    -- add_files("src/skf/*.c")
    add_files("gmssl_module.c")

    -- 添加头文件
    add_headerfiles("include/gmssl/*.h")
    add_headerfiles("src/sdf/*.h")
    add_headerfiles("src/skf/*.h")

    -- 包含目录
    add_includedirs("include", "src", "../lua")

    -- 依赖（gmssl_module.c 依赖 lua）
    add_deps("lua")

    -- 编译选项（AVX/SSE 指令集支持）
    add_cflags("-Wall", "-Wextra", "-Wno-unused-parameter", "-mssse3", "-mpclmul", "-maes", "-mrdrnd", "-mavx2")

    -- 定义宏（启用 SM4 各种模式）
    add_defines("ENABLE_SM4_ECB", "ENABLE_SM4_OFB", "ENABLE_SM4_CFB", "ENABLE_SM4_CCM", "ENABLE_SM4_XTS", "ENABLE_SHA1", "ENABLE_SHA2")