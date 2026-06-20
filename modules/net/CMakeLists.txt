# components/net CMakeLists.txt
# 基于 lwip 的 socket 异步封装

add_library(iot_net STATIC
    net.c
    lua_module.c
)

target_include_directories(iot_net PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(iot_net
    PUBLIC
        iot_core
    PRIVATE
        lwip
)

# 添加编译选项
target_compile_options(iot_net PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)

# 平台检测
if(FREERTOS)
    target_compile_definitions(iot_net PUBLIC IOT_NET_FREERTOS=1)
elseif(LINUX)
    target_compile_definitions(iot_net PUBLIC IOT_NET_LINUX=1)
endif()
