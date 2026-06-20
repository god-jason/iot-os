cmake_minimum_required(VERSION 3.10)

project(drivers)

set(DRIVER_DIRS
    bme280
    bh1750
    dht
    ssd1306
    mpu6050
    pcf8574
    ads1115
    ds18b20
    hmc5883l
    lcd1602
    relay
    led
    buzzer
)

file(GLOB DRIVER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.c
)

foreach(DIR ${DRIVER_DIRS})
    file(GLOB DIR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/*.c)
    list(APPEND DRIVER_SOURCES ${DIR_SOURCES})
endforeach()

file(GLOB DRIVER_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

foreach(DIR ${DRIVER_DIRS})
    file(GLOB DIR_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}/*.h)
    list(APPEND DRIVER_HEADERS ${DIR_HEADERS})
endforeach()

add_library(drivers STATIC
    ${DRIVER_SOURCES}
)

target_include_directories(drivers PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/iot
)

foreach(DIR ${DRIVER_DIRS})
    target_include_directories(drivers PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/${DIR}
    )
endforeach()

target_link_libraries(drivers PUBLIC
    iot
)

if(USE_I2C)
    target_compile_definitions(drivers PRIVATE USE_I2C)
endif()

if(USE_SPI)
    target_compile_definitions(drivers PRIVATE USE_SPI)
endif()

install(FILES ${DRIVER_HEADERS} DESTINATION include/drivers)
install(TARGETS drivers ARCHIVE DESTINATION lib)
