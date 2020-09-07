find_path(EXIV2_INCLUDE_DIR
    NAMES exiv2/exiv2.hpp
    PATHS $ENV{LIB_DIR}/include /usr/local/include /usr/include)

find_library(EXIV2_LIBRARY
    NAMES exiv2
    PATHS $ENV{LIB_DIR}/lib /usr/local/lib /usr/lib)

if(EXIV2_INCLUDE_DIR AND EXIV2_LIBRARY)
    set(EXIV2_FOUND TRUE)
    message(STATUS "Found Exiv2 '${EXIV2_LIBRARY}'")
else()
    message(EXIV2_INCLUDE_DIR=${EXIV2_INCLUDE_DIR})
    message(EXIV2_LIBRARY=${EXIV2_LIBRARY})
    message(FATAL_ERROR "Could not find Exiv2")
endif()
