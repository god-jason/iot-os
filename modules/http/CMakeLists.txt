# HTTP 客户端模块，基于 net_socket 实现
#
# 文件结构：
#   http.h          - HTTP 客户端接口定义
#   http.c          - HTTP 客户端核心实现
#   http_module.c   - Lua 绑定

add_library(iot_http STATIC
    http.c
    http_module.c
)

target_include_directories(iot_http PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(iot_http
    PUBLIC
        iot_core
    PRIVATE
        iot_net
        iot_url
)

target_compile_options(iot_http PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
