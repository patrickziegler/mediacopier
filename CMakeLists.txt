cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(ENABLE_TEST "Include test targets" ON)
option(ENABLE_GUI  "Include graphical user interface" ON)

if(${ENABLE_GUI})
    add_compile_definitions(ENABLE_GUI)
endif()

project(MediaCopier VERSION 2.0.0)

add_subdirectory(extern/libjpeg-turbo)
add_subdirectory(lib)
add_subdirectory(app)

if(${ENABLE_TEST})
    enable_testing()
endif()
