set(QT_LIBS)

if(ENABLE_QT6)
    find_package(Qt6 REQUIRED COMPONENTS Core Widgets StateMachine LinguistTools)
    list(APPEND QT_LIBS Qt6::Core Qt6::Widgets Qt6::StateMachine)
else()
    find_package(Qt5 REQUIRED COMPONENTS Core Widgets LinguistTools)
    list(APPEND QT_LIBS Qt5::Core Qt5::Widgets)
endif()

find_package(spdlog 1.9.2 REQUIRED) # qt_sink_* available since v1.9.2

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

qt_create_translation(QM_FILES source/ lang/lang_de.ts OPTIONS "-noobsolete")

set(RESOURCES_FILE resources.qrc)

configure_file("${RESOURCES_FILE}.in" "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

set(TARGET_NAME mediacopier-qt)

add_executable(${TARGET_NAME} ${CLI_TARGET_FLAGS})

set(EXECUTABLE_NAME ${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "source/config.cpp"
    "source/config.hpp"
    "source/main.cpp"
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
    spdlog::spdlog ${QT_LIBS} ${MEDIACOPIER_LIB_TARGET_NAME})

install(TARGETS ${TARGET_NAME})

if(${ENABLE_KDE})
    set(KF_LIBS)

    if(ENABLE_QT6)
        find_package(KF6I18n REQUIRED)
        find_package(KF6JobWidgets REQUIRED)
        list(APPEND KF_LIBS KF6::I18n KF6::JobWidgets)
        set(KF_VERSION ${KF6I18n_VERSION})
    else()
        find_package(KF5I18n REQUIRED)
        find_package(KF5JobWidgets REQUIRED)
        list(APPEND KF_LIBS KF5::I18n KF5::JobWidgets)
        set(KF_VERSION ${KF5I18n_VERSION})
    endif()

    include(GNUInstallDirs)

    target_compile_definitions(${TARGET_NAME} PUBLIC ENABLE_KDE)

    target_sources(${TARGET_NAME} PRIVATE
        "source/kde/KMediaCopierJob.cpp"
        "source/kde/KMediaCopierJob.hpp")

    target_link_libraries(${TARGET_NAME} PUBLIC
        ${KF_LIBS})

    set(DESKTOP_FILE_NAME "mediacopier.desktop")

    configure_file("source/kde/${DESKTOP_FILE_NAME}.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}")

    # install location has changed with version 5.85 according to [1], but [2] seems to contradict (?)
    # [1] https://develop.kde.org/docs/apps/dolphin/service-menus/#where-the-servicemenus-are-located
    # [2] https://invent.kde.org/documentation/develop-kde-org/-/issues/140
    if(${KF_VERSION} VERSION_GREATER_EQUAL "5.85")
        set(INSTALL_SERVICE_MENUS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/kio/servicemenus")
    else()
        set(INSTALL_SERVICE_MENUS_DIR "${CMAKE_INSTALL_DATAROOTDIR}/kservices5/ServiceMenus")
    endif()

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}"
        DESTINATION "${INSTALL_SERVICE_MENUS_DIR}")
endif()
