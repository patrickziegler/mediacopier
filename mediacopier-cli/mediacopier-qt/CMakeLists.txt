find_package(Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt5_create_translation(QM_FILES src/ lang/lang_de.ts OPTIONS "-noobsolete")

set(RESOURCES_FILE resources.qrc)

configure_file("${RESOURCES_FILE}.in" "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

set(TARGET_NAME mediacopier-cli-qt)

add_executable(${TARGET_NAME})

set(EXECUTABLE_NAME mediacopier)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "src/gui/dialog.cpp" "src/gui/dialog.hpp" "src/gui/dialog.ui" "src/main.cpp"
    ${QM_FILES} "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

target_link_libraries(${TARGET_NAME} PUBLIC
    mediacopier-cli-common)

install(TARGETS ${TARGET_NAME})
