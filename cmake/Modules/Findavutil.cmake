set(AVUTIL_FOUND "NO")

find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h
    HINTS
    $ENV{AVUTILDIR}
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

message("-- Set AVUTIL_INCLUDE_DIR to ${AVUTIL_INCLUDE_DIR}")

find_library( AVUTIL_LIBRARY
    NAMES avutil
    HINTS
    $ENV{AVUTILDIR}
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

message("-- Set AVUTIL_LIBRARY to ${AVUTIL_LIBRARY}")

set(AVUTIL_FOUND "YES")
