find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
find_package(spdlog REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(TARGET_NAME mediacopier-gui)

add_executable(${TARGET_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/dialog.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/dialog.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/dialog.ui"
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/worker.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/worker.hpp")

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt5::Core Qt5::Widgets spdlog::spdlog ${MEDIACOPIER_LIB_TARGET_NAME})

install(TARGETS ${TARGET_NAME})
