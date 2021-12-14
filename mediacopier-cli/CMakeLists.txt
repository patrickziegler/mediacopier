
find_package(spdlog REQUIRED)


find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTORCC ON)

set(TARGET_NAME mediacopier-cli)
set(EXEC_NAME mediacopier)

add_executable(${TARGET_NAME})

set_target_properties(${TARGET_NAME} PROPERTIES
    OUTPUT_NAME ${EXEC_NAME})

target_sources(${TARGET_NAME} PRIVATE
    "src/main.cpp" "src/worker.cpp" "src/worker.hpp")

target_link_libraries(${TARGET_NAME} PUBLIC
    Qt5::Core Qt5::Widgets spdlog::spdlog ${MEDIACOPIER_LIB_TARGET_NAME})

install(TARGETS ${TARGET_NAME})

if(ENABLE_KDE)
    message("-- Enabling KDE integration")

    include(GNUInstallDirs)

    find_package(KF5JobWidgets REQUIRED)
    find_package(KF5I18n REQUIRED)

    add_definitions(-DENABLE_KDE)

    target_sources(${TARGET_NAME} PRIVATE
        "src/job.hpp" "src/job.cpp")

    target_link_libraries(${TARGET_NAME} PUBLIC
        KF5::I18n KF5::JobWidgets)

    set(DESKTOP_FILE_NAME mediacopier.desktop)

    configure_file("etc/${DESKTOP_FILE_NAME}.in"
        "${CMAKE_CURRENT_BINARY_DIR}/etc/${DESKTOP_FILE_NAME}")

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/etc/${DESKTOP_FILE_NAME}"
        DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/kservices5/ServiceMenus")
endif()
