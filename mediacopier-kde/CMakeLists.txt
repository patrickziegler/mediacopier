find_package(KF5JobWidgets REQUIRED)
find_package(KF5I18n REQUIRED)
find_package(spdlog REQUIRED)

include(GNUInstallDirs)

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)

set(TARGET_NAME mediacopier-kde)
set(EXEC_NAME mediacopier)

add_executable(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${EXEC_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "src/job.hpp" "src/job.cpp" "src/main.cpp" "src/worker.cpp" "src/worker.hpp")

target_link_libraries(${TARGET_NAME} PUBLIC
    KF5::I18n KF5::JobWidgets spdlog::spdlog
    ${MEDIACOPIER_LIB_TARGET_NAME})

install(TARGETS ${TARGET_NAME})

set(DESKTOP_FILE_NAME mediacopier.desktop)

configure_file("${DESKTOP_FILE_NAME}.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE_NAME}"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/kservices5/ServiceMenus")
