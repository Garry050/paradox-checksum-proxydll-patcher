cmake_minimum_required(VERSION 3.15)
project(version_proxy C)

set(CMAKE_C_STANDARD 11)

add_library(version SHARED
    src/dllmain.c
    src/proxy.c
    src/patcher.c
    src/log.c
    exports.def
)

target_include_directories(version PRIVATE src)
target_link_libraries(version PRIVATE kernel32)

# x64 only
if(MSVC)
    target_compile_options(version PRIVATE /W3)
    set_target_properties(version PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

set_target_properties(version PROPERTIES
    PREFIX ""
    OUTPUT_NAME "version"
)
