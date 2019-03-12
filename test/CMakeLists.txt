find_package(Boost REQUIRED unit_test_framework)

add_executable(test)

target_include_directories(test PRIVATE ${BOOST_INCLUDE_DIRS})

target_sources(test PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/test_ConfigManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test_FileOperation.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/test_FileOperationStrategy.cpp"
    )

target_link_libraries(test mediacopier)
target_link_libraries(test ${Boost_LIBRARIES})

add_custom_command(
    TARGET test
    POST_BUILD
    COMMAND eval "${CMAKE_BINARY_DIR}/test/test" -l all -r detailed
    )

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/test/data"
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/generate_test_data.sh" "${CMAKE_BINARY_DIR}/test/data"
    )

add_custom_target(
    test_data
    DEPENDS "${CMAKE_BINARY_DIR}/test/data"
    )

add_dependencies(test test_data)
