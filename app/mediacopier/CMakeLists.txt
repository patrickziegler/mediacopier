find_package(spdlog REQUIRED)

set(TARGET_NAME mediacopier)

add_executable(${TARGET_NAME} ${CLI_TARGET_FLAGS})

set(EXECUTABLE_NAME ${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE "source/main.cpp")

target_link_libraries(${TARGET_NAME} PUBLIC
    ${MEDIACOPIER_UI_LIB})

install(TARGETS ${TARGET_NAME})
