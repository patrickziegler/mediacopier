cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

project(MediaCopier VERSION 0.9.0)

# ----------   S H A R E D   L I B R A R Y   ----------

add_library(mediacopier SHARED)
target_include_directories(mediacopier PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/mediacopier")
include(src/mediacopier/CMakeLists.txt)

find_package(Boost REQUIRED filesystem date_time)
target_include_directories(mediacopier PRIVATE ${BOOST_INCLUDE_DIRS})
target_link_libraries(mediacopier ${Boost_LIBRARIES})

find_package(avformat REQUIRED)
target_include_directories(mediacopier PRIVATE ${AVFORMAT_INCLUDE_DIR})
target_link_libraries(mediacopier ${AVFORMAT_LIBRARY})

find_package(avutil REQUIRED)
target_include_directories(mediacopier PRIVATE ${AVUTIL_INCLUDE_DIR})
target_link_libraries(mediacopier ${AVUTIL_LIBRARY})

find_package(JPEG REQUIRED)
target_include_directories(mediacopier PRIVATE ${JPEG_INCLUDE_DIRS})
target_link_libraries(mediacopier ${JPEG_LIBRARIES})

target_include_directories(mediacopier PRIVATE lib/libjpeg-turbo)
target_sources(mediacopier PRIVATE lib/libjpeg-turbo/transupp.c)

target_link_options(mediacopier PRIVATE "-lexiv2")

# ----------   E X E C U T A B L E S   ----------

add_executable(mcp)
add_executable(mmv)

include(src/CMakeLists.txt)

target_link_libraries(mcp mediacopier)
target_link_libraries(mmv mediacopier)

target_link_options(mcp PRIVATE "-lexiv2")
target_link_options(mmv PRIVATE "-lexiv2")

find_package(OpenMP REQUIRED)
target_link_libraries(mcp OpenMP::OpenMP_CXX)
target_link_libraries(mmv OpenMP::OpenMP_CXX)

# ----------   I N S T A L L E R S   ----------

install(TARGETS mcp mmv
    RUNTIME
    DESTINATION bin
    COMPONENT bin
    )

install(TARGETS mediacopier
    LIBRARY
    DESTINATION lib
    COMPONENT bin
    )

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/mediacopier"
    DESTINATION include
    COMPONENT headers
    )

include(CPackConfig)
