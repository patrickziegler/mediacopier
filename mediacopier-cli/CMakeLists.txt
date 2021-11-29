find_package(range-v3 REQUIRED)
find_package(spdlog REQUIRED)

set(TARGET_NAME mediacopier-cli)
set(EXEC_NAME mediacopier)

add_executable(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${EXEC_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_LIST_DIR}/config.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/main.cpp")

target_link_libraries(${TARGET_NAME} PRIVATE
    ${MEDIACOPIER_LIB_TARGET_NAME}
    range-v3 spdlog::spdlog)

install(TARGETS ${TARGET_NAME})
