find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

add_library(mediacopier-gui STATIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mediacopier/gui/dialog.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/mediacopier/gui/worker.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dialog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dialog.ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/worker.cpp")

target_include_directories(mediacopier-gui PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(mediacopier-gui PUBLIC
    Qt5::Widgets mediacopier-cli)
