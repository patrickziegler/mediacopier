find_package(spdlog REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt5_create_translation(QM_FILES src/ lang/lang_de.ts OPTIONS "-noobsolete")

set(RESOURCES_FILE common.qrc)

configure_file("${RESOURCES_FILE}.in" "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

set(TARGET_NAME mediacopier-cli-common)

add_library(${TARGET_NAME} STATIC)

target_sources(${TARGET_NAME} PRIVATE
    "src/config.cpp" "src/config.hpp" "src/worker.cpp" "src/worker.hpp"
    "src/ui/param.cpp" "src/ui/param.hpp" "src/ui/param.ui"
    ${QM_FILES} "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

target_include_directories(${TARGET_NAME} PUBLIC src/)

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt5::Core Qt5::Widgets spdlog::spdlog
    ${MEDIACOPIER_LIB_TARGET_NAME})
