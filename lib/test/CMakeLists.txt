find_package(GTest)

add_subdirectory(data)

set(TARGET mediacopier-lib-test)

add_executable(${TARGET}
    "${CMAKE_CURRENT_SOURCE_DIR}/test_fileInfoClasses.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_fileOperationClasses.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_filePathFactory.cpp")

target_link_libraries(${TARGET} PRIVATE
    gtest gtest_main)

gtest_discover_tests(${TARGET}
    TEST_PREFIX "lib-test_")

enable_testing()
