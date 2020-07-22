add_library(mediacopier SHARED
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/AbstractFile.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/AbstractFileFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/AbstractFileOperation.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/AbstractPathPattern.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/FileFactory.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/FileImage.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/FileOperationCopy.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/FileOperationMove.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/FileVideo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/core/PathPattern.cpp"
    )

target_sources(mediacopier PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/jpegtran/FileImageJpeg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/jpegtran/FileOperationCopyJpeg.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/jpegtran/FileOperationMoveJpeg.cpp"
    )

target_include_directories(mediacopier PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    )

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()
