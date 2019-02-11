cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

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

find_package(transupp)
target_include_directories(mediacopier PRIVATE lib/transupp/include)
target_link_libraries(mediacopier transupp)

target_link_options(mediacopier PRIVATE "-lexiv2")

# ----------   E X E C U T A B L E S   ----------

add_executable(mcp)
add_executable(mmv)

include(src/CMakeLists.txt)

target_link_libraries(mcp mediacopier)
target_link_libraries(mmv mediacopier)

find_package(OpenMP REQUIRED)
target_link_libraries(mcp OpenMP::OpenMP_CXX)
target_link_libraries(mmv OpenMP::OpenMP_CXX)

# ----------   I N S T A L L E R S   ----------

install(TARGETS mcp mmv
    RUNTIME
    DESTINATION bin
    COMPONENT bin
    )

install(TARGETS mediacopier transupp
    LIBRARY
    DESTINATION lib
    COMPONENT bin
    ) # don't forget to run ldconfig as root after installation!

# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/mediacopier"
#     DESTINATION include
#     COMPONENT headers
#     )

include(CPackConfig)
