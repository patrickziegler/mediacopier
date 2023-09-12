cmake_minimum_required(VERSION 3.12)

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(mediacopier VERSION 2.0.7)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

if(${MSVC})
    # range-v3 library needs permissive ruleset when compiled with MSVC
    # https://github.com/ericniebler/range-v3#supported-compilers
    add_compile_options("/permissive-")

    # Mitigation for (by Microsoft) broken macro usage in Howard Hinnans date library
    # http://www.suodenjoki.dk/us/archive/2010/min-max.htm
    add_compile_definitions(NOMINMAX=1)

    # Exiv2 still uses std::auto_ptr, MSCV needs to be specifically asked to support that
    # https://github.com/Exiv2/exiv2/issues/46
    # https://stackoverflow.com/a/48883091
    find_package(exiv2 REQUIRED)
    if(exiv2_VERSION VERSION_LESS "0.28")
        add_compile_definitions(_HAS_AUTO_PTR_ETC=1)
    endif()

    set(CLI_TARGET_FLAGS WIN32)
endif()

option(ENABLE_CLI "Build Qt based cli tool" ON)
option(ENABLE_KDE "Enable KDE integration for cli tool" OFF)
option(ENABLE_SHARED_LIB "Build shared instead of static lib" OFF)
option(ENABLE_TEST "Enable test targets" OFF)
option(ENABLE_TEST_COVERAGE "Enable test and coverage targets" OFF)
option(INSTALL_DEV_FILES "Install library headers and cmake targets" OFF)

if(${ENABLE_TEST_COVERAGE})
    set(CMAKE_CXX_FLAGS "-g -O0 --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "--coverage")
    set(ENABLE_TEST ON)
    include(TestCoverage)
endif()

set(MEDIACOPIER_LIB_TARGET_NAME mediacopier-lib)

add_subdirectory(mediacopier-lib)

if(${ENABLE_CLI})
    add_subdirectory(mediacopier-cli)
endif()

if(${ENABLE_TEST})
    enable_testing()
endif()
