find_package(Boost REQUIRED filesystem date_time)
find_package(avformat REQUIRED)
find_package(avutil REQUIRED)
find_package(JPEG REQUIRED)

add_library(mediacopier SHARED)

target_include_directories(mediacopier PRIVATE ${BOOST_INCLUDE_DIRS})
target_include_directories(mediacopier PRIVATE ${AVFORMAT_INCLUDE_DIR})
target_include_directories(mediacopier PRIVATE ${AVUTIL_INCLUDE_DIR})
target_include_directories(mediacopier PRIVATE ${JPEG_INCLUDE_DIRS})

target_include_directories(mediacopier PUBLIC "${CMAKE_SOURCE_DIR}/include/mediacopier")
target_include_directories(mediacopier PRIVATE "${CMAKE_SOURCE_DIR}/extern/libjpeg-turbo")

target_sources(mediacopier PRIVATE
    "${CMAKE_SOURCE_DIR}/extern/libjpeg-turbo/transupp.c"
    )

target_sources(mediacopier PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/ConfigManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileOperation.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/FileOperationStrategy.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ProgressBar.cpp"
    )

target_link_libraries(mediacopier ${Boost_LIBRARIES})
target_link_libraries(mediacopier ${AVFORMAT_LIBRARY})
target_link_libraries(mediacopier ${AVUTIL_LIBRARY})
target_link_libraries(mediacopier ${JPEG_LIBRARIES})

target_link_options(mediacopier PRIVATE "-lexiv2")

install(TARGETS mediacopier
    LIBRARY
    DESTINATION lib
    COMPONENT Lib
    )

# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/mediacopier"
#     DESTINATION include
#     COMPONENT headers
#     )
