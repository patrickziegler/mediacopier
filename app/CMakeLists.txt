find_package(log4cplus)

add_executable(mediacopier-main
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

set_target_properties(mediacopier-main PROPERTIES
    OUTPUT_NAME mediacopier)

add_subdirectory(cli)

target_link_libraries(mediacopier-main PRIVATE
    "${LOG4CPLUS_LIBRARY}" mediacopier-cli)

if(${ENABLE_GUI})
    add_subdirectory(gui)
    target_link_libraries(mediacopier-main PRIVATE
        mediacopier-gui)
endif()

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()
