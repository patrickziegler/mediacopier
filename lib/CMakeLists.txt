find_package(avformat)
find_package(Exiv2)
find_package(JPEG)

add_library(mediacopier SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoImage.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoJpeg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoVideo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileOperationCopy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileOperationMove.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FilePathFormat.cpp"
    "${CMAKE_SOURCE_DIR}/extern/libjpeg-turbo/transupp.c"
    )

target_include_directories(mediacopier PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

target_include_directories(mediacopier PRIVATE
    "${CMAKE_SOURCE_DIR}/extern/date/include"
    "${CMAKE_SOURCE_DIR}/extern/libjpeg-turbo/"
    "${AVFORMAT_INCLUDE_DIR}"
    "${EXIV2_INCLUDE_DIR}"
    "${JPEG_INCLUDE_DIR}"
    )

target_link_libraries(mediacopier PRIVATE
    "${AVFORMAT_LIBRARY}"
    "${EXIV2_LIBRARY}"
    "${JPEG_LIBRARY}"
    )

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()
