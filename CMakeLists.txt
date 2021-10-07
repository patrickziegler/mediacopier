cmake_minimum_required(VERSION 3.12)

project(MediaCopier VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(ENABLE_COVERAGE "Activate flags for gcov on compilation" OFF)
option(ENABLE_CLI "Include command line interface" ON)
option(ENABLE_TEST "Include test targets" OFF)
option(INSTALL_DEV_FILES "Install library headers and cmake targets" OFF)

if(${ENABLE_COVERAGE})
    include(TestCoverage)
    set(ENABLE_TEST ON)
endif()

set(MEDIACOPIER_LIB_TARGET_NAME mediacopier-lib)

add_subdirectory(mediacopier)

if(${ENABLE_CLI})
    add_subdirectory(mediacopier-cli)
endif()

if(${ENABLE_TEST})
    enable_testing()
endif()
