set(target demo)

add_app_libraries($<TARGET_FILE:${target}>)
add_library(${target} STATIC)
set_target_properties(${target} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${YOPEN_OUT_LIB_DIR})

###############
## 
###############
relative_glob(srcs *.h *.c *.cpp )

target_sources(${target} PRIVATE
    ${srcs}   ${HTTPCLIENT_DIR_SRCS} 
)

###############
## 编译参数
###############
## -E -P -x c
# target_compile_options(${target} PRIVATE -E -P -x c)

###############
## 包含头文件
###############
target_include_targets(${target} PRIVATE ${YOPEN_BSP_TARGET} components.httpclient)
