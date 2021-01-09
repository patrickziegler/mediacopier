find_package(GTest REQUIRED)

add_subdirectory(data)

add_executable(mediacopier-component-test
    "${CMAKE_CURRENT_SOURCE_DIR}/test_example.cpp")

target_link_libraries(mediacopier-component-test PRIVATE
    gtest gtest_main)

add_dependencies(mediacopier-component-test
    mediacopier-test-data)

gtest_discover_tests(mediacopier-component-test
    TEST_PREFIX "component-test_")

enable_testing()
