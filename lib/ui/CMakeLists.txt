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

set(TARGET_NAME ${MEDIACOPIER_UI_LIB})

add_library(${TARGET_NAME} SHARED)

string(REPLACE "." "_" VERSION_WITH_UNDERSCORES ${CMAKE_PROJECT_VERSION})

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME mediacopier-ui-${VERSION_WITH_UNDERSCORES})

target_sources(${TARGET_NAME} PRIVATE
    "source/config.cpp"
    "source/config.hpp"
    "source/worker.cpp"
    "source/worker.hpp"
    "source/widgets/MediaCopierDialogFull.cpp"
    "source/widgets/MediaCopierDialogFull.hpp"
    "source/widgets/MediaCopierDialogFull.ui"
    "source/widgets/MediaCopierDialogSlim.cpp"
    "source/widgets/MediaCopierDialogSlim.hpp"
    "source/widgets/MediaCopierDialogSlim.ui"
    "source/widgets/MediaCopierLogWidget.cpp"
    "source/widgets/MediaCopierLogWidget.hpp"
    "source/widgets/MediaCopierLogWidget.ui"
    "source/widgets/MediaCopierParamWidget.cpp"
    "source/widgets/MediaCopierParamWidget.hpp"
    "source/widgets/MediaCopierParamWidget.ui"
    ${QM_FILES} "${CMAKE_CURRENT_BINARY_DIR}/${RESOURCES_FILE}")

target_include_directories(${TARGET_NAME} PUBLIC source/)

target_link_libraries(${TARGET_NAME} PUBLIC
    spdlog::spdlog ${QT_LIBS} ${MEDIACOPIER_CORE_LIB})

install(TARGETS ${TARGET_NAME})
