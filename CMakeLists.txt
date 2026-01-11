cmake_minimum_required(VERSION 3.23)

project(MediaCopier VERSION 2.4.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(SKIP_GUI "Don't build graphical user interface" OFF)
option(SKIP_CLI "Don't build command line interface" OFF)
option(SKIP_KDE "Don't build KDE Plasma integration" OFF)
option(USE_QT5 "Build against Qt5 libraries (legacy)" OFF)
option(ENABLE_TEST "Enable test targets" OFF)
option(ENABLE_TEST_COVERAGE "Enable test and coverage targets" OFF)
option(ENABLE_CLANG_TIDY "Enable static code analysis with clang-tidy" OFF)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(ClangFormat)

if(${ENABLE_CLANG_TIDY})
    include(ClangTidy)
endif()

if(${ENABLE_TEST_COVERAGE})
    set(ENABLE_TEST ON)
    include(TestCoverage)
endif()

if(${MSVC})
    # Exiv2 still uses std::auto_ptr, MSCV needs to be specifically asked to support that
    # https://github.com/Exiv2/exiv2/issues/46
    # https://stackoverflow.com/a/48883091
    find_package(exiv2 REQUIRED)
    if(exiv2_VERSION VERSION_LESS "0.28")
        add_compile_definitions(_HAS_AUTO_PTR_ETC=1)
    endif()
    set(CLI_TARGET_FLAGS WIN32)
endif()

set(MEDIACOPIER_CORE_LIB libmediacopier-core)
set(MEDIACOPIER_UI_LIB libmediacopier-ui)

add_subdirectory(lib/core)

if(NOT ${SKIP_GUI} OR NOT ${SKIP_KDE})
    add_subdirectory(lib/ui)
endif()

if(NOT ${SKIP_GUI})
    add_subdirectory(app/mediacopier)
endif()

if(NOT ${SKIP_CLI})
    add_subdirectory(app/mediacopier-cli)
endif()

if(NOT ${SKIP_KDE})
    add_subdirectory(app/mediacopier-plasma)
endif()

if(${ENABLE_TEST})
    enable_testing()
endif()

add_code_format_targets()
