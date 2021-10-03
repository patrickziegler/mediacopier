find_package(exiv2 REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(spdlog REQUIRED)

pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(LIBJPEG REQUIRED libturbojpeg)

set(TARGET_NAME ${MEDIACOPIER_LIB_TARGET_NAME})

configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/src/version.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/include/mediacopier/version.hpp")

add_library(${TARGET_NAME} SHARED)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME mediacopier)

target_sources(${TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/src/FileInfoFactory.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/FileInfoImage.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/FileInfoImageJpeg.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/FileInfoVideo.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/FileOperationCopy.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/FileOperationCopyJpeg.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/FileOperationMove.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/FileOperationMoveJpeg.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/FileRegister.cpp")

target_include_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/extern/date/include")

target_include_directories(${TARGET_NAME} PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_CURRENT_LIST_DIR}/include")

target_link_libraries(${TARGET_NAME} PUBLIC
    "${AVFORMAT_LIBRARIES}"
    "${LIBJPEG_LIBRARIES}"
    exiv2lib
    spdlog::spdlog)

set_target_properties(${TARGET_NAME} PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR})

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()
