add_executable(mediacopier-cli
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    )

set_target_properties(mediacopier-cli PROPERTIES
    OUTPUT_NAME mediacopier
    )

target_link_libraries(mediacopier-cli PRIVATE
    mediacopier
    )

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()
