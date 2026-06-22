# HTTP 客户端模块，基于 net_socket 实现
#
# 文件结构：
#   http_client.h   - HTTP 客户端接口定义
#   http_client.c   - HTTP 客户端核心实现
#   http_manager.c  - HTTP 管理器实现
#   http_server.c   - HTTP 服务器实现
#   http_gzip.c     - Gzip 压缩支持
#   http_url.c      - URL 解析与编码工具
#   http_form.c     - 表单数据处理工具
#   http_module.c   - Lua 绑定

add_library(iot_http STATIC
    http_client.c
    http_manager.c
    http_server.c
    http_gzip.c
    http_url.c
    http_form.c
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
        iot_zlib
)

target_compile_options(iot_http PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
