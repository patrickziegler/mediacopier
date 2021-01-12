cmake_minimum_required(VERSION 3.12)

project(MediaCopier VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(ENABLE_COVERAGE "Activate flags for gcov on compilation" OFF)
option(ENABLE_GUI      "Include graphical user interface" ON)
option(ENABLE_TEST     "Include test targets" ON)

if(${ENABLE_COVERAGE})
    include(TestCoverage)
    set(CMAKE_CXX_FLAGS "-g -O0 --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "--coverage")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

if(${ENABLE_GUI})
    add_compile_definitions(ENABLE_GUI)
endif()

if(${ENABLE_TEST})
    add_subdirectory("${CMAKE_SOURCE_DIR}/extern/googletest")
    enable_testing()
endif()

add_subdirectory("${CMAKE_SOURCE_DIR}/lib")
add_subdirectory("${CMAKE_SOURCE_DIR}/app")
