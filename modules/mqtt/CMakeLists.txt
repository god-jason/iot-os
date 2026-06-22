# MQTT 3.1.1 客户端模块，基于 net_socket 实现
#
# 文件结构：
#   mqtt_packets.c/h     - MQTT 协议包编解码（14种数据包类型）
#   mqtt_publisher.c/h   - 发送队列和 QoS 处理
#   mqtt_keepalive.c/h   - 心跳机制和自动重连
#   mqtt_event.c/h       - 事件处理
#   mqtt_client.c/h      - MQTT 客户端核心实现
#   mqtt_module.c        - Lua 绑定（包含路由功能）

add_library(iot_mqtt STATIC
    mqtt_packets.c
    mqtt_publisher.c
    mqtt_keepalive.c
    mqtt_event.c
    mqtt_client.c
    mqtt_module.c
)

target_include_directories(iot_mqtt PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(iot_mqtt
    PUBLIC
        iot_core
    PRIVATE
        iot_net
)

target_compile_options(iot_mqtt PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)