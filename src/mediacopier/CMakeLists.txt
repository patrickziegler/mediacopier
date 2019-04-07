find_package(OpenMP REQUIRED)
find_package(Boost REQUIRED filesystem date_time)
find_package(avformat REQUIRED)
find_package(avutil REQUIRED)
find_package(JPEG REQUIRED)
find_package(Exiv2 REQUIRED)

add_library(mediacopier SHARED)

target_include_directories(mediacopier PRIVATE ${BOOST_INCLUDE_DIRS})
target_include_directories(mediacopier PRIVATE ${AVFORMAT_INCLUDE_DIR})
target_include_directories(mediacopier PRIVATE ${AVUTIL_INCLUDE_DIR})
target_include_directories(mediacopier PRIVATE ${JPEG_INCLUDE_DIRS})
target_include_directories(mediacopier PRIVATE ${EXIV2_INCLUDE_DIR})
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
    "${CMAKE_CURRENT_LIST_DIR}/Run.cpp"
    )

target_link_libraries(mediacopier OpenMP::OpenMP_CXX)
target_link_libraries(mediacopier ${Boost_LIBRARIES})
target_link_libraries(mediacopier ${AVFORMAT_LIBRARY})
target_link_libraries(mediacopier ${AVUTIL_LIBRARY})
target_link_libraries(mediacopier ${JPEG_LIBRARIES})
target_link_libraries(mediacopier ${EXIV2_LIBRARY})

install(TARGETS mediacopier
    LIBRARY
    DESTINATION lib
    COMPONENT Lib
    )

# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/mediacopier"
#     DESTINATION include
#     COMPONENT headers
#     )

add_custom_command(
    TARGET mediacopier
    POST_BUILD
    COMMAND cmake --build . --target all_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

add_custom_command(
    TARGET mediacopier
    POST_BUILD
    COMMAND ${CMAKE_CTEST_COMMAND}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/test
    )
