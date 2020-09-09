find_package(Exiv2)

add_library(mediacopier SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoImage.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoJpeg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileInfoVideo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileOperationCopy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileOperationMove.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FileOperationPrint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/FilePathFormat.cpp"
    )

target_include_directories(mediacopier PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

target_include_directories(mediacopier PRIVATE
    "${CMAKE_SOURCE_DIR}/extern/date/include"
    "${EXIV2_INCLUDE_DIR}"
    )

target_link_libraries(mediacopier PRIVATE exiv2)

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()
