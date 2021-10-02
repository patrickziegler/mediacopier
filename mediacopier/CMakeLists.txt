find_package(avformat REQUIRED)
find_package(Exiv2 REQUIRED)
find_package(JPEG REQUIRED)
find_package(spdlog REQUIRED)

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
    "${CMAKE_CURRENT_LIST_DIR}/src/FileRegister.cpp"
    "${CMAKE_SOURCE_DIR}/extern/libjpeg-turbo/transupp.c")

target_include_directories(${TARGET_NAME} PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include")

target_include_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/extern/date/include"
    "${CMAKE_SOURCE_DIR}/extern/libjpeg-turbo/"
    "${AVFORMAT_INCLUDE_DIR}"
    "${JPEG_INCLUDE_DIRS}"
    "${EXIV2_INCLUDE_DIR}")

target_link_libraries(${TARGET_NAME} PRIVATE
    "${AVFORMAT_LIBRARY}"
    "${EXIV2_LIBRARY}"
    "${JPEG_LIBRARIES}")

target_link_libraries(${TARGET_NAME} PUBLIC
    spdlog::spdlog)

set_target_properties(${TARGET_NAME} PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR})

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()
