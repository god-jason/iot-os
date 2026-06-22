add_library(iot_mqtt STATIC
    mqtt_packets.c
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