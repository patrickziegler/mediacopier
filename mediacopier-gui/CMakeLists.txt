find_package(Qt5 COMPONENTS Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(TARGET_NAME mediacopier-gui)

add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME}
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dialog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/dialog.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/worker.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/worker.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ui/dialog.ui")

target_include_directories(${TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt5::Widgets ${MEDIACOPIER_LIB_TARGET_NAME})
