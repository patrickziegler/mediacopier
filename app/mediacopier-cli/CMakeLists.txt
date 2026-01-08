find_package(CLI11 REQUIRED)
find_package(spdlog REQUIRED)

set(TARGET_NAME mediacopier-cli)

add_executable(${TARGET_NAME} ${CLI_TARGET_FLAGS})

target_sources(${TARGET_NAME} PRIVATE
    "source/cli.cpp"
    "source/cli.hpp"
    "source/main.cpp")

target_link_libraries(${TARGET_NAME} PUBLIC
    spdlog::spdlog ${MEDIACOPIER_CORE_LIB})

install(TARGETS ${TARGET_NAME})

register_code_format_target(${TARGET_NAME})
