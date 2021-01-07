find_package(log4cplus)

add_library(mediacopier-cli STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/src/config.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/feedback.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/run.cpp")

target_include_directories(mediacopier-cli PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(mediacopier-cli PUBLIC
    "${LOG4CPLUS_LIBRARY}"
    mediacopier)
