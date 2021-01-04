find_package(log4cplus)

# -- mediacopier-main (executable)

add_executable(mediacopier-main
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")

set_target_properties(mediacopier-main PROPERTIES
    OUTPUT_NAME mediacopier)

# -- mediacopier-cli (static library)

add_library(mediacopier-cli STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/cli/src/config.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cli/src/feedback.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cli/src/run.cpp")

target_include_directories(mediacopier-cli PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/cli/include")

target_link_libraries(mediacopier-cli PUBLIC
    "${LOG4CPLUS_LIBRARY}"
    mediacopier)

target_link_libraries(mediacopier-main PRIVATE
    mediacopier-cli)

# -- mediacopier-gui (static library)

if(${ENABLE_GUI})
    find_package(Qt5 COMPONENTS Widgets REQUIRED)

    add_definitions(-DENABLE_GUI)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
    set(CMAKE_AUTORCC ON)

    add_library(mediacopier-gui STATIC
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/include/mediacopier/gui/dialog.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/include/mediacopier/gui/worker.hpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/src/dialog.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/src/dialog.ui"
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/src/worker.cpp")

    target_include_directories(mediacopier-gui PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/gui/include")

    target_link_libraries(mediacopier-gui PUBLIC
        Qt5::Widgets mediacopier-cli)

    target_link_libraries(mediacopier-main PRIVATE
        mediacopier-gui)
endif()

# -- test

if(${ENABLE_TEST})
    add_subdirectory(test)
    enable_testing()
endif()
