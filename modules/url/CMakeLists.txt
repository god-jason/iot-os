# modules/url CMakeLists.txt
# URL 解析和操作库

add_library(iot_url STATIC
    url.c
    url_module.c
)

target_include_directories(iot_url PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(iot_url
    PUBLIC
        iot_core
)

# 添加编译选项
target_compile_options(iot_url PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)
