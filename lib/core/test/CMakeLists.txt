find_package(GTest)

set(TARGET_NAME libmediacopier-core-unittest)

add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "common_test_fixtures.hpp"
    "test_file_info_classes.cpp"
    "test_file_operation_classes.cpp"
    "test_file_register.cpp")

target_link_libraries(${TARGET_NAME} PRIVATE
    gtest gtest_main "${MEDIACOPIER_CORE_LIB}")

target_compile_definitions(${TARGET_NAME} PRIVATE
    "TEST_DATA_DIR=\"${CMAKE_CURRENT_LIST_DIR}/data\"")

gtest_discover_tests(${TARGET_NAME}
    TEST_PREFIX "${TARGET_NAME}_")

enable_testing()

register_code_format_target(${TARGET_NAME})
