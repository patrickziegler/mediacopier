add_subdirectory("${CMAKE_SOURCE_DIR}/extern/googletest" build)

find_package(GTest)

add_subdirectory(data)

set(TARGET_NAME mediacopier-unittest)

add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/test_fileInfoClasses.cpp")

target_include_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_SOURCE_DIR}/extern/date/include")

target_link_libraries(${TARGET_NAME} PRIVATE
    gtest gtest_main ${MEDIACOPIER_LIB_TARGET_NAME})

target_compile_definitions(${TARGET_NAME} PRIVATE "TEST_DATA_DIR=\"${TEST_DATA_DIR}\"")

add_dependencies(${TARGET_NAME}
    test-data)

gtest_discover_tests(${TARGET_NAME}
    TEST_PREFIX "${TARGET_NAME}_")

enable_testing()
