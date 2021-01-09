set(TEST_DATA_DIR "${CMAKE_BINARY_DIR}/test/data")

add_custom_target(mediacopier-test-data
    DEPENDS "${TEST_DATA_DIR}")

add_custom_command(
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/generate_test_data.sh" "${TEST_DATA_DIR}"
    OUTPUT "${TEST_DATA_DIR}")
