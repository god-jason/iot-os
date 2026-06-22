add_library(iot_fs STATIC
    fs_module.c
    path.c
    fs_dir.c
)

target_include_directories(iot_fs PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(iot_fs
    PUBLIC
        iot_core
)

target_compile_options(iot_fs PRIVATE
    -Wall
    -Wextra
    -Wno-unused-parameter
)