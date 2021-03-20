set(TEST_DATA_DIR "${CMAKE_CURRENT_BINARY_DIR}/data/original")

add_compile_definitions(TEST_DATA_DIR="${TEST_DATA_DIR}")

add_custom_command(
    OUTPUT "${TEST_DATA_DIR}"
    COMMAND "${CMAKE_CURRENT_LIST_DIR}/generate_test_data.sh" "${TEST_DATA_DIR}")

add_custom_target(${TEST_DATA_TARGET_NAME}
    DEPENDS "${TEST_DATA_DIR}")
