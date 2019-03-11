set(AVFORMAT_FOUND "NO")

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
    /mingw/include
    )

message("-- Set AVFORMAT_INCLUDE_DIR to ${AVFORMAT_INCLUDE_DIR}")

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
    /mingw
    )

message("-- Set AVFORMAT_LIBRARY to ${AVFORMAT_LIBRARY}")

set(AVFORMAT_FOUND "YES")
