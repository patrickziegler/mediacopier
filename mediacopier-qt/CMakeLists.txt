find_package(Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools)
find_package(spdlog 1.9.2 REQUIRED) # qt_sink_* available since v1.9.2
find_package(toml11 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt5_create_translation(QM_FILES source/ lang/lang_de.ts OPTIONS "-noobsolete")

set(RESOURCES_FILE resources.qrc)

configure_file("${RESOURCES_FILE}.in" "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

set(TARGET_NAME mediacopier-qt)

add_executable(${TARGET_NAME} ${CLI_TARGET_FLAGS})

set(EXECUTABLE_NAME ${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "source/config.cpp"
    "source/config.hpp"
    "source/main.cpp"
    "source/status.cpp"
    "source/status.hpp"
    "source/worker.cpp"
    "source/worker.hpp"
    "source/gui/MediaCopierDialogFull.cpp"
    "source/gui/MediaCopierDialogFull.hpp"
    "source/gui/MediaCopierDialogFull.ui"
    "source/gui/MediaCopierDialogSlim.cpp"
    "source/gui/MediaCopierDialogSlim.hpp"
    "source/gui/MediaCopierDialogSlim.ui"
    "source/gui/MediaCopierLogWidget.cpp"
    "source/gui/MediaCopierLogWidget.hpp"
    "source/gui/MediaCopierLogWidget.ui"
    "source/gui/MediaCopierParamWidget.cpp"
    "source/gui/MediaCopierParamWidget.hpp"
    "source/gui/MediaCopierParamWidget.ui"
    ${QM_FILES} "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

target_include_directories(${TARGET_NAME} PUBLIC source/)

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt5::Core Qt5::Widgets spdlog::spdlog
    ${MEDIACOPIER_LIB_TARGET_NAME})

install(TARGETS ${TARGET_NAME})

if(${ENABLE_KDE})
    find_package(KF5JobWidgets REQUIRED)
    find_package(KF5I18n REQUIRED)

    include(GNUInstallDirs)

    target_compile_definitions(${TARGET_NAME} PUBLIC ENABLE_KDE)

    target_sources(${TARGET_NAME} PRIVATE
        "source/kde/KMediaCopierJob.cpp"
        "source/kde/KMediaCopierJob.hpp")

    target_link_libraries(${TARGET_NAME} PUBLIC
        KF5::I18n KF5::JobWidgets)

    set(DESKTOP_FILE_NAME "mediacopier.desktop")

    configure_file("source/kde/${DESKTOP_FILE_NAME}.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}")

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/kservices5/ServiceMenus")
endif()
