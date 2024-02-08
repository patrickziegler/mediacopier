find_package(CLI11 REQUIRED)
find_package(spdlog 1.9.2 REQUIRED)
find_package(toml11 REQUIRED)

set(TARGET_NAME mediacopier-cli)

add_executable(${TARGET_NAME})

set(EXECUTABLE_NAME mediacopier)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "source/config.cpp"
    "source/config.hpp"
    "source/main.cpp")

target_link_libraries(${TARGET_NAME} PUBLIC
    spdlog::spdlog ${MEDIACOPIER_LIB_TARGET_NAME})

install(TARGETS ${TARGET_NAME})
