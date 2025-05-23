list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(exiv2 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(spdlog REQUIRED)
find_package(toml11 REQUIRED)

# inspired by https://github.com/Exiv2/exiv2/commit/130064dc76d2494b6fd473b293f2f32d36eb88f3,
# needed here because package in Ubuntu 22.04 was not updated yet
if (NOT TARGET Exiv2::exiv2lib)
    add_library(Exiv2::exiv2lib ALIAS exiv2lib)
endif()

pkg_check_modules(AVFORMAT REQUIRED libavformat libavutil)
pkg_check_modules(LIBJPEG REQUIRED libturbojpeg)

if(exiv2_VERSION VERSION_LESS "0.28")
    add_compile_definitions(EXIV2_HAS_TOLONG=1)
endif()

# just in case we need the timezone library
# set(BUILD_TZ_LIB ON CACHE BOOL "")
# set(USE_SYSTEM_TZ_DB ON CACHE BOOL "")
add_subdirectory("extern/date" EXCLUDE_FROM_ALL)

set(TARGET_NAME ${MEDIACOPIER_CORE_LIB})

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/mediacopier/version.hpp")

add_library(${TARGET_NAME} SHARED)

string(REPLACE "." "_" VERSION_WITH_UNDERSCORES ${CMAKE_PROJECT_VERSION})

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME mediacopier-core-${VERSION_WITH_UNDERSCORES})

target_sources(${TARGET_NAME} PRIVATE
    "include/mediacopier/abstract_file_info.hpp"
    "include/mediacopier/abstract_operation.hpp"
    "include/mediacopier/duplicate_check.hpp"
    "include/mediacopier/error.hpp"
    "include/mediacopier/file_info_factory.hpp"
    "include/mediacopier/file_info_image.hpp"
    "include/mediacopier/file_info_image_jpeg.hpp"
    "include/mediacopier/file_info_video.hpp"
    "include/mediacopier/file_register.hpp"
    "include/mediacopier/operation_copy.hpp"
    "include/mediacopier/operation_copy_jpeg.hpp"
    "include/mediacopier/operation_move.hpp"
    "include/mediacopier/operation_move_jpeg.hpp"
    "include/mediacopier/operation_simulate.hpp"
    "include/mediacopier/persistent_config.hpp"
    "source/duplicate_check.cpp"
    "source/file_info_factory.cpp"
    "source/file_info_image.cpp"
    "source/file_info_image_jpeg.cpp"
    "source/file_info_video.cpp"
    "source/file_register.cpp"
    "source/operation_copy.cpp"
    "source/operation_copy_jpeg.cpp"
    "source/operation_move.cpp"
    "source/operation_move_jpeg.cpp"
    "source/operation_simulate.cpp"
    "source/persistent_config.cpp")

target_include_directories(${TARGET_NAME} PRIVATE
    ${AVFORMAT_INCLUDE_DIRS}
    ${LIBJPEG_INCLUDE_DIRS})

target_include_directories(${TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(${TARGET_NAME} PRIVATE
    ${AVFORMAT_LINK_LIBRARIES} ${LIBJPEG_LINK_LIBRARIES}
    date::date Exiv2::exiv2lib spdlog::spdlog)

if(${ENABLE_TEST_COVERAGE})
    set(CMAKE_CXX_FLAGS "-g -O0 --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "--coverage")
    set(ENABLE_TEST ON)
    include(TestCoverage)
endif()

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()

if(${INSTALL_DEV_FILES})
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    set(TARGETS_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/mediacopier)

    # install headers
    install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/" "${CMAKE_CURRENT_BINARY_DIR}/include/" DESTINATION include)

    # install library (and flag the target for export)
    install(
        TARGETS ${TARGET_NAME}
        EXPORT ${PROJECT_NAME}Targets)

    # install target files
    install(
        EXPORT ${PROJECT_NAME}Targets
        NAMESPACE ${PROJECT_NAME}::
        DESTINATION "${TARGETS_INSTALL_DIR}")

    # create config file for enabling `find_package`
    configure_package_config_file(
        "cmake/Config.cmake.in"
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION "${TARGETS_INSTALL_DIR}")

    # create version file for enabling version check with `find_package`
    write_basic_package_version_file(
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        COMPATIBILITY ExactVersion)

    # install config and version files
    install(
        FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
            "${CMAKE_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION "${TARGETS_INSTALL_DIR}")
endif()

install(TARGETS ${TARGET_NAME})
