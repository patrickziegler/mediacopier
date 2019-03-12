find_package(Boost REQUIRED unit_test_framework)

include(BoostTestHelpers)

enable_testing()

add_boost_test(test_ConfigManager.cpp mediacopier)
add_boost_test(test_FileOperation.cpp mediacopier)
add_boost_test(test_FileOperationStrategy.cpp mediacopier)

add_custom_command(
    OUTPUT "${CMAKE_BINARY_DIR}/test/data"
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/generate_test_data.sh" "${CMAKE_BINARY_DIR}/test/data"
    )

add_custom_target(
    test_data
    DEPENDS "${CMAKE_BINARY_DIR}/test/data"
    )

add_dependencies(test_FileOperationStrategy test_data)
