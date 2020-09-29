find_package(avformat)
find_package(Exiv2)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.hpp.in"
    "${CMAKE_BINARY_DIR}/include/mediacopier/version.hpp")

add_library(mediacopier SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoImage.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoImageJpeg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoVideo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileOperationCopy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileOperationCopyJpeg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileOperationMove.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileOperationMoveJpeg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FilePathFactory.cpp")

target_include_directories(mediacopier PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_BINARY_DIR}/include")

target_include_directories(mediacopier PRIVATE
    "${CMAKE_SOURCE_DIR}/extern/date/include"
    "${CMAKE_SOURCE_DIR}/extern/libjpeg-turbo/"
    "${AVFORMAT_INCLUDE_DIR}"
    "${EXIV2_INCLUDE_DIR}")

set_property(TARGET turbojpeg-static PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(mediacopier PRIVATE
    "${AVFORMAT_LIBRARY}"
    "${EXIV2_LIBRARY}"
    turbojpeg-static)

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()
