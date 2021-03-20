cmake_minimum_required(VERSION 3.12)

project(MediaCopier VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(ENABLE_COVERAGE "Activate flags for gcov on compilation" OFF)
option(ENABLE_GUI "Include graphical user interface" OFF)
option(ENABLE_TEST "Include test targets" OFF)

if(${ENABLE_COVERAGE})
    include(TestCoverage)
    set(CMAKE_CXX_FLAGS "-g -O0 --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "--coverage")
    set(ENABLE_GUI OFF)
    set(ENABLE_TEST ON)
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

if(${ENABLE_TEST})
    set(TEST_DATA_TARGET_NAME test-data)
    add_subdirectory(test-data)
    enable_testing()
endif()

set(MEDIACOPIER_LIB_TARGET_NAME mediacopier-lib)

add_subdirectory(mediacopier/lib)

add_subdirectory(mediacopier/cli)

if(${ENABLE_GUI})
    add_subdirectory(mediacopier-gui)
endif()
