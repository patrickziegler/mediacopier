find_package(exiv2 REQUIRED)
find_package(spdlog REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(AVFORMAT REQUIRED libavformat libavutil)
pkg_check_modules(LIBJPEG REQUIRED libturbojpeg)

set(TARGET_NAME ${MEDIACOPIER_LIB_TARGET_NAME})

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/mediacopier/version.hpp")

if (${ENABLE_SHARED_LIB})
    add_library(${TARGET_NAME} SHARED)
    set_target_properties(${TARGET_NAME} PROPERTIES
        VERSION ${CMAKE_PROJECT_VERSION}
        SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR})
else()
    add_library(${TARGET_NAME} STATIC)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME mediacopier)

target_sources(${TARGET_NAME} PRIVATE
    "src/files/image.cpp"
    "src/files/image_jpeg.cpp"
    "src/files/video.cpp"
    "src/operations/copy.cpp"
    "src/operations/copy_jpeg.cpp"
    "src/operations/move.cpp"
    "src/operations/move_jpeg.cpp"
    "src/operations/simulate.cpp"
    "src/file_info_factory.cpp"
    "src/file_info_register.cpp")

target_include_directories(${TARGET_NAME} PRIVATE
    "${AVFORMAT_INCLUDE_DIRS}"
    "${LIBJPEG_INCLUDE_DIRS}"
    "extern/date/include")

target_include_directories(${TARGET_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_link_libraries(${TARGET_NAME} PRIVATE
    "${AVFORMAT_LINK_LIBRARIES}"
    "${LIBJPEG_LINK_LIBRARIES}"
    exiv2lib spdlog::spdlog)

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
        cmake/Config.cmake.in
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

elseif(${ENABLE_SHARED_LIB})
    # install library
    install(TARGETS ${TARGET_NAME})

endif()
