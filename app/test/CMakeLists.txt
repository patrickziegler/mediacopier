find_package(GTest REQUIRED)

add_executable(mediacopier-component-test
    "${CMAKE_CURRENT_SOURCE_DIR}/test_example.cpp"
    )

target_link_libraries(mediacopier-component-test PRIVATE
    gtest gtest_main
    )

gtest_discover_tests(mediacopier-component-test)

enable_testing()
