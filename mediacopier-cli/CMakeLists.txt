find_package(spdlog REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt5_create_translation(QM_FILES src/ lang/lang_de.ts OPTIONS "-noobsolete")

set(RESOURCES_FILE resources.qrc)

configure_file("${RESOURCES_FILE}.in" "${CMAKE_BINARY_DIR}/${RESOURCES_FILE}")

set(TARGET_NAME mediacopier-cli)

add_executable(${TARGET_NAME})

set(EXECUTABLE_NAME mediacopier)

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${EXECUTABLE_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "src/core/config.cpp" "src/core/config.hpp" "src/core/worker.cpp" "src/core/worker.hpp"
    "src/gui/dialog.cpp" "src/gui/dialog.hpp" "src/gui/dialog.ui"
    "src/gui/param.cpp" "src/gui/param.hpp" "src/gui/param.ui"
    "src/main.cpp"
    ${QM_FILES} "${CMAKE_BINARY_DIR}/${RESOURCES_FILE}")

target_include_directories(${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt5::Core Qt5::Widgets spdlog::spdlog
    ${MEDIACOPIER_LIB_TARGET_NAME})

install(TARGETS ${TARGET_NAME})

if(ENABLE_KDE)
    message("-- Enabling KDE integration")

    include(GNUInstallDirs)

    find_package(KF5JobWidgets REQUIRED)
    find_package(KF5I18n REQUIRED)

    add_definitions(-DENABLE_KDE)

    target_sources(${TARGET_NAME} PRIVATE
        "src/kde/KMediaCopierDialog.cpp" "src/kde/KMediaCopierDialog.hpp" "src/kde/KMediaCopierDialog.ui"
        "src/kde/KMediaCopierJob.hpp" "src/kde/KMediaCopierJob.cpp")

    target_link_libraries(${TARGET_NAME} PUBLIC
        KF5::I18n KF5::JobWidgets)

    set(DESKTOP_FILE_NAME mediacopier.desktop)

    configure_file("${DESKTOP_FILE_NAME}.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}")

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/kservices5/ServiceMenus")
endif()
