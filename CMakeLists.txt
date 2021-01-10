cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(ENABLE_TEST "Include test targets" ON)
option(ENABLE_GUI  "Include graphical user interface" ON)

if(${ENABLE_GUI})
    add_compile_definitions(ENABLE_GUI)
endif()

project(MediaCopier VERSION 2.0.0)

if(${ENABLE_TEST})
    add_subdirectory("${CMAKE_SOURCE_DIR}/extern/googletest")
    enable_testing()
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/lib")
add_subdirectory("${CMAKE_SOURCE_DIR}/app")
