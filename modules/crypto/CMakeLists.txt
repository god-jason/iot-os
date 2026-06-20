# modules/crypto CMakeLists.txt
# 加密算法库，基于 GmSSL

add_library(iot_crypto STATIC
    crypto.c
    crypto_module.c
)

target_include_directories(iot_crypto PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 链接 GmSSL 库
target_link_libraries(iot_crypto
    PUBLIC
        iot_core
    PRIVATE
        gmssl
)

# 添加编译选项
target_compile_options(iot_crypto PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)

# 启用 GmSSL 的 SHA1 和 SHA2 支持
target_compile_definitions(iot_crypto PRIVATE
    ENABLE_SHA1=1
    ENABLE_SHA2=1
)
