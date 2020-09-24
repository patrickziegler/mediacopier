add_executable(mediacopier-cli
    "${CMAKE_CURRENT_SOURCE_DIR}/cli/src/main.cpp"
    )

set_target_properties(mediacopier-cli PROPERTIES
    OUTPUT_NAME mediacopier
    )

target_link_libraries(mediacopier-cli PRIVATE
    mediacopier
    )

if(${ENABLE_GUI})
    find_package(Qt5 COMPONENTS Widgets REQUIRED)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    add_library(mediacopier-qt STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/include/mediacopier/gui/dialog.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/src/dialog.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/src/dialog.ui"
        )

    target_include_directories(mediacopier-qt PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/include"
        )

    target_link_libraries(mediacopier-qt PUBLIC Qt5::Widgets mediacopier)
    target_link_libraries(mediacopier-cli PRIVATE mediacopier-qt)
endif()

if(${ENABLE_TEST})
    add_subdirectory(cli/test)
    enable_testing()
endif()
