find_package(GTest)

add_subdirectory(data)

set(TARGET_NAME mediacopier-unittest)

add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "common_test_fixtures.hpp"
    "file_info_type_detector.hpp"
    "test_file_info_classes.cpp"
    "test_file_operation_classes.cpp"
    "test_file_register.cpp")

target_link_libraries(${TARGET_NAME} PRIVATE
    gtest gtest_main "${MEDIACOPIER_CORE_LIB}")

target_compile_definitions(${TARGET_NAME} PRIVATE "TEST_DATA_DIR=\"${TEST_DATA_DIR}\"")

add_dependencies(${TARGET_NAME}
    test-data)

gtest_discover_tests(${TARGET_NAME}
    TEST_PREFIX "${TARGET_NAME}_")

enable_testing()
