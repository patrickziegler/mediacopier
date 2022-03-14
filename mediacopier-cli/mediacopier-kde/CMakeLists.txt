find_package(Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools)
find_package(KF5JobWidgets REQUIRED)
find_package(KF5I18n REQUIRED)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt5_create_translation(QM_FILES src/ lang/lang_de.ts OPTIONS "-noobsolete")

set(RESOURCES_FILE resources.qrc)

configure_file("${RESOURCES_FILE}.in" "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

set(TARGET_NAME mediacopier-cli-kde)

add_executable(${TARGET_NAME})

set(EXECUTABLE_NAME mediacopier-kde)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "src/ui/dialog_kde.cpp" "src/ui/dialog_kde.hpp" "src/ui/dialog_kde.ui" "src/ui/job.hpp" "src/ui/job.cpp" "src/main.cpp"
    ${QM_FILES} "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

target_link_libraries(${TARGET_NAME} PUBLIC
    KF5::I18n KF5::JobWidgets mediacopier-cli-common)

install(TARGETS ${TARGET_NAME})

set(DESKTOP_FILE_NAME mediacopier.desktop)

configure_file("${DESKTOP_FILE_NAME}.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/kservices5/ServiceMenus")
