find_path(LOG4CPLUS_INCLUDE_DIR
    NAMES log4cplus/logger.h
    PATHS $ENV{LIB_DIR}/include /usr/local/include /usr/include)

find_library(LOG4CPLUS_LIBRARY
    NAMES log4cplus
    PATHS $ENV{LIB_DIR}/lib /usr/local/lib /usr/lib)

if(LOG4CPLUS_INCLUDE_DIR AND LOG4CPLUS_LIBRARY)
    set(LOG4CPLUS_FOUND TRUE)
    message(STATUS "Found Log4CPlus '${LOG4CPLUS_LIBRARY}'")
else()
    message(LOG4CPLUS_INCLUDE_DIR=${LOG4CPLUS_INCLUDE_DIR})
    message(LOG4CPLUS_LIBRARY=${LOG4CPLUS_LIBRARY})
    message(FATAL_ERROR "Could not find Log4CPlus")
endif()
