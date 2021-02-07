find_package(GTest)

set(TARGET mediacopier-app-test)

add_executable(${TARGET}
    "${CMAKE_CURRENT_SOURCE_DIR}/test_config.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_feedback.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_run.cpp")

target_link_libraries(${TARGET} PRIVATE
    gtest gtest_main mediacopier-cli)

add_dependencies(${TARGET}
    ${TEST_DATA_TARGET})

gtest_discover_tests(${TARGET}
    TEST_PREFIX "app-test_")

enable_testing()
