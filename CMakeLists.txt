cmake_minimum_required(VERSION 3.12)

project(MediaCopier VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

option(ENABLE_CLI "Build command line interface" ON)
option(ENABLE_KDE "Build KDE integration" OFF)
option(ENABLE_SHARED_LIB "Build shared instead of static lib" OFF)
option(ENABLE_TEST "Include test targets" OFF)
option(ENABLE_TEST_COVERAGE "Activate flags for gcov on compilation" OFF)
option(INSTALL_DEV_FILES "Install library headers and cmake targets" OFF)

if(${ENABLE_TEST_COVERAGE})
    include(TestCoverage)
    set(ENABLE_TEST ON)
endif()

set(MEDIACOPIER_LIB_TARGET_NAME mediacopier-lib)

add_subdirectory(mediacopier-lib)

if(${ENABLE_CLI})
    add_subdirectory(mediacopier-cli)
endif()

if(${ENABLE_TEST})
    enable_testing()
endif()
