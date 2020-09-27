find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h
    HINTS
    $ENV{AVFORMATDIR}
    PATH_SUFFIXES ffmpeg
    PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /usr/local/include
    /usr/include
    /sw/include
    /opt/local/include
    /opt/csw/include
    /opt/include
    /mingw/include)

find_library(AVFORMAT_LIBRARY
    NAMES avformat
    HINTS
    $ENV{AVFORMATDIR}
    PATH_SUFFIXES lib64 lib bin
    PATHS
    /usr/local
    /usr
    /sw
    /opt/local
    /opt/csw
    /opt
    /mingw)

if(AVFORMAT_INCLUDE_DIR AND AVFORMAT_LIBRARY)
    set(AVFORMAT_FOUND TRUE)
    message(STATUS "Found libavformat '${AVFORMAT_LIBRARY}'")
else()
    message(AVFORMAT_INCLUDE_DIR=${AVFORMAT_INCLUDE_DIR})
    message(AVFORMAT_LIBRARY=${AVFORMAT_LIBRARY})
    message(FATAL_ERROR "Could not find libavformat")
endif()
