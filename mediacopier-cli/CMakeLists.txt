set(TARGET_NAME mediacopier-cli)

add_executable(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME mediacopier)

target_sources(${TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/src/cli/ConfigManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/cli/SequentialExecutor.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/main.cpp")

target_include_directories(${TARGET_NAME} PUBLIC
    "${CMAKE_CURRENT_LIST_DIR}")

target_link_libraries(${TARGET_NAME} PRIVATE
    ${MEDIACOPIER_LIB_TARGET_NAME})
