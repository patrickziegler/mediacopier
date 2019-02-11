add_library(transupp SHARED)

target_sources(transupp PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/lib/transupp/src/transupp.c"
    )

target_include_directories(transupp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/transupp/src")
target_include_directories(transupp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/lib/transupp/include")

find_package(JPEG REQUIRED)
target_include_directories(transupp PRIVATE ${JPEG_INCLUDE_DIRS})
target_link_libraries(transupp ${JPEG_LIBRARIES})
