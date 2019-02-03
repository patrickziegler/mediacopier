cmake_minimum_required(VERSION 3.9)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib") 
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

project(mcp_cc VERSION 0.1.0)

# ----------   S H A R E D   L I B R A R Y   ----------

add_library(${PROJECT_NAME} SHARED)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}")
include(src/lib/CMakeLists.txt)

find_package(Boost REQUIRED filesystem date_time)
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOST_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})

find_package(avformat REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${AVFORMAT_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${AVFORMAT_LIBRARY})

find_package(avutil REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${AVUTIL_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${AVUTIL_LIBRARY})

target_link_options(${PROJECT_NAME} PRIVATE "-lexiv2")

# ----------   E X E C U T A B L E S   ----------

add_executable(mcp)
add_executable(mmv)

include(src/CMakeLists.txt)

target_link_libraries(mcp ${PROJECT_NAME})
target_link_libraries(mmv ${PROJECT_NAME})

find_package(OpenMP REQUIRED)
target_link_libraries(mcp OpenMP::OpenMP_CXX)
target_link_libraries(mmv OpenMP::OpenMP_CXX)

# ----------   I N S T A L L E R S   ----------

install(TARGETS mcp mmv
    RUNTIME
    DESTINATION bin
    COMPONENT bin
    )

install(TARGETS ${PROJECT_NAME}
    LIBRARY
    DESTINATION lib
    COMPONENT bin
    ) # don't forget to run ldconfig as root after installation!

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}"
    DESTINATION include
    COMPONENT headers
    )

include(CPackConfig)
